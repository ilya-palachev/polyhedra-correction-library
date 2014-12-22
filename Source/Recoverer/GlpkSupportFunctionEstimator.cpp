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

VectorXd GlpkSupportFunctionEstimator::run(void)
{
	DEBUG_START;

	/* Create the problem. */
	glp_prob *problem = glp_create_prob();
	glp_set_prob_name(problem, "Support function estimation from PCL.");

	/* Set the direction of optimization - to minimum. */
	glp_set_obj_dir(problem, GLP_MIN);

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

	auto directions = data->supportDirections();
	VectorXd h = data->supportVector();
	for (int i = 0; i < numDirections; ++i)
	{
		std::string nameLower = "eps-lower-" + std::to_string(i);
		std::string nameUpper = "eps-upper-" + std::to_string(i);
		glp_set_row_name(problem, numConsistencyConstraints + 2 * i + 1,
				nameLower.c_str());
		glp_set_row_name(problem, numConsistencyConstraints + 2 * i + 2,
				nameUpper.c_str());
		glp_set_row_bnds(problem, numConsistencyConstraints + 2 * i + 1,
				GLP_LO, h(i), 0.0);
		glp_set_row_bnds(problem, numConsistencyConstraints + 2 * i + 2,
				GLP_LO, -h(i), 0.0);
	}

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
	glp_set_obj_coef(problem, data->numValues(), 1.0);

	/* Translate Eigen matrix to triple-form. */
	int *iRows = new int[matrix.nonZeros()];
	int *iCols = new int[matrix.nonZeros()];
	double *values = new double[matrix.nonZeros()];
	int i = 0;
	for (int k = 0; k < matrix.outerSize(); ++k)
	{
		for (SparseMatrix::InnerIterator it(matrix, k); it;
				++it)
		{
			iRows[i] = it.row() + 1;
			ASSERT(iRows[i] > 0);
			iCols[i] = it.col() + 1;
			ASSERT(iCols[i] > 0);
			values[i] = it.value();
			++i;
		}
	}
	glp_load_matrix(problem, matrix.nonZeros(), iRows, iCols, values);

	/* Run the simplex solver. */
	glp_simplex(problem, NULL);

	/* Get the solution vector. */
	VectorXd solution(data->numValues());
	for (int i = 0; i < data->numValues(); ++i)
		solution(i) = glp_get_col_prim(problem, i + 1);

	glp_delete_prob(problem);
	glp_free_env();

	DEBUG_END;
	return solution;
}

#endif /* USE_GLPK */
