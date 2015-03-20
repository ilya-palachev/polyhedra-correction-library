/*
 * Copyright (c) 2009-2015 Ilya Palachev <iliyapalachev@gmail.com>
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

IpoptSupportFunctionEstimator::IpoptSupportFunctionEstimator(
		SupportFunctionEstimationDataPtr data) :
	SupportFunctionEstimator(data),
	problemType_(DEFAULT_ESTIMATION_PROBLEM_NORM),
	numVariablesX(),
	numVariablesEpsilon(),
	numConsistencyConditions(),
	numLocalityConditions(),
	solution()
{
	DEBUG_START;
	numVariablesX = data->numValues();
	numVariablesEpsilon = 
		(problemType_ == ESTIMATION_PROBLEM_NORM_L_INF)
		? 1 : (numVariablesX / 3);
	numConsistencyConditions = data->numConditions();
	numLocalityConditions = 2 * (numVariablesX / 3);
	DEBUG_END;
}

IpoptSupportFunctionEstimator::~IpoptSupportFunctionEstimator()
{
	DEBUG_START;
	DEBUG_END;
}

void IpoptSupportFunctionEstimator::setProblemType(EstimationProblemNorm type)
{
	DEBUG_START;
	problemType_ = type;
	numVariablesEpsilon = 
		(problemType_ == ESTIMATION_PROBLEM_NORM_L_INF)
		? 1 : (numVariablesX / 3);
	DEBUG_END;
}

bool IpoptSupportFunctionEstimator::get_nlp_info(Index& n, Index& m,
		Index& nnz_jac_g, Index& nnz_h_lag, IndexStyleEnum& index_style)
{
	DEBUG_START;
	n = numVariablesX + numVariablesEpsilon;
	DEBUG_PRINT("Number of variables is set to n = %d", n);
	
	m = numConsistencyConditions + numLocalityConditions;
	DEBUG_PRINT("Number of constraints is set to m = %d", m);
	
	nnz_jac_g = data->supportMatrix().nonZeros()
		+ 4 * numLocalityConditions;
	DEBUG_PRINT("Number of non-zero elements in the Jacobian of g is set to"
			" nnz_jac_g = %d", nnz_jac_g);
	
	nnz_h_lag = problemType_ == ESTIMATION_PROBLEM_NORM_L_2
		? numVariablesX / 3 : 0;
	DEBUG_PRINT("Number of non-zero elements in the Hessian is set to"
			" nnz_h_lag = %d", nnz_h_lag);

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
	ASSERT(n == numVariablesX + numVariablesEpsilon);
	ASSERT(m = numConsistencyConditions + numLocalityConditions);

	/* There are no restrictions on regular variables: */
	for (int i = 0; i < numVariablesX; ++i)
	{
		x_l[i] = -TNLP_INFINITY;
		x_u[i] = +TNLP_INFINITY;
	}
	/* But there are restrictions on epsilons: */
	double epsilon = data->startingEpsilon();
	for (int i = numVariablesX; i < numVariablesX + numVariablesEpsilon;
			++i)
	{
		x_l[i] = 0.;
		x_u[i] = epsilon;
	}
	/* All consistency conditions should only be non-negative: */
	for (int i = 0; i < numConsistencyConditions; ++i)
	{
		g_l[i] = 0.;
		g_u[i] = +TNLP_INFINITY;
	}
	/* All locality conditions have bounds depending on h0: */
	auto h0 = data->supportVector();
	for (int i = numConsistencyConditions;
			i < numConsistencyConditions + numLocalityConditions;
			++i)
	{
		int iDirection = (i - numConsistencyConditions) / 2;
		g_l[i] = ((i - numConsistencyConditions) % 2)
			? -h0(iDirection) : h0(iDirection);
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
	ASSERT(n == numVariablesX + numVariablesEpsilon);
	ASSERT(m = numConsistencyConditions + numLocalityConditions);
	if (init_x)
	{
		VectorXd x0 = data->startingVector();
		double espilon = data->startingEpsilon();
		for (int i = 0; i < numVariablesX; ++i)
		{
			x[i] = x0(i);
		}
		for (int i = numVariablesX;
				i < numVariablesX + numVariablesEpsilon; ++i)
		{
			x[i] = espilon;
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
	ASSERT(n == numVariablesX + numVariablesEpsilon);
	switch(problemType_)
	{
	case ESTIMATION_PROBLEM_NORM_L_INF:
		obj_value = x[numVariablesX];
		break;
	case ESTIMATION_PROBLEM_NORM_L_1:
		obj_value = 0.;
		for (int i = numVariablesX;
				i < numVariablesX + numVariablesEpsilon; ++i)
		{
			obj_value += x[i];
		}
		break;
	case ESTIMATION_PROBLEM_NORM_L_2:
		obj_value = 0.;
		for (int i = numVariablesX;
				i < numVariablesX + numVariablesEpsilon; ++i)
		{
			obj_value += x[i] * x[i];
		}
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
	ASSERT(n == numVariablesX + numVariablesEpsilon);
	switch(problemType_)
	{
	case ESTIMATION_PROBLEM_NORM_L_INF:
		for (int i = 0; i < numVariablesX; ++i)
		{
			grad_f[i] = 0.;
		}
		grad_f[numVariablesX] = 1.;
		break;
	case ESTIMATION_PROBLEM_NORM_L_1:
		for (int i = 0; i < numVariablesX; ++i)
		{
			grad_f[i] = 0.;
		}
		for (int i = numVariablesX;
				i < numVariablesX + numVariablesEpsilon; ++i)
		{
			grad_f[i] = 1.;
		}
		break;
	case ESTIMATION_PROBLEM_NORM_L_2:
		for (int i = 0; i < numVariablesX; ++i)
		{
			grad_f[i] = 0.;
		}
		for (int i = numVariablesX;
				i < numVariablesX + numVariablesEpsilon; ++i)
		{
			grad_f[i] = 2. * x[i];
		}
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
	ASSERT(n == numVariablesX + numVariablesEpsilon);
	ASSERT(m = numConsistencyConditions + numLocalityConditions);
	VectorXd h(numVariablesX);
	for (int i = 0; i < numVariablesX; ++i)
	{
		h(i) = x[i];
	}

	VectorXd Qh(m);
	Qh = data->supportMatrix() * h;

	for (int i = 0; i < numConsistencyConditions; ++i)
	{
		g[i] = Qh[i];
	}
	int numDirections = data->supportDirections().size();
	auto directions = data->supportDirections();
	for (int i = 0; i < numDirections; ++i)
	{
		Vector3d direction = directions[i];
		Vector3d point(x[3 * i], x[3 * i + 1], x[3 * i + 2]);
		double product = direction * point;
		double epsilon = 0.;
		switch(problemType_)
		{
		case ESTIMATION_PROBLEM_NORM_L_INF:
			epsilon = x[numVariablesX];
			break;
		case ESTIMATION_PROBLEM_NORM_L_1:
		case ESTIMATION_PROBLEM_NORM_L_2:
			epsilon = x[numVariablesX + i];
			break;
		}
		g[numConsistencyConditions + 2 * i] = product + epsilon;
		g[numConsistencyConditions + 2 * i + 1] = -product + epsilon;
	}

	DEBUG_END;
	return true;
}

bool IpoptSupportFunctionEstimator::eval_jac_g(Index n, const Number* x,
		bool new_x, Index m, Index n_ele_jac, Index* iRow, Index *jCol,
		Number* values)
{
	DEBUG_START;
	if (!values)
	{
		ASSERT(iRow);
		ASSERT(jCol);
	}
	if (new_x)
	{
		ASSERT(values);
	}
	ASSERT(n == numVariablesX + numVariablesEpsilon);
	ASSERT(m = numConsistencyConditions + numLocalityConditions);
	ASSERT(n_ele_jac == data->supportMatrix().nonZeros()
			+ 4 * numLocalityConditions);

	SparseMatrix Q = data->supportMatrix();
	int nonZeros = Q.nonZeros();

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
	if (jCol)
		ASSERT(jCol[0] < Q.cols());

	auto directions = data->supportDirections();
	int numDirections = directions.size();
	if (values)
	{
		for (int i = 0; i < numDirections; ++i)
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
	}
	else
	{
		for (int i = 0; i < numDirections; ++i)
		{
			int iEpsilon =
				problemType_ == ESTIMATION_PROBLEM_NORM_L_INF
				? numVariablesX : numVariablesX + i;
			iRow[nonZeros + 8 * i] =
				iRow[nonZeros + 8 * i + 1] =
				iRow[nonZeros + 8 * i + 2] =
				iRow[nonZeros + 8 * i + 3] =
				numConsistencyConditions + 2 * i;
			jCol[nonZeros + 8 * i] = 3 * i;
			jCol[nonZeros + 8 * i + 1] = 3 * i + 1;
			jCol[nonZeros + 8 * i + 2] = 3 * i + 2;
			jCol[nonZeros + 8 * i + 3] = iEpsilon; /* epsilon */

			iRow[nonZeros + 8 * i + 4] =
				iRow[nonZeros + 8 * i + 5] =
				iRow[nonZeros + 8 * i + 6] =
				iRow[nonZeros + 8 * i + 7] =
				numConsistencyConditions + 2 * i + 1;
			jCol[nonZeros + 8 * i + 4] = 3 * i;
			jCol[nonZeros + 8 * i + 5] = 3 * i + 1;
			jCol[nonZeros + 8 * i + 6] = 3 * i + 2;
			jCol[nonZeros + 8 * i + 7] = iEpsilon; /* epsilon */
		}
	}

	if (jCol)
		ASSERT(jCol[0] < Q.cols());

	/*
	 * Check that all values of sparsity structure have been correctly set.
	 */
	if (!values)
	{
		for (int i = 0; i < n_ele_jac; ++i)
		{
			DEBUG_PRINT("Checking element #%d: iRow = %d, "
					"jCol = %d, numVariablesX = %d, "
					"numVariablesEpsilon = %d", i, iRow[i],
					jCol[i], numVariablesX,
					numVariablesEpsilon);
			ASSERT(iRow[i] != -1);
			ASSERT(iRow[i] >= 0);
			ASSERT(iRow[i] < numConsistencyConditions
					+ numLocalityConditions);
			ASSERT(jCol[i] != -1);
			ASSERT(jCol[i] >= 0);
			ASSERT(jCol[i] < numVariablesX + numVariablesEpsilon);
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

	ASSERT(n == numVariablesX + numVariablesEpsilon);
	ASSERT(m = numConsistencyConditions + numLocalityConditions);

	if (problemType_ != ESTIMATION_PROBLEM_NORM_L_2)
	{
		/* Hessian is zero matrix in linear case. */
		DEBUG_END;
		return true;
	}
	ASSERT(n_ele_hess == numVariablesX / 3);

	if (new_x)
		ASSERT(values);

	if (!values)
	{
		/* Set sparsity structure of the Hessian. */
		ASSERT(iRow);
		ASSERT(jCol);
		for (int i = 0; i < numVariablesEpsilon; ++i)
		{
			iRow[i] = numVariablesX + i;
			jCol[i] = numVariablesX + i;
		}
	}
	else
	{
		/* Set values of the Hessian. */
		for (int i = 0; i < numVariablesEpsilon; ++i)
		{
			values[i] = 2. * obj_factor;
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
	ASSERT(n == numVariablesX + numVariablesEpsilon);
	VectorXd result(numVariablesX);

	switch (status)
	{
	case SUCCESS:
		MAIN_PRINT("SUCCESS");
		for (int i = 0; i < numVariablesX; ++i)
		{
			result(i) = x[i];
		}
		solution = result;
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

	//app->Options()->SetNumericValue("tol", 1e-3);
	//app->Options()->SetNumericValue("acceptable_tol", 1e-3);
	//app->Options()->SetIntegerValue("max_iter", 3000000);

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
