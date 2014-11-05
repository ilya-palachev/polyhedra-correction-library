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
 * @file GardnerKiderlenSupportMatrix.cpp
 * @brief Support matrix based on the approach described by Gardner and
 * Kiderlen 
 * - implementation.
 */

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "DataContainers/SupportFunctionEstimationData/GardnerKiderlenSupportMatrix.h"

GardnerKiderlenSupportMatrix::GardnerKiderlenSupportMatrix(long int numRows,
		long int numColumns) :
	SupportMatrix(numRows, numColumns)
{
	DEBUG_START;
	DEBUG_END;
}


GardnerKiderlenSupportMatrix::~GardnerKiderlenSupportMatrix()
{
	DEBUG_START;
	DEBUG_END;
}

GardnerKiderlenSupportMatrix *constructGardnerKiderlenSupportMatrix(
		SupportFunctionDataPtr data)
{
	DEBUG_START;
	GardnerKiderlenSupportMatrix *matrix = new GardnerKiderlenSupportMatrix(
			data->size() * data->size() - data->size(),
			data->size());

	std::vector<Vector3d> directions = data->supportDirections();
	VectorXd values = data->supportValues();

	std::vector<Eigen::Triplet<double>> triplets;
	for (int i = 0; i < data->size(); ++i)
	{
		for (int j = 0; j < data->size(); ++j)
		{
			if (j == i)
				continue;
			triplets.push_back(Eigen::Triplet<double>(i, i, 1.));
			triplets.push_back(Eigen::Triplet<double>(i, j,
				-directions[i] * directions[j]));
		}
	}
	matrix->setFromTriplets(triplets.begin(), triplets.end());
	DEBUG_END;
	return matrix;
}

/* TODO: implement this function. */
GardnerKiderlenSupportMatrix *constructReducedGardnerKiderlenSupportMatrix(
		SupportFunctionDataPtr data)
{
	DEBUG_START;
	ASSERT(0 && "Not implemented yet!");
	DEBUG_END;
	return NULL;
}
