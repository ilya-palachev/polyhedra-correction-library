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
 * @brief Definition of structure that represents the input data for support
 * function estimation engines.
 */

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "SupportFunctionEstimationData.h"

/** The theory works only when number of measurements is >= 5 */
#define MINIMAL_NUMBER_OF_SUPPORT_FUNCTION_MEASUREMENTS 5

SupportFunctionEstimationData::SupportFunctionEstimationData()
{
	DEBUG_START;
	ASSERT(0 && "Do not try to create empty SupportFunctionEstimationData !");
	DEBUG_END;
}

SupportFunctionEstimationData::SupportFunctionEstimationData(
	SupportFunctionEstimationData *data) :
	m_numValues(data->numValues()),
	m_numConditions(data->numConditions()),
	m_supportMatrix(data->supportMatrix()),
	m_supportVector(data->supportVector()),
	m_startingVector(data->startingVector()),
	m_supportDirections(data->supportDirections())
{
	DEBUG_START;
	ASSERT(m_numValues >= MINIMAL_NUMBER_OF_SUPPORT_FUNCTION_MEASUREMENTS);
	ASSERT(m_supportMatrix.rows() == m_numConditions);
	ASSERT(m_supportMatrix.cols() == m_numValues);
	ASSERT(m_numValues == m_supportVector.rows());
	ASSERT(m_numValues == m_startingVector.rows());
	DEBUG_END;
}

SupportFunctionEstimationData::SupportFunctionEstimationData(
	SparseMatrix supportMatrix, VectorXd supportVector,
	VectorXd startingVector, std::vector<Vector3d> supportDirections) :
	m_numValues(supportMatrix.cols()),
	m_numConditions(supportMatrix.rows()),
	m_supportMatrix(supportMatrix),
	m_supportVector(supportVector),
	m_startingVector(startingVector),
	m_supportDirections(supportDirections)
{
	DEBUG_START;
	ASSERT(m_numValues >= MINIMAL_NUMBER_OF_SUPPORT_FUNCTION_MEASUREMENTS);
	ASSERT(m_supportMatrix.rows() == m_numConditions);
	ASSERT(m_supportMatrix.cols() == m_numValues);
	ASSERT(m_numValues == m_supportVector.rows());
	ASSERT(m_numValues == m_startingVector.rows());
	ASSERT(m_numValues == m_supportDirections.size());
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
	return m_numValues;
}

int SupportFunctionEstimationData::numConditions(void)
{
	DEBUG_START;
	DEBUG_END;
	return m_numConditions;
}

SparseMatrix SupportFunctionEstimationData::supportMatrix(void)
{
	DEBUG_START;
	DEBUG_END;
	return m_supportMatrix;
}

VectorXd SupportFunctionEstimationData::supportVector(void)
{
	DEBUG_START;
	DEBUG_END;
	return m_supportVector;
}

VectorXd SupportFunctionEstimationData::startingVector(void)
{
	DEBUG_START;
	DEBUG_END;
	return m_startingVector;
}

std::vector<Vector3d> SupportFunctionEstimationData::supportDirections(void)
{
	DEBUG_START;
	DEBUG_END;
	return m_supportDirections;
}

