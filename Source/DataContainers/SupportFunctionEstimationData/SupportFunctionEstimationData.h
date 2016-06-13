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
 * @file SupportFunctionEstimationData.h
 * @brief Structure that represents the input data for support
 * function estimation engines
 * - declaration.
 */

#ifndef SUPPORTFUNCTIONESTIMATIONDATA_H_
#define SUPPORTFUNCTIONESTIMATIONDATA_H_

#include <memory>

#include "SparseMatrixEigen.h"
#include "Vector3d.h"
#include "SupportMatrix.h"
#include "DataContainers/SupportFunctionData/SupportFunctionData.h"

/** Each condition has exactly not more than  4 non-zero coefficients. */
#define NUM_NONZERO_COEFFICIENTS_IN_CONDITION 4

/** Type of support matrix. */
typedef enum
{
	/** Karl-Kulkarni-Verghese-Willsky conditions. */
	SUPPORT_MATRIX_TYPE_KKVW,

	/** Optimized Karl-Kulkarni-Verghese-Willsky conditions. */
	SUPPORT_MATRIX_TYPE_KKVW_OPT,

	/** Gardner-Kiderlen conditions. */
	SUPPORT_MATRIX_TYPE_GK,

	/** Optimized Gardner-Kiderlen conditions. */
	SUPPORT_MATRIX_TYPE_GK_OPT,

	/** Empty support matrix (for native estimator). */
	SUPPORT_MATRIX_TYPE_EMPTY
} SupportMatrixType;

/** Support matrix type that is used by default. */
const SupportMatrixType DEFAULT_SUPPORT_MATRIX_TYPE =
	SUPPORT_MATRIX_TYPE_GK_OPT;

/** Type of the optimization problem. */
typedef enum
{
	/** L_{\infty} problem. */
	ESTIMATION_PROBLEM_NORM_L_INF,

	/** L_{1} problem. */
	ESTIMATION_PROBLEM_NORM_L_1,

	/** L_{2} problem. */
	ESTIMATION_PROBLEM_NORM_L_2
} EstimationProblemNorm;

/** Type of the optimization problem that is used by default. */
const EstimationProblemNorm DEFAULT_ESTIMATION_PROBLEM_NORM =
	ESTIMATION_PROBLEM_NORM_L_INF;

/**
 * Structure that represents the input data for support function estimation
 * engines.
 */
class SupportFunctionEstimationData
{
private:
	/** The number of support function measurements. */
	int numValues_;

	/**
	 * The number of consistency conditions to be verified to provide
	 * consistent estimation.
	 */
	int numConditions_;

	/** The support matrix of problem. */
	SupportMatrix supportMatrix_;

	/**
	 * The support vector, i. e. the vector of support function
	 * measurements.
	 */
	VectorXd supportVector_;

	/**
	 * Starting point of the algorithm (rude estimate)
	 */
	VectorXd startingVector_;

	/**
	 * Support directions of data.
	 */
	std::vector<Vector3d> supportDirections_;

	/**
	 * Starting epsilon for native estimator.
	 */
	double startingEpsilon_;

	/** The original support function data. */
	SupportFunctionDataPtr supportData_;
public:
	/** Default empty constructor. */
	SupportFunctionEstimationData();

	/** Copy-from-pointer constructor */
	SupportFunctionEstimationData(SupportFunctionEstimationData *data);

	/**
	 * Default constructor.
	 *
	 * @param supportMatrix		The support matrix of problem.
	 * @param supportVector		The support vector, i. e. the vector
	 * 				of support function measurements.
	 * @param startingVector	The starting vector of the algorithm.
	 * @param supportDirections	The vector of support directions
	 * @param startingEpsilon	Starting epsilon for native estimator.
	 * @param supportData		The original support function data.
	 */
	SupportFunctionEstimationData(SupportMatrix supportMatrix,
		VectorXd supportVector, VectorXd startingVector,
		std::vector<Vector3d> supportDirections, double epsilon,
		SupportFunctionDataPtr supportData);

	/** Default destructor. */
	virtual ~SupportFunctionEstimationData();

	/** Getter for member numValues_. */
	int numValues(void);

	/** Getter for member numConditions_. */
	int numConditions(void);

	/** Getter for member supportMatrix_. */
	SupportMatrix supportMatrix(void);

	/** Getter for member supportVector_ */
	VectorXd supportVector(void);

	/** Getter for member startingVector_ */
	VectorXd startingVector(void);

	/** Getter for member supportDirections_ */
	std::vector<Vector3d> supportDirections(void);

	/** Getter for startingEpsilon_ */
	double startingEpsilon(void);

	/** Getter for supportData_ */
	SupportFunctionDataPtr supportData(void);
};

/** Shared pointer for support function estimation data. */
typedef std::shared_ptr<SupportFunctionEstimationData>
	SupportFunctionEstimationDataPtr;

#endif /* SUPPORTFUNCTIONESTIMATIONDATA_H_ */
