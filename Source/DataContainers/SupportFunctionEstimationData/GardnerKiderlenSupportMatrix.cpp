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

#include <CGAL/Nef_polyhedron_3.h>

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "DataContainers/SupportFunctionEstimationData/GardnerKiderlenSupportMatrix.h"
#include "halfspaces_intersection.h"

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

static void addCondition(std::vector<Eigen::Triplet<double>> &triplets,
		int &iCondition, int i, int j, Vector3d direction)
{
	if (i == j)
	{
		DEBUG_END;
		return;
	}

	triplets.push_back(Eigen::Triplet<double>(iCondition, 3 * i,
				direction.x));
	triplets.push_back(Eigen::Triplet<double>(iCondition, 3 * i + 1,
				direction.y));
	triplets.push_back(Eigen::Triplet<double>(iCondition, 3 * i + 2,
				direction.z));
	triplets.push_back(Eigen::Triplet<double>(iCondition, 3 * j,
				-direction.x));
	triplets.push_back(Eigen::Triplet<double>(iCondition, 3 * j + 1,
				-direction.y));
	triplets.push_back(Eigen::Triplet<double>(iCondition, 3 * j + 2,
				-direction.z));
	++iCondition;
}


GardnerKiderlenSupportMatrix *constructGardnerKiderlenSupportMatrix(
		SupportFunctionDataPtr data)
{
	DEBUG_START;
	long int numDirections = data->size();
	long int numValues = 3 * numDirections;
	long int numConditions = numDirections * numDirections - numDirections;
	GardnerKiderlenSupportMatrix *matrix = new GardnerKiderlenSupportMatrix(
			numConditions, numValues);

	std::vector<Vector3d> directions = data->supportDirections();

	std::vector<Eigen::Triplet<double>> triplets;
	int iCondition = 0;
	for (int i = 0; i < numDirections; ++i)
	{
		for (int j = 0; j < numDirections; ++j)
		{
			addCondition(triplets, iCondition, i, j, directions[i]);
		}
	}
	DEBUG_PRINT("triplets.size() = %ld", triplets.size());
	DEBUG_PRINT("numConditions = %ld", numConditions);
	ASSERT(triplets.size() == 6 * (unsigned) numConditions);
	matrix->setFromTriplets(triplets.begin(), triplets.end());
	DEBUG_END;
	return matrix;
}

/**
 * Gets duals of support planes which free coefficients were incremented on a
 * given number.
 *
 * @param data		Support dunction data.
 * @param epsilon	The shifting number.
 *
 * @return		Duals of shifted support planes.
 */
static std::vector<Point_3> getShiftedDualPoints(SupportFunctionDataPtr data,
		double epsilon)
{
	DEBUG_START;
	std::vector<Point_3> points;
	long int numDirections = data->size();
	for (int i = 0; i < numDirections; ++i)
	{
		auto item = (*data)[i];
		Plane_3 plane(item.direction.x, item.direction.y,
				item.direction.z, -item.value - epsilon);
		points.push_back(dual(plane));
	}
	DEBUG_END;
	return points;
}

#include <CGAL/Delaunay_triangulation_3.h>
typedef CGAL::Delaunay_triangulation_3<Kernel> Delaunay;

GardnerKiderlenSupportMatrix *constructReducedGardnerKiderlenSupportMatrix(
		SupportFunctionDataPtr data, double epsilon)
{
	DEBUG_START;

	/* Get duals of higher and lower support planes */
	auto pointsHigher = getShiftedDualPoints(data, epsilon);
	auto pointsLower = getShiftedDualPoints(data, -epsilon);

	/* Construct 3D Delaunay triangulation of points. */
	Delaunay triangulation;
	triangulation.insert(pointsHigher.begin(), pointsHigher.end());

	/* Find needed conditions. */
	long int numDirections = data->size();
	std::vector<Vector3d> directions = data->supportDirections();
	std::vector<Eigen::Triplet<double>> triplets;
	int iCondition = 0;
	int numSkipped = 0;
	for (int i = 0; i < numDirections; ++i)
	{
		/* Calculate the allowability body for i-th direction. */
		auto vertex = triangulation.insert(pointsLower[i]);

		for (int j = 0; j < numDirections; ++j)
		{
			if (j == i)
				continue;
			auto cell = triangulation.inexact_locate(
					pointsLower[j]);
			if (!triangulation.is_infinite(cell))
			{
				addCondition(triplets, iCondition, i, j,
						directions[i]);
			}
			else
			{
				DEBUG_PRINT("(%d, %d) condition was skipped",
						i, j);
				++numSkipped;
			}
		}

		/* Return triangulation in the initial state. */
		triangulation.remove(vertex);
	}
	long int numValues = 3 * numDirections;
	long int numConditions = triplets.size() / 6;
	DEBUG_PRINT("Number of skipped directions: %d (%lf from total)",
			numSkipped, ((double) numSkipped) / numConditions);
	DEBUG_PRINT("Number of conditions: %ld", numConditions);

	GardnerKiderlenSupportMatrix *matrix = new GardnerKiderlenSupportMatrix(
			numConditions, numValues);
	matrix->setFromTriplets(triplets.begin(), triplets.end());
	DEBUG_END;
	return matrix;
}
