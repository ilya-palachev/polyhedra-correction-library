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
 * @file CPLEXSupportFunctionEstimator.cpp
 * @brief Implementation of CPLEX-based linear programming solver used for
 * support function estimation.
 */

#ifdef USE_CPLEX

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "Recoverer/CPLEXSupportFunctionEstimator.h"
#include "Recoverer/GlpkSFELinfBuilder.h"

CPLEXSupportFunctionEstimator::CPLEXSupportFunctionEstimator(
		SupportFunctionEstimationDataPtr data) :
	SupportFunctionEstimator(data)
{
	DEBUG_START;
	DEBUG_END;
}

CPLEXSupportFunctionEstimator::~CPLEXSupportFunctionEstimator()
{
	DEBUG_START;
	DEBUG_END;
}

VectorXd CPLEXSupportFunctionEstimator::run(void)
{
	DEBUG_START;
	VectorXd solution(data->numValues());

	/* Construct the CPLEX problem. */
	GlpkSFELinfBuilder builder(data);
	glp_prob *problem = builder.build();

	char *mps_file_name = strdup("/tmp/glpk-linf-problem.mps");
	if (glp_write_mps(problem, GLP_MPS_FILE, NULL, mps_file_name))
	{
		ERROR_PRINT("Failed to write MPS problem!");
		DEBUG_END;
		return solution;
	}

	/* TODO: it's too hard-code solution! */
	char *command = (char*) malloc(1024 * sizeof(char));
	char *solution_file_name = strdup("/tmp/solution.txt");
	sprintf(command,
		"%s/Scripts/run_cplex_solver.sh %s %s %s",
		SOURCE_DIR, mps_file_name, "/tmp/solution.xml", solution_file_name);
	int result = system(command);
	std::cerr << "CPLEX solver wrapper script returned " << result << std::endl;

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

	/* Cleanup. */
	glp_delete_prob(problem);
	glp_free_env();

	free(mps_file_name);
	free(command);
	free(solution_file_name);
	DEBUG_END;
	return solution;
}

#endif /* USE_CPLEX */
