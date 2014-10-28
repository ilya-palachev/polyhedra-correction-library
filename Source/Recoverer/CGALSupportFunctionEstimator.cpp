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
 * @file CGALSupportFunctionEstimator.h
 * @brief CGAL-based support function estimation (definition).
 */

#include <iostream>
#include <cassert>
#include <CGAL/basic.h>
#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>
/* choose exact integral type */
#ifdef CGAL_USE_GMP
#include <CGAL/Gmpz.h>
typedef CGAL::Gmpz ET;
#else
#include <CGAL/MP_Float.h>
typedef CGAL::MP_Float ET;
#endif
/* program and solution types */
typedef CGAL::Quadratic_program<double> Program;
typedef CGAL::Quadratic_program_solution<ET> Solution;

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "CGALSupportFunctionEstimator.h"

CGALSupportFunctionEstimator::CGALSupportFunctionEstimator(
		SupportFunctionEstimationDataPtr data,
		CGALEstimationMode modeOrig) :
		SupportFunctionEstimator(data),
		mode(modeOrig)
{
	DEBUG_START;
	DEBUG_END;
}

CGALSupportFunctionEstimator::~CGALSupportFunctionEstimator()
{
	DEBUG_START;
	DEBUG_END;
}

VectorXd CGALSupportFunctionEstimator::runLP(void)
{
	DEBUG_START;
	/*
	 * Create LP problem with constraints Ax >= b and no variables lower/upper
	 * bounds.
	 */
	Program lp (CGAL::LARGER, false, 0., false, 0.);

	int iEpsilon = data->numValues();
	int numVariables = data->numValues();
	int numConstraints = data->numConditions();

	lp.set_c(iEpsilon, 1.); /* Functional = \epsilon */

	/* Add standard SFE constraints. */
	SparseMatrix Q = data->supportMatrix();
	for (int k = 0; k < Q.outerSize(); ++k)
		for (SparseMatrix::InnerIterator it(Q, k); it; ++it)
		{
			lp.set_a(it.row(), it.col(), it.value());
		}

	/*
	 * Add additional constraints of that provide ||h - h^{0}|| <= \epsilon
	 *
	 * System
	 *
	 *  h_{i} - h_{i}^{0} >= -\epslion
	 *  h_{i} - h_{i}^{0} <=  \epslion
	 *
	 * is equal to the system
	 *
	 *  h_{i} + \epsilon >= h_{i}^{0}
	 * -h_{i} + \epsilon >= -h_{i}^{0}
	 */
	VectorXd h0 = data->supportVector();
	for (int i = 0; i < numVariables; ++i)
	{
		/*  h_{i} + \epsilon >= h_{i}^{0} */
		lp.set_a(2 * i + numConstraints, i, 1.);
		lp.set_a(2 * i + numConstraints, iEpsilon, 1.);
		lp.set_b(2 * i + numConstraints, h0(i));

		/* -h_{i} + \epsilon >= -h_{i}^{0} */
		lp.set_a(2 * i + 1 + numConstraints, i, -1.);
		lp.set_a(2 * i + 1 + numConstraints, iEpsilon, 1.);
		lp.set_b(2 * i + 1 + numConstraints, -h0(i));
	}

	/* solve the program, using ET as the exact type */
	Solution s = CGAL::solve_linear_program(lp, ET());
	assert (s.solves_linear_program(lp));

	/* output solution */
	std::cout << s;
	VectorXd estimation(data->numValues()); int i = 0;
	for (auto d = s.variable_numerators_begin(); d != s.variable_numerators_end(); ++d)
	{
		estimation(i++) = d->to_double();
	}

	DEBUG_END;
	return estimation;
}

VectorXd CGALSupportFunctionEstimator::runQP(void)
{
	DEBUG_START;
	/*
	 * Create QP problem with constraints Ax >= b and no variables lower/upper
	 * bounds.
	 */
	Program qp (CGAL::LARGER, false, 0., false, 0.);
	int numVariables = data->numValues();

	/* Set matrix D to identity matrix. */
	for (int i = 0; i < numVariables; ++i)
	{
		qp.set_d(i, i, 2.); /* NOTE: We set matrix 2 * D here! */
	}

	/* Set std::vector c to -2 * h_0. */
	double h0norm = 0.;
	VectorXd h0 = data->supportVector();
	for (int i = 0; i < numVariables; ++i)
	{
		qp.set_c(i, -2. * h0(i));
		h0norm += h0(i) * h0(i);
	}

	/* Set scalar c0 to <h_0, h_0>. */
	qp.set_c0(h0norm);

	/* Set A matrix to Q. */
	SparseMatrix Q = data->supportMatrix();
	for (int k = 0; k < Q.outerSize(); ++k)
		for (SparseMatrix::InnerIterator it(Q, k); it; ++it)
		{
			qp.set_a(it.row(), it.col(), it.value());
		}

	/* solve the program, using ET as the exact type */
	Solution s = CGAL::solve_quadratic_program(qp, ET());
	ASSERT(s.solves_quadratic_program(qp));

	/* output solution */
	std::cout << s;
	VectorXd estimation(data->numValues()); int i = 0;
	for (auto d = s.variable_numerators_begin(); d != s.variable_numerators_end(); ++d)
	{
		estimation(i++) = d->to_double();
	}

	DEBUG_END;
	return estimation;
}

VectorXd CGALSupportFunctionEstimator::run(void)
{
	DEBUG_START;
	switch(mode)
	{
	case CGAL_ESTIMATION_LINEAR:
		return runLP();
		break;
	case CGAL_ESTIMATION_QUADRATIC:
		return runQP();
		break;
	default:
		__builtin_unreachable();
		break;
	}

	__builtin_unreachable();
	DEBUG_END;
	return VectorXd(1);
}
