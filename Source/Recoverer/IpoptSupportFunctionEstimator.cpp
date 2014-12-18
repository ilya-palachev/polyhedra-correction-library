/*
 * Copyright (c) 2009-2014 Ilya Palachev <iliyapalachev@gmail.com>
 *
 * This file is part of Polyhedra Correction Library.
 *
 * Polyhedra Correction Library is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Polyhedra Correction Library is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Polyhedra Correction Library.
 * If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file IpoptSupportFunctionEstimator.h
 * @brief Definition of Iptopt-based non-linear optimizer in case of support
 * function estimation.
 */

#ifdef USE_IPOPT

#include "DebugPrint.h"
#include "DebugAssert.h"
#include <coin/IpIpoptApplication.hpp>
#include "Recoverer/IpoptSupportFunctionEstimator.h"

/*
 * TODO: Implement dual problem here!
 */

IpoptSupportFunctionEstimator::IpoptSupportFunctionEstimator(
		SupportFunctionEstimationDataPtr data,
		IpoptEstimationMode modeOrig) :
	SupportFunctionEstimator(data),
	mode(modeOrig),
	problemType(PROBLEM_PRIMAL),
	solution(data->numValues())
{
	DEBUG_START;
	DEBUG_END;
}

IpoptSupportFunctionEstimator::~IpoptSupportFunctionEstimator()
{
	DEBUG_START;
	DEBUG_END;
}

bool IpoptSupportFunctionEstimator::get_nlp_info(Index& n, Index& m,
		Index& nnz_jac_g, Index& nnz_h_lag, IndexStyleEnum& index_style)
{
	DEBUG_START;

	switch (mode)
	{
	case IPOPT_ESTIMATION_QUADRATIC:
		/*
		 * The number of variables is equal to the number of "values".
		 */
		n = data->numValues();

		/*
		 * The number of conditions is also saved in data structure.
		 */
		m = data->numConditions();

		/*
		 * The Jacobian of conditions is actually equal to the support
		 * matrix, thus they have the same number of non-zero elements.
		 */
		nnz_jac_g = data->supportMatrix().nonZeros();

		/*
		 * The Hessian of Lagrangian is a quadratci functional, which
		 * quadratic part is the sum of ((u_i, x_i) - h_i)^2. Thus, the
		 * partial derivative on x_i_s and x_i_t is 2 * u_i_s * u_i_t.
		 * That means that the Hessian is consisted of blocks of size
		 * 3x3 for each support direction.
		 */
		nnz_h_lag = (data->numValues() / 3) * 9;
		break;
	case IPOPT_ESTIMATION_LINEAR:
		/*
		 * For linear problem there is 1 additional variable - 
		 * \varepsilon
		 */
		n = data->numValues() + 1;

		/*
		 * For linear problem there are 2 additional conditions for each
		 * support direction u_i:
		 *
		 * |(u_i, x_i) - h_i| < \varepsilon, which is equal to
		 *
		 * (u_i, x_i) - h_i > -\varepsilon
		 * (u_i, x_i) - h_i <  \varepsilon
		 *
		 * The number if directions is n / 3, so
		 * there are (n / 3) * 2 additional conditions.
		 */
		m = data->numConditions() + (data->numValues() / 3) * 2;

		/*
		 * The Jacobian of conditions is actually a support matrix
		 * extended with additional rows. Each row has 4 non-zero
		 * coefficients: u_i_x, u_i_y, u_i_z, \varepsilon.
		 *
		 * Thus we have (n / 3) * 4 additional non-zero values in the
		 * Jacobian of conditions.
		 */
		nnz_jac_g = data->supportMatrix().nonZeros()
			+ (data->numValues() / 3) * 4;

		/*
		 * Th Lagrangian is a linear function of x_i_s and \varepsilon.
		 * Thus, all its partial derivatives of 2nd order are zero. That
		 * means that the Hessian of Lagrangian is a zero matrix.
		 */
		nnz_h_lag = 0;
		break;
	}

	DEBUG_PRINT("Number of variables is set to n = %d", n);
	DEBUG_PRINT("Number of constraints is set to m = %d", m);
	DEBUG_PRINT("Number of non-zero elements in the Jacobian of g is set to "
			"nnz_jac_g = %d", nnz_jac_g);
	DEBUG_PRINT("Number of non-zero elements in the Hessian is set to "
			"nnz_h_lag = %d", nnz_h_lag);

	index_style = TNLP::C_STYLE;
	DEBUG_END;
	return true;
}

#define TNLP_INFINITY 2e19

bool IpoptSupportFunctionEstimator::get_bounds_info(Index n, Number* x_l,
		Number* x_u, Index m, Number* g_l, Number* g_u)
{
	DEBUG_START;

	ASSERT(x_l);
	ASSERT(x_u);
	ASSERT(g_l);
	ASSERT(g_u);
	if (mode == IPOPT_ESTIMATION_QUADRATIC)
	{
		ASSERT(n == data->numValues());
	}
	if (mode == IPOPT_ESTIMATION_LINEAR)
	{
		ASSERT(n == data->numValues() + 1);
	}

	/* There are no restrictions on variables: */
	for (int i = 0; i < n; ++i)
	{
		x_l[i] = -TNLP_INFINITY;
		x_u[i] = +TNLP_INFINITY;
	}

	/* All conditions are tranformed to the form (*) > 0: */
	for (int i = 0; i < m; ++i)
	{
		g_l[i] = 0.;
		g_u[i] = +TNLP_INFINITY;
	}
	DEBUG_END;
	return true;
}

bool IpoptSupportFunctionEstimator::get_starting_point(Index n, bool init_x,
		Number* x, bool init_z, Number* z_L, Number* z_U, Index m,
		bool init_lambda, Number* lambda)
{
	DEBUG_START;

	ASSERT(x);
	if (mode == IPOPT_ESTIMATION_QUADRATIC)
	{
		ASSERT(n == data->numValues());
	}
	if (mode == IPOPT_ESTIMATION_LINEAR)
	{
		ASSERT(n == data->numValues() + 1);
	}
	

	if (init_x)
	{
		VectorXd x0 = data->startingVector();
		int num = data->numValues();
		for (int i = 0; i < num; ++i)
		{
			x[i] = x0(i);
		}
		if (mode == IPOPT_ESTIMATION_LINEAR)
		{
			std::vector<Vector3d> directions =
				data->supportDirections();
			/*
			 * It is supposed that given starting point lies in the 
			 * consistency area, thus we can get estimate for
			 * \varepsilon from it.
			 */
			double maxDifference = 0.;
			for (int i = 0; i < num / 3; ++i)
			{
				double difference = directions[i].x * x0(3 * i)
					+ directions[i].y * x0(3 * i + 1)
					+ directions[i].z * x0(3 * i + 2);
				if (difference > maxDifference)
				{
					maxDifference = difference;
				}
			}
			x[num] = maxDifference;
		}
		
	}
	if (init_z || init_lambda)
	{
		ASSERT(0 && "Not implemented yet");
	}
	DEBUG_END;
	return true;
}

bool IpoptSupportFunctionEstimator::eval_f(Index n, const Number* x, bool new_x,
		Number& obj_value)
{
	DEBUG_START;

	ASSERT(x);
	VectorXd x0 = data->supportVector();
	std::vector<Vector3d> directions = data->supportDirections();
	if (mode == IPOPT_ESTIMATION_QUADRATIC)
	{
		ASSERT(n == data->numValues());
	}
	if (mode == IPOPT_ESTIMATION_LINEAR)
	{
		ASSERT(n == data->numValues() + 1);
	}

	switch (mode)
	{
	case IPOPT_ESTIMATION_QUADRATIC:
		obj_value = 0.;
		for (int i = 0; i < n / 3; ++i)
		{
			double difference = directions[i].x * x[3 * i]
				+ directions[i].y * x[3 * i + 1]
				+ directions[i].z * x[3 * i + 2] - x0(i);
			obj_value += difference * difference;
		}
		break;
	case IPOPT_ESTIMATION_LINEAR:
		obj_value = x[data->numValues()]; /* epsilon */
		break;
	}
	DEBUG_END;
	return true;
}

bool IpoptSupportFunctionEstimator::eval_grad_f(Index n, const Number* x,
		bool new_x, Number* grad_f)
{
	DEBUG_START;

	ASSERT(x);
	ASSERT(grad_f);
	if (mode == IPOPT_ESTIMATION_QUADRATIC)
	{
		ASSERT(n == data->numValues());
	}
	if (mode == IPOPT_ESTIMATION_LINEAR)
	{
		ASSERT(n == data->numValues() + 1);
	}
	VectorXd x0 = data->supportVector();
	int num = data->numValues();
	std::vector<Vector3d> directions = data->supportDirections();

	switch (mode)
	{
	case IPOPT_ESTIMATION_QUADRATIC:
		for (int i = 0; i < num / 3; ++i)
		{
			double multiplier = 2. * (directions[i].x * x[3 * i]
				+ directions[i].y * x[3 * i + 1]
				+ directions[i].z * x[3 * i + 2] - x0(i));
			grad_f[3 * i] = multiplier * directions[i].x;
			grad_f[3 * i + 1] = multiplier * directions[i].y;
			grad_f[3 * i + 2] = multiplier * directions[i].z;
		}
		break;
	case IPOPT_ESTIMATION_LINEAR:
		for (int i = 0; i < num; ++i)
		{
			grad_f[i] = 0.;
		}
		grad_f[num] = 1.;
		break;
	}

	DEBUG_END;
	return true;
}

bool IpoptSupportFunctionEstimator::eval_g(Index n, const Number* x, bool new_x,
		Index m, Number* g)
{
	DEBUG_START;

	ASSERT(x);
	ASSERT(g);
	if (mode == IPOPT_ESTIMATION_QUADRATIC)
	{
		ASSERT(n == data->numValues());
	}
	if (mode == IPOPT_ESTIMATION_LINEAR)
	{
		ASSERT(n == data->numValues() + 1);
	}
	int num = data->numValues();

	VectorXd h(num);
	for (int i = 0; i < num; ++i)
	{
		h(i) = x[i];
	}

	VectorXd Qh(m);
	Qh = data->supportMatrix() * h;
	VectorXd h0 = data->supportVector();
	std::vector<Vector3d> directions = data->supportDirections();

	int numConds = data->numConditions();
	for (int i = 0; i < numConds; ++i)
	{
		g[i] = Qh[i];
	}
	if (mode == IPOPT_ESTIMATION_LINEAR)
	{
		for (int i = 0 ; i < num / 3; ++i)
		{
			double product = directions[i].x * h(3 * i)
				+ directions[i].y * h(3 * i + 1)
				+ directions[i].z * h(3 * i + 2);
			g[numConds + 2 * i] = product + x[num] - h0(i);
			g[numConds + 2 * i + 1] = -product + x[num] + h0(i);
		}
	}

	DEBUG_END;
	return true;
}

bool IpoptSupportFunctionEstimator::eval_jac_g(Index n, const Number* x,
		bool new_x, Index m, Index n_ele_jac, Index* iRow, Index *jCol,
		Number* values)
{
	DEBUG_START;

	DEBUG_PRINT("Input parameters:");
	DEBUG_PRINT("   n = %d", n);
	DEBUG_PRINT("   x = %p", (void*) x);
	DEBUG_PRINT("   new_x = %d", new_x);
	DEBUG_PRINT("   m = %d", m);
	DEBUG_PRINT("   n_ele_jac = %d", n_ele_jac);
	DEBUG_PRINT("   iRow = %p", (void*) iRow);
	DEBUG_PRINT("   jCol = %p", (void*) jCol);
	DEBUG_PRINT("   values = %p", (void*) values);\
	int num = data->numValues();

	if (!values)
	{
		ASSERT(iRow);
		ASSERT(jCol);
	}
	if (new_x)
	{
		ASSERT(values);
	}
	if (mode == IPOPT_ESTIMATION_QUADRATIC)
	{
		ASSERT(n == data->numValues());
	}
	if (mode == IPOPT_ESTIMATION_LINEAR)
	{
		ASSERT(n == data->numValues() + 1);
	}

	SparseMatrix Q = data->supportMatrix();
	if (mode == IPOPT_ESTIMATION_QUADRATIC)
	{
		ASSERT(n_ele_jac == Q.nonZeros());
	}
	if (mode == IPOPT_ESTIMATION_LINEAR)
	{
		ASSERT(n_ele_jac == Q.nonZeros() + num * 4);
	}
	int nonZeros = Q.nonZeros();
	int numConds = data->numConditions();

	/*
	 * Assign all values of sparsity structure to -1 - this will be used for
	 * further checks that all values where correctly initialized.
	 */
	if (!values)
	{
		for (int i = 0; i < n_ele_jac; ++i)
		{
			iRow[i] = jCol[i] = -1;
		}
	}

	int i = 0;
	for (int k = 0; k < Q.outerSize(); ++k)
		for (SparseMatrix::InnerIterator it(Q, k); it; ++it)
		{
			if (values)
			{
				/* Inform about the values of Lagrangian of g */
				values[i] = it.value();
			}
			else
			{
				/*
				 * Inform about the sparsity structure of the 
				 * Lagrangian of g
				 */
				iRow[i] = it.row();
				jCol[i] = it.col();
			}
			++i;
		}
	ASSERT(i == nonZeros);

	if (mode == IPOPT_ESTIMATION_LINEAR)
	{
		auto directions = data->supportDirections();
		for (i = 0; i < num / 3; ++i)
		{
			if (values)
			{
				values[nonZeros + 8 * i] = directions[i].x;
				values[nonZeros + 8 * i + 1] = directions[i].y;
				values[nonZeros + 8 * i + 2] = directions[i].z;
				values[nonZeros + 8 * i + 3] = 1.;
				values[nonZeros + 8 * i + 4] = -directions[i].x;
				values[nonZeros + 8 * i + 5] = -directions[i].y;
				values[nonZeros + 8 * i + 6] = -directions[i].z;
				values[nonZeros + 8 * i + 7] = 1.;
			}
			else
			{
				iRow[nonZeros + 8 * i] =
					iRow[nonZeros + 8 * i + 1] =
					iRow[nonZeros + 8 * i + 2] =
					iRow[nonZeros + 8 * i + 3] =
					numConds + 2 * i;
				jCol[nonZeros + 8 * i] = 3 * i;
				jCol[nonZeros + 8 * i + 1] = 3 * i + 1;
				jCol[nonZeros + 8 * i + 2] = 3 * i + 2;
				jCol[nonZeros + 8 * i + 3] = num; /* epsilon */

				iRow[nonZeros + 8 * i + 4] =
					iRow[nonZeros + 8 * i + 1] =
					iRow[nonZeros + 8 * i + 2] =
					iRow[nonZeros + 8 * i + 3] =
					numConds + 2 * i + 1;
				jCol[nonZeros + 8 * i + 4] = 3 * i;
				jCol[nonZeros + 8 * i + 1] = 3 * i + 1;
				jCol[nonZeros + 8 * i + 2] = 3 * i + 2;
				jCol[nonZeros + 8 * i + 3] = num; /* epsilon */
			}
		}
	}

	/*
	 * Check that all values of sparsity structure have been correctly set.
	 */
	if (!values)
	{
		for (int i = 0; i < n_ele_jac; ++i)
		{
			DEBUG_PRINT("Checking element #%d", i);
			ASSERT(iRow[i] != -1);
			ASSERT(iRow[i] >= 0);
			ASSERT(jCol[i] != -1);
			ASSERT(jCol[i] >= 0);
			if (mode == IPOPT_ESTIMATION_QUADRATIC)
			{
				ASSERT(iRow[i] < Q.rows());
				ASSERT(jCol[i] < Q.cols());
			}
			if (mode == IPOPT_ESTIMATION_LINEAR)
			{
				ASSERT(iRow[i] < Q.rows() + 2 * (num / 3));
				ASSERT(jCol[i] < Q.cols() + 1);
			}
		}
	}
	DEBUG_END;
	return true;
}

bool IpoptSupportFunctionEstimator::eval_h(Index n, const Number* x, bool new_x,
		Number obj_factor, Index m, const Number* lambda,
		bool new_lambda, Index n_ele_hess, Index* iRow, Index* jCol,
		Number* values)
{
	DEBUG_START;

	DEBUG_PRINT("Input parameters:");
	DEBUG_PRINT("   n = %d", n);
	DEBUG_PRINT("   x = %p", (void*) x);
	DEBUG_PRINT("   new_x = %d", new_x);
	DEBUG_PRINT("   obj_factor = %lf", obj_factor);
	DEBUG_PRINT("   m = %d", m);
	DEBUG_PRINT("   lambda = %p", (void*) lambda);
	DEBUG_PRINT("   new_lambda = %d", new_lambda);
	DEBUG_PRINT("   n_ele_hess = %d", n_ele_hess);
	DEBUG_PRINT("   iRow = %p", (void*) iRow);
	DEBUG_PRINT("   jCol = %p", (void*) jCol);
	DEBUG_PRINT("   values = %p", (void*) values);

	if (mode == IPOPT_ESTIMATION_QUADRATIC)
	{
		ASSERT(n == data->numValues());
	}
	if (mode == IPOPT_ESTIMATION_LINEAR)
	{
		ASSERT(n == data->numValues() + 1);
	}
	if (mode == IPOPT_ESTIMATION_LINEAR)
	{
		/* Hessian is zero matrix in linear case. */
		DEBUG_END;
		return true;
	}
	ASSERT(n_ele_hess == data->numValues() * 3);

	if (new_x)
		ASSERT(values);

	int numSupportDirections = data->numValues() / 3;
	if (!values)
	{
		/* Set sparsity structure of the Hessian. */
		ASSERT(iRow);
		ASSERT(jCol);
		for (int i = 0; i < numSupportDirections; ++i)
		{
			/*
			 * The Hessian consists of 3x3 blocks for each support
			 * direction.
			 */
			iRow[9 * i] = 3 * i;
			jCol[9 * i] = 3 * i;
			iRow[9 * i + 1] = 3 * i;
			jCol[9 * i + 1] = 3 * i + 1;
			iRow[9 * i + 2] = 3 * i;
			jCol[9 * i + 2] = 3 * i + 2;

			iRow[9 * i + 3] = 3 * i + 1;
			jCol[9 * i + 3] = 3 * i;
			iRow[9 * i + 4] = 3 * i + 1;
			jCol[9 * i + 4] = 3 * i + 1;
			iRow[9 * i + 5] = 3 * i + 1;
			jCol[9 * i + 5] = 3 * i + 2;

			iRow[9 * i + 6] = 3 * i + 2;
			jCol[9 * i + 6] = 3 * i;
			iRow[9 * i + 7] = 3 * i + 2;
			jCol[9 * i + 7] = 3 * i + 1;
			iRow[9 * i + 8] = 3 * i + 2;
			jCol[9 * i + 8] = 3 * i + 2;
		}
	}
	else
	{
		auto directions = data->supportDirections();
		ASSERT(directions.size() == numSupportDirections);
		/* Set values of the Hessian. */
		for (int i = 0; i < numSupportDirections; ++i)
		{
			values[9 * i] = 2. * obj_factor *
				directions[i].x * directions[i].x;
			values[9 * i + 1] = 2. * obj_factor *
				directions[i].x * directions[i].y;
			values[9 * i + 2] = 2. * obj_factor *
				directions[i].x * directions[i].z;
			values[9 * i + 3] = 2. * obj_factor *
				directions[i].y * directions[i].x;
			values[9 * i + 4] = 2. * obj_factor *
				directions[i].y * directions[i].y;
			values[9 * i + 5] = 2. * obj_factor *
				directions[i].y * directions[i].z;
			values[9 * i + 6] = 2. * obj_factor *
				directions[i].z * directions[i].x;
			values[9 * i + 7] = 2. * obj_factor *
				directions[i].z * directions[i].y;
			values[9 * i + 8] = 2. * obj_factor *
				directions[i].z * directions[i].z;
		}
	}

	DEBUG_END;
	return true;
}

void IpoptSupportFunctionEstimator::finalize_solution(SolverReturn status,
		Index n, const Number* x, const Number* z_L, const Number* z_U, Index m,
		const Number* g, const Number* lambda, Number obj_value,
		const IpoptData* ip_data, IpoptCalculatedQuantities* ip_cq)
{
	DEBUG_START;
	if (mode == IPOPT_ESTIMATION_QUADRATIC)
	{
		ASSERT(n == data->numValues());
	}
	if (mode == IPOPT_ESTIMATION_LINEAR)
	{
		ASSERT(n == data->numValues() + 1);
	}

	switch (status)
	{
	case SUCCESS:
		MAIN_PRINT("SUCCESS");
		for (int i = 0; i < data->numValues(); ++i)
		{
			solution(i) = x[i];
		}
		break;
	case MAXITER_EXCEEDED:
		MAIN_PRINT("MAXITER_EXCEEDED");
		break;
	case CPUTIME_EXCEEDED:
		MAIN_PRINT("CPUTIME_EXCEEDED");
		break;
	case STOP_AT_TINY_STEP:
		MAIN_PRINT("STOP_AT_TINY_STEP");
		break;
	case STOP_AT_ACCEPTABLE_POINT:
		MAIN_PRINT("STOP_AT_ACCEPTABLE_POINT");
		break;
	case LOCAL_INFEASIBILITY:
		MAIN_PRINT("LOCAL_INFEASIBILITY");
		break;
	case USER_REQUESTED_STOP:
		MAIN_PRINT("USER_REQUESTED_STOP");
		break;
	case FEASIBLE_POINT_FOUND:
		MAIN_PRINT("FEASIBLE_POINT_FOUND");
		break;
	case DIVERGING_ITERATES:
		MAIN_PRINT("DIVERGING_ITERATES");
		break;
	case RESTORATION_FAILURE:
		MAIN_PRINT("RESTORATION_FAILURE");
		break;
	case ERROR_IN_STEP_COMPUTATION:
		MAIN_PRINT("ERROR_IN_STEP_COMPUTATION");
		break;
	case INVALID_NUMBER_DETECTED:
		MAIN_PRINT("INVALID_NUMBER_DETECTED");
		break;
	case TOO_FEW_DEGREES_OF_FREEDOM:
		MAIN_PRINT("TOO_FEW_DEGREES_OF_FREEDOM");
		break;
	case INVALID_OPTION:
		MAIN_PRINT("INVALID_OPTION");
		break;
	case OUT_OF_MEMORY:
		MAIN_PRINT("OUT_OF_MEMORY");
		break;
	case INTERNAL_ERROR:
		MAIN_PRINT("INTERNAL_ERROR");
		break;
	case UNASSIGNED:
		MAIN_PRINT("UNASSIGNED");
		break;
	}

	DEBUG_END;
	return;
}

VectorXd IpoptSupportFunctionEstimator::run(void)
{
	DEBUG_START;
	SmartPtr<IpoptApplication> app = IpoptApplicationFactory();

	/* Intialize the IpoptApplication and process the options */
	ApplicationReturnStatus status;
	status = app->Initialize();
	if (status != Solve_Succeeded)
	{
		MAIN_PRINT("*** Error during initialization!");
		return solution;
	}

	app->Options()->SetNumericValue("tol", 1e-10);
	app->Options()->SetNumericValue("acceptable_tol", 1e-10);

	/* Ask Ipopt to solve the problem */
	status = app->OptimizeTNLP(this);
	if (status == Solve_Succeeded)
	{
		MAIN_PRINT("*** The problem solved!");
	}
	else
	{
		MAIN_PRINT("** The problem FAILED!");
	}

	DEBUG_END;
	return solution;
}

#endif /* USE_IPOPT */
