/*
 * Copyright (c) 2009-2016 Ilya Palachev <iliyapalachev@gmail.com>
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
 * @file GlpkSFELinearProgramBuilder.h
 * @brief Builder of GLPK L-inf and L-1 support function estimation problem
 * function estimation (implementation).
 */

#ifdef USE_GLPK

#include <iostream>
#include <fstream>

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "Recoverer/GlpkSFELinearProgramBuilder.h"
#include "PCLDumper.h"
#include "Recoverer/GlpProbWrappers.h"

GlpkSFELinearProgramBuilder::GlpkSFELinearProgramBuilder(SupportFunctionEstimationDataPtr dataOrig) : data(dataOrig)
{
	DEBUG_START;
	DEBUG_END;
}

GlpkSFELinearProgramBuilder::~GlpkSFELinearProgramBuilder()
{
	DEBUG_START;
	DEBUG_END;
}

static void declareRows(glp_prob *problem, SupportFunctionEstimationDataPtr data)
{
	DEBUG_START;
	SparseMatrix matrix = data->supportMatrix();
	int numDirections = data->numValues() / 3;
	int numConsistencyConstraints = matrix.rows();
	int numEpsConstraints = numDirections * 2;
	int numConstraints = numConsistencyConstraints + numEpsConstraints;

	/* Add rows. */
	glp_add_rows(problem, numConstraints);

	for (int i = 0; i < numConsistencyConstraints; ++i)
	{
		std::string name = "consistency" + std::to_string(i);
		glp_set_row_name(problem, i + 1, name.c_str());
		glp_set_row_bnds(problem, i + 1, GLP_LO, 0.0, 0.0);
	}

	VectorXd h = data->supportVector();
	for (int i = 0; i < numDirections; ++i)
	{
		std::string nameLower = "eps-lower-" + std::to_string(i);
		std::string nameUpper = "eps-upper-" + std::to_string(i);
		glp_set_row_name(problem, numConsistencyConstraints + 2 * i + 1, nameLower.c_str());
		glp_set_row_name(problem, numConsistencyConstraints + 2 * i + 2, nameUpper.c_str());
		/*  (u_i, x_i) + \varepsilon > h_i */
		glp_set_row_bnds(problem, numConsistencyConstraints + 2 * i + 1, GLP_LO, h(i), 0.0);
		/* -(u_i, x_i) + \varepsilon > -h_i */
		glp_set_row_bnds(problem, numConsistencyConstraints + 2 * i + 2, GLP_LO, -h(i), 0.0);
	}
	DEBUG_END;
}

static void declareTangientPoints(glp_prob *problem, int numValues)
{
	DEBUG_START;
	for (int i = 0; i < numValues; ++i)
	{
		std::string name = "x_" + std::to_string(i / 3) + "_" + std::to_string(i % 3);
		glp_set_col_name(problem, i + 1, name.c_str());
		glp_set_col_bnds(problem, i + 1, GLP_FR, 0.0, 0.0);
#if 0
		/* TODO: test algorithm with and without this feature. */
		glp_set_col_bnds(problem, i, GLP_LO, 0.0, 0.0);
#endif
		glp_set_obj_coef(problem, i + 1, 0.0);
	}
	DEBUG_END;
}

static void declareLinfColumnsAndObjective(glp_prob *problem, SupportFunctionEstimationDataPtr data)
{
	DEBUG_START;
	int iEpsilon = data->numValues() + 1;

	/* Add columns. */
	glp_add_cols(problem, data->numValues() + 1);
	declareTangientPoints(problem, data->numValues());

	/* Set the objective function = \varepsilon. */
	glp_set_col_name(problem, data->numValues() + 1, "epsilon");
	/* epsilnon should be positive */
	glp_set_col_bnds(problem, data->numValues() + 1, GLP_LO, 0.0, 0.0);
	glp_set_obj_coef(problem, iEpsilon, 1.0);
	DEBUG_END;
}

static void declareL1ColumnsAndObjective(glp_prob *problem, SupportFunctionEstimationDataPtr data)
{
	DEBUG_START;
	/* Add columns. */
	glp_add_cols(problem, data->numValues() + data->numValues() / 3);
	declareTangientPoints(problem, data->numValues());

	/*
	 * Set the objective function
	 * I = \varepsilon_{1} + \ldots + \varepsilon_{m}
	 */
	if (data->startingEpsilon() <= 0.)
	{
		ERROR_PRINT("starting epsilon = %lf", data->startingEpsilon());
		exit(EXIT_FAILURE);
	}
	for (int i = 0; i < data->numValues() / 3; ++i)
	{
		std::string name = "epsilon_" + std::to_string(i);
		glp_set_col_name(problem, data->numValues() + i + 1, name.c_str());
		/*
		 * \varepsilnon_{i} should be positive and not grater than
		 * starting speilon.
		 */
		glp_set_col_bnds(problem, data->numValues() + i + 1, GLP_DB, 0.0, data->startingEpsilon());
		glp_set_obj_coef(problem, data->numValues() + i + 1, 1.0);
	}
	DEBUG_END;
}

static void constructMatrixUpper(SparseMatrix matrix, int *iRows, int *iCols, double *values)
{
	DEBUG_START;
	/* Translate Eigen matrix to triple-form. */
	int i = 0;
	for (int k = 0; k < matrix.outerSize(); ++k)
	{
		for (SparseMatrix::InnerIterator it(matrix, k); it; ++it)
		{
			iRows[i + 1] = it.row() + 1;
			ASSERT(iRows[i + 1] > 0);
			iCols[i + 1] = it.col() + 1;
			ASSERT(iCols[i + 1] > 0);
			values[i + 1] = it.value();
			++i;
		}
	}
	DEBUG_END;
}

static void constructMatrixLowerRow(Vector3d direction, int iRow, int iDirection, int iEpsilon, int *iRows, int *iCols,
									double *values)
{
	DEBUG_START;
	iRows[0] = iRow;
	iRows[1] = iRow;
	iRows[2] = iRow;
	iRows[3] = iRow;
	iCols[0] = 3 * iDirection + 1;
	iCols[1] = 3 * iDirection + 2;
	iCols[2] = 3 * iDirection + 3;
	iCols[3] = iEpsilon;
	values[0] = direction.x;
	values[1] = direction.y;
	values[2] = direction.z;
	values[3] = 1.;
	DEBUG_END;
}

static void constructMatrixLower(std::vector<Vector3d> directions, int numConsistencyConstraints, bool ifLinfProblem,
								 int *iRows, int *iCols, double *values)
{
	DEBUG_START;
	int numDirections = directions.size();
	for (int i = 0; i < numDirections; ++i)
	{
		int iEpsilon = ifLinfProblem ? 3 * numDirections + 1 : 3 * numDirections + i + 1;
		/*  (u_i, x_i) + \varepsilon > h_i */
		constructMatrixLowerRow(directions[i], numConsistencyConstraints + 2 * i + 1, i, iEpsilon, iRows + 8 * i + 1,
								iCols + 8 * i + 1, values + 8 * i + 1);
		/* -(u_i, x_i) + \varepsilon > -h_i */
		constructMatrixLowerRow(-directions[i], numConsistencyConstraints + 2 * i + 2, i, iEpsilon, iRows + 8 * i + 5,
								iCols + 8 * i + 5, values + 8 * i + 5);
	}
	DEBUG_END;
}

static void constructMatrix(glp_prob *problem, SupportFunctionEstimationDataPtr data, bool ifLinfProblem)
{
	DEBUG_START;
	SparseMatrix matrix = data->supportMatrix();
	int numDirections = data->numValues() / 3;
	int numConsistencyConstraints = matrix.rows();

	/* Allocate memory for arrays of matrix triplets. */
	int numNonZerosTotal = matrix.nonZeros() + 2 * 4 * numDirections;
	int *iRows = new int[numNonZerosTotal + 1];
	int *iCols = new int[numNonZerosTotal + 1];
	double *values = new double[numNonZerosTotal + 1];
	DEBUG_PRINT("Allocated iRows, iCols, values arrays of length %d", numNonZerosTotal + 1);

	constructMatrixUpper(matrix, iRows, iCols, values);

	/* Fill the bottom part of the matrix. */
	auto directions = data->supportDirections();
	int numTopPart = matrix.nonZeros();
	constructMatrixLower(directions, numConsistencyConstraints, ifLinfProblem, iRows + numTopPart, iCols + numTopPart,
						 values + numTopPart);

	glp_load_matrix(problem, numNonZerosTotal, iRows, iCols, values);
	DEBUG_END;
}

glp_prob *GlpkSFELinearProgramBuilder::buildLinfProblem(void)
{
	DEBUG_START;
	/* Create the problem. */
	glp_prob *problem = glp_create_prob();
	glp_set_prob_name(problem, "Support function estimation from PCL in "
							   "L-inf norm.");

	/* Set the direction of optimization - to minimum. */
	glp_set_obj_dir(problem, GLP_MIN);

	declareRows(problem, data);

	declareLinfColumnsAndObjective(problem, data);

	constructMatrix(problem, data, true);

	/* Construct the GLPK problem. */
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG, "glpk-l-inf-problem.lp") << glp_prob_wrapper(problem);
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG, "glpk-l-inf-problem.mps") << glp_prob_wrapper_mps(problem);

	DEBUG_END;
	return problem;
}

glp_prob *GlpkSFELinearProgramBuilder::buildL1Problem(void)
{
	DEBUG_START;
	/* Create the problem. */
	glp_prob *problem = glp_create_prob();
	glp_set_prob_name(problem, "Support function estimation from PCL in "
							   "L-1 norm.");

	/* Set the direction of optimization - to minimum. */
	glp_set_obj_dir(problem, GLP_MIN);

	declareRows(problem, data);

	declareL1ColumnsAndObjective(problem, data);

	constructMatrix(problem, data, false);

	/* Construct the GLPK problem. */
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG, "glpk-l-1-problem.lp") << glp_prob_wrapper(problem);
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG, "glpk-l-1-problem.mps") << glp_prob_wrapper_mps(problem);

	DEBUG_END;
	return problem;
}

#endif /* USE_GLPK */
