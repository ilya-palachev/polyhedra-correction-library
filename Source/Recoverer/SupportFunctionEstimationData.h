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
 * @brief Declaration of structure that represents the input data for support
 * function estimation engines.
 */

#ifndef SUPPORTFUNCTIONESTIMATIONDATA_H_
#define SUPPORTFUNCTIONESTIMATIONDATA_H_

#include "SparseMatrixEigen.h"

/**
 * Structure that represents the input data for support function estimation
 * engines.
 */
class SupportFunctionEstimationData
{
private:
	/** The number of support function measurements. */
	int m_numValues;

	/**
	 * The number of consistency conditions to be verified to provide consistent
	 * estimation.
	 */
	int m_numConditions;

	/** The support matrix of problem. */
	SparseMatrix m_supportMatrix;

	/**
	 * The support vector, i. e. the vector of support function
	 * measurements.
	 *
	 * TODO: Do we need to use some useful Eigen class here?
	 */
	VectorXd m_supportVector;

public:
	/** Default empty constructor. */
	SupportFunctionEstimationData();

	/**
	 * Default constructor.
	 *
	 * @param numValues		The number of support function measurements.
	 * @param numConditions	The number of consistency conditions to be verified
	 * 						to provide consistent estimation.
	 * @param supportMatrix	The support matrix of problem.
	 * @param supportVector	The support vector, i. e. the vector of support
	 * 						function measurements.
	 */
	SupportFunctionEstimationData(int numValues,
			int numConditions, SparseMatrix supportMatrix,
			VectorXd supportVector);

	/** Default destructor. */
	~SupportFunctionEstimationData();

	/** Getter for member m_numValues. */
	int numValues(void);

	/** Getter for member m_numConditions. */
	int numConditions(void);

	/** Getter for member m_supportMatrix. */
	SparseMatrix supportMatrix(void);

	/** Getter for member m_supportVector */
	VectorXd supportVector(void);

	/* FIXME: Do we need to have setters here? */
};

#endif /* SUPPORTFUNCTIONESTIMATIONDATA_H_ */
