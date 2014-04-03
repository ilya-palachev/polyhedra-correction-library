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

#include "DebugPrint.h"
#include "Recoverer/IpoptSupportFunctionEstimator.h"

/*
 * TODO: Implement dual problem here!
 */

IpoptSupportFunctionEstimator::IpoptSupportFunctionEstimator(
		SupportFunctionEstimationData *data) :
	SupportFunctionEstimationData(*data),
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
	DEBUG_END;
	return true;
}

bool IpoptSupportFunctionEstimator::get_bounds_info(Index n, Number* x_l,
		Number* x_u, Index m, Number* g_l, Number* g_u)
{
	DEBUG_START;
	DEBUG_END;
	return true;
}

bool IpoptSupportFunctionEstimator::get_starting_point(Index n, bool init_x,
		Number* x, bool init_z, Number* z_L, Number* z_U, Index m,
		bool init_lambda, Number* lambda)
{
	DEBUG_START;
	DEBUG_END;
	return true;
}

bool IpoptSupportFunctionEstimator::eval_f(Index n, const Number* x, bool new_x,
		Number& obj_value)
{
	DEBUG_START;
	DEBUG_END;
	return true;
}

bool IpoptSupportFunctionEstimator::eval_grad_f(Index n, const Number* x,
		bool new_x, Number* grad_f)
{
	DEBUG_START;
	DEBUG_END;
	return true;
}

bool IpoptSupportFunctionEstimator::eval_g(Index n, const Number* x, bool new_x,
		Index m, Number* g)
{
	DEBUG_START;
	DEBUG_END;
	return true;
}

bool IpoptSupportFunctionEstimator::eval_jac_g(Index n, const Number* x,
		bool new_x, Index m, Index n_ele_jac, Index* iRow, Index *jCol,
		Number* values)
{
	DEBUG_START;
	DEBUG_END;
	return true;
}

bool IpoptSupportFunctionEstimator::eval_h(Index n, const Number* x, bool new_x,
		Number obj_factor, Index m, const Number* lambda, bool new_lambda,
		Index nele_hess, Index* iRow, Index* jCol, Number* values)
{
	DEBUG_START;
	DEBUG_END;
	return true;
}

void IpoptSupportFunctionEstimator::finalize_solution(SolverReturn status,
		Index n, const Number* x, const Number* z_L, const Number* z_U, Index m,
		const Number* g, const Number* lambda, Number obj_value,
		const IpoptData* ip_data, IpoptCalculatedQuantities* ip_cq)
{
	DEBUG_START;
	DEBUG_END;
	return;
}
