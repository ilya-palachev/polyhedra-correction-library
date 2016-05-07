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
 * @file QuadApproxSupportFunctionEstimator.cpp
 * @brief Estimation engine based on iterative solution of quadratic
 * approximations of the initial problem (definitiona).
 */

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "Recoverer/QuadApproxSupportFunctionEstimator.h"

QuadApproxSupportFunctionEstimator::QuadApproxSupportFunctionEstimator(
		SupportFunctionEstimationDataPtr data) :
	SupportFunctionEstimator(data)
{
	DEBUG_START;
	DEBUG_END;
}

QuadApproxSupportFunctionEstimator::~QuadApproxSupportFunctionEstimator()
{
	DEBUG_START;
	DEBUG_END;
}

VectorXd QuadApproxSupportFunctionEstimator::run(void)
{
	DEBUG_START;
	auto supportData = data->supportData();
	auto values = supportData->supportValues();
	auto directions = supportData->supportDirections();
	std::cerr << "We're going to minimize problem with "
		<< directions.size() << " directions..." << std::endl;
	auto startingVector = data->startingVector();
	std::cerr << "Starting vector has " << startingVector.size()
		<< " coordinates..." << std::endl;
	ERROR_PRINT("Not implemented yet!");
	exit(EXIT_FAILURE);
	DEBUG_END;
	return VectorXd(1);
}
