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

#define CGAL_LINKED_WITH_TBB 1

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

#include <CGAL/Bbox_3.h>
#include <CGAL/Delaunay_triangulation_3.h>
typedef CGAL::Triangulation_data_structure_3<
	CGAL::Triangulation_vertex_base_3<Kernel>,
	CGAL::Triangulation_cell_base_3<Kernel>,
	CGAL::Parallel_tag> Tds;
typedef CGAL::Delaunay_triangulation_3<Kernel, Tds> Delaunay;

static void reportLocationType(Delaunay::Locate_type locationType)
{
	switch (locationType)
	{
	case Delaunay::VERTEX:
		DEBUG_PRINT("Delaunay::VERTEX");
		break;
	case Delaunay::EDGE:
		DEBUG_PRINT("Delaunay::EDGE");
		break;
	case Delaunay::FACET:
		DEBUG_PRINT("Delaunay::FACET");
		break;
	case Delaunay::CELL:
		DEBUG_PRINT("Delaunay::CELL");
		break;
	case Delaunay::OUTSIDE_CONVEX_HULL:
		DEBUG_PRINT("Delaunay::OUTSIDE_CONVEX_HULL");
		break;
	case Delaunay::OUTSIDE_AFFINE_HULL:
		DEBUG_PRINT("Delaunay::OUTSIDE_AFFINE_HULL");
		break;
	}
}

GardnerKiderlenSupportMatrix *constructReducedGardnerKiderlenSupportMatrix(
		SupportFunctionDataPtr data, double epsilon)
{
	DEBUG_START;
	std::cout << "epsilon = " << epsilon << std::endl;

	/* Get duals of higher and lower support planes */
	auto pointsHigher = getShiftedDualPoints(data, epsilon);
	auto pointsLower = getShiftedDualPoints(data, -epsilon);

	/* Construct 3D Delaunay triangulation of points. */
	Delaunay::Lock_data_structure locking_ds(
			CGAL::Bbox_3(-1000., -1000., -1000., 1000., 1000.,
				1000.), 50);
	Delaunay triangulation(pointsHigher.begin(), pointsHigher.end(),
			&locking_ds);
	//auto infinity = triangulation.infinite_vertex();

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
		int numConditionsForOne = 0;

		for (int j = 0; j < numDirections; ++j)
		{
			if (j == i)
				continue;
			DEBUG_PRINT("Checking condition for (%d, %d)", i, j);
			int locationIndexI, locationIndexJ;
			Delaunay::Locate_type locationType;
			auto cell = triangulation.locate(pointsLower[j],
					locationType, locationIndexI,
					locationIndexJ);
			reportLocationType(locationType);
			if (!triangulation.is_infinite(cell))
			{
				addCondition(triplets, iCondition, i, j,
						directions[i]);
				++numConditionsForOne;
			}
			else
			{
				++numSkipped;
			}
		}
		DEBUG_PRINT("Conditions for %d-th point: %d\n", i,
				numConditionsForOne);

		/* Return triangulation in the initial state. */
		triangulation.remove(vertex);
	}
	long int numValues = 3 * numDirections;
	long int numConditions = triplets.size() / 6;
	ALWAYS_PRINT(stdout, "Number of skipped directions: %d (%lf from "
			"total)\n", numSkipped,
			((double) numSkipped) / (numSkipped + numConditions));
	ALWAYS_PRINT(stdout, "Number of conditions: %ld\n", numConditions);

	GardnerKiderlenSupportMatrix *matrix = new GardnerKiderlenSupportMatrix(
			numConditions, numValues);
	matrix->setFromTriplets(triplets.begin(), triplets.end());
	DEBUG_END;
	return matrix;
}
