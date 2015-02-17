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
 * @file SupportFunctionEstimationDataConstructor.h
 * @brief Constructor of data used for support function estimation process
 * - declaraion.
 */

#ifndef SUPPORT_FUNCTION_ESTIMATION_DATA_CONSTRUCTOR_H
#define SUPPORT_FUNCTION_ESTIMATION_DATA_CONSTRUCTOR_H

#include "DataContainers/SupportFunctionEstimationData/SupportFunctionEstimationData.h"
#include "DataContainers/SupportFunctionData/SupportFunctionData.h"

/**
 * Type of body from which the starting vector of estimation process is
 * constructed.
 */
typedef enum
{
	/** Body is intersection of halfspaces formed by shadow cylinders. */
	SUPPORT_FUNCTION_ESTIMATION_STARTING_BODY_TYPE_CYLINDERS_INTERSECTION,

	/** Body is convex hull of support points (u_i * h_i). */
	SUPPORT_FUNCTION_ESTIMATION_STARTING_BODY_TYPE_POINTS_HULL,

	/** Body is sphere (each support value is = 1). */
	SUPPORT_FUNCTION_ESTIMATION_STARTING_BODY_TYPE_SPHERE,

	/** Body is cube. */
	SUPPORT_FUNCTION_ESTIMATION_STARTING_BODY_TYPE_CUBE,

	/** Body is pyramid. */
	SUPPORT_FUNCTION_ESTIMATION_STARTING_BODY_TYPE_PYRAMID,

	/** Body is prism. */
	SUPPORT_FUNCTION_ESTIMATION_STARTING_BODY_TYPE_PRISM,
} SupportFunctionEstimationStartingBodyType;

/** Default type of the starting body. */
#define DEFAULT_STARTING_BODY_TYPE \
	SUPPORT_FUNCTION_ESTIMATION_STARTING_BODY_TYPE_CYLINDERS_INTERSECTION

/** Constructor of data used for support function estimation process */
class SupportFunctionEstimationDataConstructor
{
private:
	/** Whether to scale matrix. */
	bool ifScaleMatrix;
public:
	/** Empty constructor. */
	SupportFunctionEstimationDataConstructor();

	/** Destructor. */
	~SupportFunctionEstimationDataConstructor();

	/** Enables matrix scaling */
	void enableMatrixScaling();

	/**
	 * Constructs data used for support function estimation from general
	 * support function data.
	 *
	 * @param data			Support function data in general form.
	 * @param supportMatrixType	Support function type.
	 * @param startingBodyType	Starting body type
	 *
	 * @return			Support function estimation data ready
	 * 				to be processed.
	 */
	SupportFunctionEstimationDataPtr run(SupportFunctionDataPtr data,
		SupportMatrixType supportMatrixType,
		SupportFunctionEstimationStartingBodyType startingBodyType);

	/**
	 * Checks the result of estimation process.
	 *
	 * @param data			Support function estimation data.
	 * @param supportMatrixType     Support function type.
	 * @param estimate		The estimate obtained from estimation
	 * 				proces.
	 *
	 * @return			True, if the result is correct.
	 */
	bool checkResult(SupportFunctionEstimationDataPtr data,
			SupportMatrixType supportMatrixType, VectorXd estimate);
};

#endif /* SUPPORT_FUNCTION_ESTIMATION_DATA_CONSTRUCTOR_H */
