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
		SupportFunctionEstimationData *data) :
		SupportFunctionEstimator(data),
	problemType(PROBLEM_PRIMAL)
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

	n = numValues();
	DEBUG_PRINT("Number of variables is set to n = %d", n);

	m = numConditions();
	DEBUG_PRINT("Number of constraints is set to m = %d", m);

	nnz_jac_g = supportMatrix().nonZeros();
	DEBUG_PRINT("Number of non-zero elements in the Jacobian of g is set to "
			"nnz_jac_g = %d", nnz_jac_g);

	nnz_h_lag = numValues();
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

	for (int i = 0; i < n; ++i)
	{
		x_l[i] = -TNLP_INFINITY;
		x_u[i] = +TNLP_INFINITY;
	}
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

	if (init_x)
	{
		for (int i = 0; i < n; ++i)
		{
			x[i] = 1.;
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

	obj_value = 0.;
	VectorXd x0 = supportVector();
	for (int i = 0; i < n; ++i)
	{
		obj_value += (x[i] - x0(i)) * (x[i] - x0(i));
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

	VectorXd x0 = supportVector();
	for (int i = 0; i < n; ++i)
	{
		grad_f[i] = 2. * (x[i] - x0(i));
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

	VectorXd h(n);
	for (int i = 0; i < n; ++i)
	{
		h(i) = x[i];
	}

	VectorXd Qh(m);
	Qh = supportMatrix() * h;

	for (int i = 0; i < m; ++i)
	{
		g[i] = Qh[i];
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

	if (!values)
	{
		ASSERT(iRow);
		ASSERT(jCol);
	}
	if (new_x)
		ASSERT(values);

	SparseMatrix Q = supportMatrix();
	ASSERT(n_ele_jac == Q.nonZeros());

	/* Assign all values of sparsity structure to -1 */
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
				/* Inform about the sparsity structure of the Lagrangian of g */
				iRow[i] = it.row();
				jCol[i] = it.col();
			}
			++i;
		}

	/* Check that all values of sparsity structure have been correctly set. */
	if (!values)
	{
		for (int i = 0; i < n_ele_jac; ++i)
		{
			ASSERT(iRow[i] != -1);
			ASSERT(iRow[i] >= 0);
			ASSERT(iRow[i] < Q.rows());
			ASSERT(jCol[i] != -1);
			ASSERT(jCol[i] >= 0);
			ASSERT(jCol[i] < Q.cols());
		}
	}
	DEBUG_END;
	return true;
}

bool IpoptSupportFunctionEstimator::eval_h(Index n, const Number* x, bool new_x,
		Number obj_factor, Index m, const Number* lambda, bool new_lambda,
		Index n_ele_hess, Index* iRow, Index* jCol, Number* values)
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

	ASSERT(n_ele_hess == numValues());

	if (new_x)
		ASSERT(values);

	if (!values)
	{
		/* Set sparsity structure of the Hessian. */
		ASSERT(iRow);
		ASSERT(jCol);
		for (int i = 0; i < n_ele_hess; ++i)
		{
			iRow[i] = jCol[i] = i;
		}
	}
	else
	{
		/* Set values of the Hessian. */
		for (int i = 0; i < n_ele_hess; ++i)
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

	switch (status)
	{
	case SUCCESS:
		MAIN_PRINT("SUCCESS");
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

void IpoptSupportFunctionEstimator::run(void)
{
	DEBUG_START;
	SmartPtr<IpoptApplication> app = IpoptApplicationFactory();

	/* Intialize the IpoptApplication and process the options */
	ApplicationReturnStatus status;
	status = app->Initialize();
	if (status != Solve_Succeeded)
	{
		MAIN_PRINT("*** Error during initialization!");
		return;
	}

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
}


#endif /* USE_IPOPT*/
