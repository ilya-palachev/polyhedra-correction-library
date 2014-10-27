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

/** Each condition has exactly not more than  4 non-zero coefficients. */
#define NUM_NONZERO_COEFFICIENTS_IN_CONDITION 4

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
	SparseMatrix supportMatrix_;

	/**
	 * The support vector, i. e. the vector of support function
	 * measurements.
	 */
	VectorXd supportVector_;

	/**
	 * Starting point of the algorith (rude estimate)
	 */
	VectorXd startingVector_;

	/**
	 * Support directions of data.
	 */
	std::vector<Vector3d> supportDirections_;

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
	 */
	SupportFunctionEstimationData(SparseMatrix supportMatrix,
		VectorXd supportVector, VectorXd startingVector,
		std::vector<Vector3d> supportDirections);

	/** Default destructor. */
	virtual ~SupportFunctionEstimationData();

	/** Getter for member numValues_. */
	int numValues(void);

	/** Getter for member numConditions_. */
	int numConditions(void);

	/** Getter for member supportMatrix_. */
	SparseMatrix supportMatrix(void);

	/** Getter for member supportVector_ */
	VectorXd supportVector(void);

	/** Getter for member startingVector_ */
	VectorXd startingVector(void);

	/** Getter for member supportDirections_ */
	std::vector<Vector3d> supportDirections(void);

	/* FIXME: Do we need to have setters here? */
};

/** Shared pointer for support function estimation data. */
typedef std::shared_ptr<SupportFunctionEstimationData>
	SupportFunctionEstimationDataPtr;

#endif /* SUPPORTFUNCTIONESTIMATIONDATA_H_ */
