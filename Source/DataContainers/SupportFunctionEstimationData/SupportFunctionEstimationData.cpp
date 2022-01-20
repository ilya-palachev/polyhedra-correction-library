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
 * @file SupportFunctionEstimationData.cpp
 * @brief Structure that represents the input data for support
 * function estimation engines
 * - implementation.
 */

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "DataContainers/SupportFunctionEstimationData/SupportFunctionEstimationData.h"

/** The theory works only when number of measurements is >= 5 */
#define MINIMAL_NUMBER_OF_SUPPORT_FUNCTION_MEASUREMENTS 5

SupportFunctionEstimationData::SupportFunctionEstimationData()
{
	DEBUG_START;
	ASSERT(0 && "Do not try to create empty SupportFunctionEstimationData !");
	DEBUG_END;
}

SupportFunctionEstimationData::SupportFunctionEstimationData(SupportFunctionEstimationData *data) :
	numValues_(data->numValues()),
	numConditions_(data->numConditions()),
	supportMatrix_(data->supportMatrix()),
	supportVector_(data->supportVector()),
	startingVector_(data->startingVector()),
	supportDirections_(data->supportDirections()),
	startingEpsilon_(0.),
	supportData_(NULL),
	ifShadowHeuristics_(false)
{
	DEBUG_START;
	ASSERT(numValues_ >= MINIMAL_NUMBER_OF_SUPPORT_FUNCTION_MEASUREMENTS);
	ASSERT(supportMatrix_.rows() == numConditions_);
	ASSERT(supportMatrix_.cols() == numValues_);
	ASSERT(numValues_ == 3 * supportVector_.rows());
	ASSERT(numValues_ == startingVector_.rows());
	DEBUG_END;
}

SupportFunctionEstimationData::SupportFunctionEstimationData(SupportMatrix supportMatrix, VectorXd supportVector,
															 VectorXd startingVector,
															 std::vector<Vector3d> supportDirections, double epsilon,
															 SupportFunctionDataPtr supportData,
															 bool ifShadowHeuristics) :
	numValues_(supportMatrix.cols()),
	numConditions_(supportMatrix.rows()),
	supportMatrix_(supportMatrix),
	supportVector_(supportVector),
	startingVector_(startingVector),
	supportDirections_(supportDirections),
	startingEpsilon_(epsilon),
	supportData_(supportData),
	ifShadowHeuristics_(ifShadowHeuristics)
{
	DEBUG_START;
	if (supportMatrix.nonZeros() > 0)
	{
		ASSERT(numValues_ >= MINIMAL_NUMBER_OF_SUPPORT_FUNCTION_MEASUREMENTS);
		ASSERT(supportMatrix_.rows() == numConditions_);
		ASSERT(supportMatrix_.cols() == numValues_);
		ASSERT(numValues_ == 3 * supportVector_.rows());
		ASSERT(numValues_ == startingVector_.rows());
		ASSERT(numValues_ == 3 * (int)supportDirections_.size());
	}
	/*
	 * TODO: Check also that number of conditions (i. e. number of edges in the
	 * sphere triangulation) satisfies bounds arrising from Euler's theorem.
	 */
	DEBUG_END;
}

SupportFunctionEstimationData::~SupportFunctionEstimationData()
{
	DEBUG_START;
	DEBUG_END;
}

int SupportFunctionEstimationData::numValues(void)
{
	DEBUG_START;
	DEBUG_END;
	return numValues_;
}

int SupportFunctionEstimationData::numConditions(void)
{
	DEBUG_START;
	DEBUG_END;
	return numConditions_;
}

SupportMatrix SupportFunctionEstimationData::supportMatrix(void)
{
	DEBUG_START;
	DEBUG_END;
	return supportMatrix_;
}

VectorXd SupportFunctionEstimationData::supportVector(void)
{
	DEBUG_START;
	DEBUG_END;
	return supportVector_;
}

VectorXd SupportFunctionEstimationData::startingVector(void)
{
	DEBUG_START;
	DEBUG_END;
	return startingVector_;
}

std::vector<Vector3d> SupportFunctionEstimationData::supportDirections(void)
{
	DEBUG_START;
	DEBUG_END;
	return supportDirections_;
}

double SupportFunctionEstimationData::startingEpsilon(void)
{
	DEBUG_START;
	DEBUG_END;
	return startingEpsilon_;
}

SupportFunctionDataPtr SupportFunctionEstimationData::supportData(void)
{
	DEBUG_START;
	DEBUG_END;
	return supportData_;
}
