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
 * @file ClpSupportFunctionEstimator.cpp
 * @brief Implementation of Clp-based linear programming solver used for
 * support function estimation.
 */

#ifdef USE_CLP

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "Recoverer/ClpSupportFunctionEstimator.h"
#include "Recoverer/GlpkSFELinearProgramBuilder.h"

ClpSupportFunctionEstimator::ClpSupportFunctionEstimator(
		SupportFunctionEstimationDataPtr data) :
	SupportFunctionEstimator(data),
	ifCommandlineMode(false),
	problemType_(DEFAULT_ESTIMATION_PROBLEM_NORM)
{
	DEBUG_START;
	DEBUG_END;
}

ClpSupportFunctionEstimator::~ClpSupportFunctionEstimator()
{
	DEBUG_START;
	DEBUG_END;
}

void ClpSupportFunctionEstimator::enableCommandlineMode()
{
	DEBUG_START;
	ifCommandlineMode = true;
	DEBUG_END;
}

void ClpSupportFunctionEstimator::setProblemType(EstimationProblemNorm type)
{
	DEBUG_START;
	problemType_ = type;
	DEBUG_END;
}

VectorXd ClpSupportFunctionEstimator::run(void)
{
	DEBUG_START;
	VectorXd solution(data->numValues());

	/* Construct the CLP problem. */
	GlpkSFELinearProgramBuilder builder(data);
	glp_prob *problem = NULL;

	switch (problemType_)
	{
	case ESTIMATION_PROBLEM_NORM_L_INF:
		problem = builder.buildLinfProblem();
		break;
	case ESTIMATION_PROBLEM_NORM_L_1:
		problem = builder.buildL1Problem();
		break;
	case ESTIMATION_PROBLEM_NORM_L_2:
		ERROR_PRINT("Not implemented yet!");
		exit(EXIT_FAILURE);
		break;
	}

	char *mps_file_name = strdup("/tmp/glpk-linf-problem.mps");
	if (glp_write_mps(problem, GLP_MPS_FILE, NULL, mps_file_name))
	{
		ERROR_PRINT("Failed to write MPS problem!");
		DEBUG_END;
		return solution;
	}

	if (ifCommandlineMode)
	{
		char *command = (char*) malloc(1024 * sizeof(char));
		char *solution_file_name = strdup("/tmp/solution.txt");
		char *temporary_solution_file_name =
			strdup("/tmp/temporary-solution.txt");
		unlink(solution_file_name);
		unlink(temporary_solution_file_name);
		sprintf(command,
			"%s/Scripts/run_clp_solver.sh %s %s %s",
			SOURCE_DIR, mps_file_name, temporary_solution_file_name,
			solution_file_name);
		int result = system(command);
		std::cerr << "CLP solver wrapper script returned " << result
			<< std::endl;
	
		FILE *file = fopen(solution_file_name, "r");
	
		for (int i = 0; i < data->numValues(); ++i)
		{
			double current = 0.;
			if (fscanf(file, "%lf", &current) == 1)
			{
				solution(i) = current;
			}
			DEBUG_PRINT("solution(%d) = %lf", i, solution(i));
		}
		free(solution_file_name);
		free(temporary_solution_file_name);
		free(command);
	}
	else
	{
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
	}

	/* Cleanup. */
	glp_delete_prob(problem);
	glp_free_env();

	free(mps_file_name);
	DEBUG_END;
	return solution;
}

#endif /* USE_CLP */
