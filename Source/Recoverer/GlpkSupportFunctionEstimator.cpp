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
 * @file GlpkSupportFunctionEstimator.cpp
 * @brief Implementation of Glpk-based linear programming solver used for
 * support function estimation.
 */

#ifdef USE_GLPK

#include "DebugPrint.h"
#include "Recoverer/GlpkSupportFunctionEstimator.h"
#include "Recoverer/GlpkSFELinfBuilder.h"

GlpkSupportFunctionEstimator::GlpkSupportFunctionEstimator(
		SupportFunctionEstimationDataPtr data) :
	SupportFunctionEstimator(data)
{
	DEBUG_START;
	DEBUG_END;
}

GlpkSupportFunctionEstimator::~GlpkSupportFunctionEstimator()
{
	DEBUG_START;
	DEBUG_END;
}

VectorXd GlpkSupportFunctionEstimator::run(void)
{
	DEBUG_START;

	/* Construct the GLPK problem. */
	GlpkSFELinfBuilder builder(data);
	glp_prob *problem = builder.build();

	/* Run the simplex solver. */
	glp_simplex(problem, NULL);

	/* Get the solution vector. */
	VectorXd solution(data->numValues());
	for (int i = 0; i < data->numValues(); ++i)
	{
		solution(i) = glp_get_col_prim(problem, i + 1);
		DEBUG_PRINT("solution(%d) = %lf", i, solution(i));
	}

	/* Cleanup. */
	glp_delete_prob(problem);
	glp_free_env();

	DEBUG_END;
	return solution;
}

#endif /* USE_GLPK */
