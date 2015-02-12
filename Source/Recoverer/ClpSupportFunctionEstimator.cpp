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
 * @file ClpSupportFunctionEstimator.cpp
 * @brief Implementation of Clp-based linear programming solver used for
 * support function estimation.
 */

#ifdef USE_CLP

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "Recoverer/ClpSupportFunctionEstimator.h"
#include "Recoverer/GlpkSFELinfBuilder.h"

ClpSupportFunctionEstimator::ClpSupportFunctionEstimator(
		SupportFunctionEstimationDataPtr data) :
	SupportFunctionEstimator(data)
{
	DEBUG_START;
	DEBUG_END;
}

ClpSupportFunctionEstimator::~ClpSupportFunctionEstimator()
{
	DEBUG_START;
	DEBUG_END;
}

VectorXd ClpSupportFunctionEstimator::run(void)
{
	DEBUG_START;
	VectorXd solution(data->numValues());

	/* Construct the CLP problem. */
	GlpkSFELinfBuilder builder(data);
	glp_prob *problem = builder.build();

	char *mps_file_name = strdup("/tmp/glpk-linf-problem.mps");
	if (glp_write_mps(problem, GLP_MPS_FILE, NULL, mps_file_name))
	{
		ERROR_PRINT("Failed to write MPS problem!");
		DEBUG_END;
		return solution;
	}

	ClpSimplex model;
	if (model.readMps(mps_file_name))
	{
		ERROR_PRINT("Failed to read MPS problem!");
		DEBUG_END;
		return solution;
	}

	/* Run the solver */
	model.primal();

	/* Get the solution vector. */
	int numColumns = model.numberColumns();
	ASSERT(numColumns - 1 == data->numValues());
	double *columnPrimal = model.primalColumnSolution();
	for (int i = 0; i < numColumns - 1; ++i)
	{
		solution(i) = columnPrimal[i]
		DEBUG_PRINT("solution(%d) = %lf", i, solution(i));
	}

	/* Cleanup. */
	glp_delete_prob(problem);
	glp_free_env();

	free(mps_file_name);
	DEBUG_END;
	return solution;
}

#endif /* USE_CLP */
