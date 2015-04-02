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
 * @file CPLEXSupportFunctionEstimator.cpp
 * @brief Implementation of CPLEX-based linear programming solver used for
 * support function estimation.
 */

#ifdef USE_CPLEX

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "Recoverer/CPLEXSupportFunctionEstimator.h"
#include "Recoverer/GlpkSFELinearProgramBuilder.h"

CPLEXSupportFunctionEstimator::CPLEXSupportFunctionEstimator(
		SupportFunctionEstimationDataPtr data) :
	SupportFunctionEstimator(data),
	problemType_(DEFAULT_ESTIMATION_PROBLEM_NORM)
{
	DEBUG_START;
	DEBUG_END;
}

CPLEXSupportFunctionEstimator::~CPLEXSupportFunctionEstimator()
{
	DEBUG_START;
	DEBUG_END;
}

void CPLEXSupportFunctionEstimator::setProblemType(EstimationProblemNorm type)
{
	DEBUG_START;
	problemType_ = type;
	DEBUG_END;
}

double printSignedValue(double value, FILE *file)
{
	if (value >= 0)
		fprintf(file, "+ ");
	fprintf(file, "%lf ", value);
}

static void printConsistencyConstraints(SparseMatrix matrix, FILE *file)
{
	DEBUG_START;
	int iConstraint = 0;
	for (int k = 0; k < matrix.outerSize(); ++k)
	{
		SparseMatrix::InnerIterator it(matrix, k);
		fprintf(file, " c%d: ", iConstraint);
		int iRow = it.row();
		for (int iProduct = 0; iProduct < 2; ++iProduct)
		{
			int iCol = it.col();
			ASSERT(iCol % 3 == 0);
			int iVariable = iCol / 3;
			for (int iDimension = 0; iDimension < 3;
					++iDimension)
			{
				ASSERT(it.row() == iRow);
				ASSERT(it.col() == iCol + iDimension);
				printSignedValue(it.value(), file);
				fprintf(file, "x_%d_%d ", iVariable,
						iDimension);
				++it;
				if (!it)
				{
					ASSERT(iDimension == 2);
					break;
				}
			}
		}
		fprintf(file, "\n");
		++iConstraint;
	}
	DEBUG_END;
}

void printLocalityConstraint(FILE *file, const char *name, Vector3d direction,
		int iVariable, double bound, bool ifLinfProblem)
{
	fprintf(file, " %s%i: ", name, iVariable);
	printSignedValue(direction.x, file);
	fprintf(file, " x_%d_1", iVariable);
	printSignedValue(direction.y, file);
	fprintf(file, " x_%d_2", iVariable);
	printSignedValue(direction.z, file);
	fprintf(file, " x_%d_3", iVariable);
	fprintf(file, " + v");
	if (!ifLinfProblem)
		fprintf(file, "_%d", iVariable);
	fprintf(file, " >= %lf\n", bound);
}

void printLinfLocalityConstraints(std::vector<Vector3d> directions,
		VectorXd h, FILE *file)
{
	DEBUG_START;
	int numDirections = directions.size();
	for (int i = 0; i < numDirections; ++i)
	{
		Vector3d direction = directions[i];
		printLocalityConstraint(file, "l", direction, i, h(i), true);
		printLocalityConstraint(file, "u", -direction, i, -h(i), true);
	}
	DEBUG_END;
}

void printL1L2LocalityConstraints(std::vector<Vector3d> directions,
		VectorXd h, FILE *file)
{
	DEBUG_START;
	int numDirections = directions.size();
	for (int i = 0; i < numDirections; ++i)
	{
		Vector3d direction = directions[i];
		printLocalityConstraint(file, "l", direction, i, h(i), false);
		printLocalityConstraint(file, "u", -direction, i, -h(i), false);
	}
	DEBUG_END;
}

static void printLinfProblem(SupportFunctionEstimationDataPtr data,
		char *file_name)
{
	DEBUG_START;
	FILE *file = (FILE*) fopen(file_name, "w");
	fprintf(file, "\\Problem name: cplex-problem.lp\n\n");
	fprintf(file, "Minimize\n");
	fprintf(file, " obj: v\n");
	fprintf(file, "Subject To\n");
	printConsistencyConstraints(data->supportMatrix(), file);
	printLinfLocalityConstraints(data->supportDirections(),
			data->supportVector(); file);
	fprintf(file, "Bounds\n");
	fprintf(file, " 0 <= v <= %lf\n", data->startingEpsilon());
	fprintf(file, "End");
	fclose(file);
	DEBUG_END;
}

static void printL1Problem(SupportFunctionEstimationDataPtr data,
		char *file_name)
{
	DEBUG_START;
	FILE *file = (FILE*) fopen(file_name, "w");
	fprintf(file, "\\Problem name: cplex-problem.lp\n\n");
	fprintf(file, "Minimize\n");
	fprintf(file, " obj: ");
	int numDirections = data->numValues() / 3;
	for (int i = 0; i < numDirections; ++i)
	{
		fprintf(file, "+ v_%d ", i);
	}
	fprintf(file, "\n");
	fprintf(file, "Subject To\n");
	printConsistencyConstraints(data->supportMatrix(), file);
	printL1L2LocalityConstraints(data->supportDirections(),
			data->supportVector(); file);
	fprintf(file, "Bounds\n");
	for (int i = 0; i < numDirections; ++i)
	{
		fprintf(file, "0 <= v_%d <= %lf\n", i,
				data->startingEpsilon());
	}
	fprintf(file, "End");
	fclose(file);
	DEBUG_END;
}

static void printL2Problem(SupportFunctionEstimationDataPtr data,
		char *file_name)
{
	DEBUG_START;
	FILE *file = (FILE*) fopen(file_name, "w");
	fprintf(file, "\\Problem name: cplex-problem.lp\n\n");
	fprintf(file, "Minimize\n");
	fprintf(file, " obj: ");
	int numDirections = data->numValues() / 3;
	for (int i = 0; i < numDirections; ++i)
	{
		fprintf(file, "+ v_%d ^2", i);
	}
	fprintf(file, "\n");
	fprintf(file, "Subject To\n");
	printConsistencyConstraints(data->supportMatrix(), file);
	printL1L2LocalityConstraints(data->supportDirections(),
			data->supportVector(); file);
	fprintf(file, "Bounds\n");
	for (int i = 0; i < numDirections; ++i)
	{
		fprintf(file, "0 <= v_%d <= %lf\n", i,
				data->startingEpsilon());
	}
	fprintf(file, "End");
	fclose(file);
	DEBUG_END;
}

VectorXd CPLEXSupportFunctionEstimator::run(void)
{
	DEBUG_START;
	VectorXd solution(data->numValues());

	/* Construct the CPLEX problem. */
	char *problem_file_name = strdup("/tmp/cplex-problem.lp");
	unlink(problem_file_name);

	switch (problemType_)
	{
	case ESTIMATION_PROBLEM_NORM_L_INF:
		printLinfProblem(data, problem_file_name);
		break;
	case ESTIMATION_PROBLEM_NORM_L_1:
		printL1Problem(data, problem_file_name);
		break;
	case ESTIMATION_PROBLEM_NORM_L_2:
		printL2Problem(data, problem_file_name);
		break;
	}

	/* TODO: it's too hard-code solution! */
	char *command = (char*) malloc(1024 * sizeof(char));
	char *solution_file_name = strdup("/tmp/solution.txt");
	unlink(solution_file_name);
	sprintf(command,
		"%s/Scripts/run_cplex_solver.sh %s %s %s",
		SOURCE_DIR, problem_file_name, "/tmp/solution.xml",
		solution_file_name);
	int result = system(command);
	std::cerr << "CPLEX solver wrapper script returned " << result
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

	/* Cleanup. */
	free(problem_file_name);
	free(command);
	free(solution_file_name);
	DEBUG_END;
	return solution;
}

#endif /* USE_CPLEX */
