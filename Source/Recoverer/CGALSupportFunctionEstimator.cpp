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
typedef CGAL::Quadratic_program<int> Program;
typedef CGAL::Quadratic_program_solution<ET> Solution;

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "CGALSupportFunctionEstimator.h"

CGALSupportFunctionEstimator::CGALSupportFunctionEstimator(
		SupportFunctionEstimationData *data) :
		SupportFunctionEstimationData(data)
{
	DEBUG_START;
	DEBUG_END;
}

CGALSupportFunctionEstimator::~CGALSupportFunctionEstimator()
{
	DEBUG_START;
	DEBUG_END;
}

void CGALSupportFunctionEstimator::run(void)
{
	DEBUG_START;
	/* by default, we have a nonnegative QP with Ax <= b */
	Program qp (CGAL::SMALLER, true, 0, false, 0);

	/* Set matrix D to identity matrix. */
	for (int i = 0; i < numValues(); ++i)
	{
		qp.set_d(i, i, 1.);
	}

	/* Set vector c to -2 * h_0. */
	double h0norm = 0.;
	VectorXd h0 = supportVector();
	for (int i = 0; i < numValues(); ++i)
	{
		qp.set_c(i, -2. * h0(i));
		h0norm += h0(i) * h0(i);
	}

	/* Set scalar c0 to <h_0, h_0>. */
	qp.set_c0(h0norm);

	/* Set A matrix to -Q. */
	SparseMatrix Q = supportMatrix();
	for (int k = 0; k < Q.outerSize(); ++k)
		for (SparseMatrix::InnerIterator it(Q, k); it; ++it)
		{
			qp.set_a(it.row(), it.col(), -it.value());
		}

	/* solve the program, using ET as the exact type */
	Solution s = CGAL::solve_quadratic_program(qp, ET());
	ASSERT(s.solves_quadratic_program(qp));

	/* output solution */
	std::cout << s;

	DEBUG_END;
}
