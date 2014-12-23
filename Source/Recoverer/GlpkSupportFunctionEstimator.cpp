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
#include "DebugAssert.h"
#include "Recoverer/GlpkSupportFunctionEstimator.h"

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

static void declareRows(glp_prob *problem, SupportFunctionEstimationDataPtr
		data)
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
		glp_set_row_name(problem, numConsistencyConstraints + 2 * i + 1,
				nameLower.c_str());
		glp_set_row_name(problem, numConsistencyConstraints + 2 * i + 2,
				nameUpper.c_str());
		/*  (u_i, x_i) + \varepsilon > h_i */
		glp_set_row_bnds(problem, numConsistencyConstraints + 2 * i + 1,
				GLP_LO, h(i), 0.0);
		/* -(u_i, x_i) + \varepsilon > -h_i */
		glp_set_row_bnds(problem, numConsistencyConstraints + 2 * i + 2,
				GLP_LO, -h(i), 0.0);
	}
	DEBUG_END;
}

static void declareColumnsAndObjective(glp_prob *problem,
		SupportFunctionEstimationDataPtr data)
{
	DEBUG_START;
	int iEpsilon = data->numValues() + 1;

	/* Add columns. */
	glp_add_cols(problem, data->numValues() + 1);

	/* Set the objective function = \varepsilon. */
	for (int i = 0; i < data->numValues(); ++i)
	{
		std::string name = "x_" + std::to_string(i / 3) + "_"
			+ std::to_string(i % 3);
		glp_set_col_name(problem, i + 1, name.c_str());
#if 0
		/* TODO: test algorithm with and without this feature. */
		glp_set_col_bnds(problem, i, GLP_LO, 0.0, 0.0);
#endif
		glp_set_obj_coef(problem, i + 1, 0.0);
	}
	glp_set_obj_coef(problem, iEpsilon, 1.0);
	DEBUG_END;
}

static void constructMatrixUpper(SparseMatrix matrix, int *iRows, int *iCols,
		double *values)
{
	DEBUG_START;
	/* Translate Eigen matrix to triple-form. */
	int i = 0;
	for (int k = 0; k < matrix.outerSize(); ++k)
	{
		for (SparseMatrix::InnerIterator it(matrix, k); it;
				++it)
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

static void constructMatrixLower(std::vector<Vector3d> directions,
		int numConsistencyConstraints, int iEpsilon,
		int *iRows, int *iCols, double *values)
{
	DEBUG_START;
	int numDirections = directions.size();
	for (int i = 0; i < numDirections; ++i)
	{
		/*  (u_i, x_i) + \varepsilon > h_i */
		iRows[8 * i + 1] = numConsistencyConstraints
			+ 2 * i + 1;
		iCols[8 * i + 1] = 3 * i + 1;
		values[8 * i + 1] = directions[i].x;

		iRows[8 * i + 2] = numConsistencyConstraints
			+ 2 * i + 1;
		iCols[8 * i + 2] = 3 * i + 2;
		values[8 * i + 2] = directions[i].y;

		iRows[8 * i + 3] = numConsistencyConstraints
			+ 2 * i + 1;
		iCols[8 * i + 3] = 3 * i + 3;
		values[8 * i + 3] = directions[i].z;

		iRows[8 * i + 4] = numConsistencyConstraints
			+ 2 * i + 1;
		iCols[8 * i + 4] = iEpsilon;
		values[8 * i + 4] = 1.;

		/* -(u_i, x_i) + \varepsilon > -h_i */
		iRows[8 * i + 5] = numConsistencyConstraints
			+ 2 * i + 2;
		iCols[8 * i + 5] = 3 * i + 1;
		values[8 * i + 5] = -directions[i].x;

		iRows[8 * i + 6] = numConsistencyConstraints
			+ 2 * i + 2;
		iCols[8 * i + 6] = 3 * i + 2;
		values[8 * i + 6] = -directions[i].y;

		iRows[8 * i + 7] = numConsistencyConstraints
			+ 2 * i + 2;
		iCols[8 * i + 7] = 3 * i + 3;
		values[8 * i + 7] = -directions[i].z;

		iRows[8 * i + 8] = numConsistencyConstraints
			+ 2 * i + 2;
		iCols[8 * i + 8] = iEpsilon;
		values[8 * i + 8] = 1.;
	}
	DEBUG_END;
}

static void constructMatrix(glp_prob *problem,
		SupportFunctionEstimationDataPtr data)
{
	DEBUG_START;
	SparseMatrix matrix = data->supportMatrix();
	int numDirections = data->numValues() / 3;
	int numConsistencyConstraints = matrix.rows();
	int iEpsilon = data->numValues() + 1;

	/* Allocate memory for arrays of matrix triplets. */
	int numNonZerosTotal = matrix.nonZeros() + 2 * 4 * numDirections;
	int *iRows = new int[numNonZerosTotal + 1];
	int *iCols = new int[numNonZerosTotal + 1];
	double *values = new double[numNonZerosTotal + 1];
	DEBUG_PRINT("Allocated iRows, iCols, values arrays of length %d",
			numNonZerosTotal + 1);

	constructMatrixUpper(matrix, iRows, iCols, values);

	/* Fill the bottom part of the matrix. */
	auto directions = data->supportDirections();
	int numTopPart = matrix.nonZeros();
	constructMatrixLower(directions, numConsistencyConstraints, iEpsilon,
			iRows + numTopPart, iCols + numTopPart,
			values + numTopPart);

	glp_load_matrix(problem, numNonZerosTotal, iRows, iCols, values);
	DEBUG_END;
}

static glp_prob *constructProblem(SupportFunctionEstimationDataPtr data)
{
	DEBUG_START;
	/* Create the problem. */
	glp_prob *problem = glp_create_prob();
	glp_set_prob_name(problem, "Support function estimation from PCL.");

	/* Set the direction of optimization - to minimum. */
	glp_set_obj_dir(problem, GLP_MIN);

	declareRows(problem, data);

	declareColumnsAndObjective(problem, data);

	constructMatrix(problem, data);

	DEBUG_END;
	return problem;
}

VectorXd GlpkSupportFunctionEstimator::run(void)
{
	DEBUG_START;

	/* Construct the GLPK problem. */
	glp_prob *problem = constructProblem(data);

	/* Run the simplex solver. */
	glp_simplex(problem, NULL);

	/* Get the solution vector. */
	VectorXd solution(data->numValues());
	for (int i = 0; i < data->numValues(); ++i)
		solution(i) = glp_get_col_prim(problem, i + 1);

	/* Cleanup. */
	glp_delete_prob(problem);
	glp_free_env();

	DEBUG_END;
	return solution;
}

#endif /* USE_GLPK */
