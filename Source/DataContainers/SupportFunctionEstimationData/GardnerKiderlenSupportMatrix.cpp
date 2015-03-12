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
 * @file GardnerKiderlenSupportMatrix.cpp
 * @brief Support matrix based on the approach described by Gardner and
 * Kiderlen 
 * - implementation.
 */

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
	DEBUG_START;
	if (i == j)
	{
		DEBUG_END;
		return;
	}
	DEBUG_PRINT("Adding condition (%d, %d)", i, j);

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
	DEBUG_END;
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

# if 1
double GardnerKiderlenSupportMatrix::epsilonFactor = 1.;

GardnerKiderlenSupportMatrix *constructReducedGardnerKiderlenSupportMatrix(
		SupportFunctionDataPtr data, double epsilon)
{
	DEBUG_START;
	std::cout << "epsilon = " << epsilon << std::endl;
	epsilon *= GardnerKiderlenSupportMatrix::epsilonFactor;
	std::cout << "epsilon * factor = " << epsilon << std::endl;

	/* Get duals of higher and lower support planes */
	auto pointsHigher = data->getShiftedDualPoints(epsilon);
	auto pointsLower = data->getShiftedDualPoints(-epsilon);

	CGALPolyhedron hull;
	CGAL::convex_hull_3(pointsHigher.begin(), pointsHigher.end(), hull);
	Tree tree(faces(hull).first, faces(hull).second, hull);
	tree.accelerate_distance_queries();

	/* Find needed conditions. */
	long int numDirections = data->size();
	std::vector<Vector3d> directions = data->supportDirections();
	std::vector<Eigen::Triplet<double>> triplets;
	int iCondition = 0;
	int numSkipped = 0;
	for (int i = 0; i < numDirections; ++i)
	{
		int numConditionsForOne = 0;

		for (int j = i + 1; j < numDirections; ++j)
		{
			Segment query(pointsLower[i], pointsLower[j]);

			if (!tree.do_intersect(query))
			{
				addCondition(triplets, iCondition, i, j,
						directions[i]);
				addCondition(triplets, iCondition, j, i,
						directions[j]);
				++numConditionsForOne;
			}
			else
			{
				++numSkipped;
			}
		}
		DEBUG_PRINT("Conditions for %d-th point: %d\n", i,
				numConditionsForOne);
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
#endif

#if 0
GardnerKiderlenSupportMatrix *constructReducedGardnerKiderlenSupportMatrix(
		SupportFunctionDataPtr data, double epsilon)
{
	DEBUG_START;
	auto directions = data->supportDirectionsCGAL();
	Polyhedron_3 hull;
	CGAL::convex_hull_3(directions.begin(), directions.end(), hull);

	long int numDirections = data->size();
	for (auto vertex = hull.vertices_begin();
			vertex != hull.vertices_end(); ++vertex)
	{
		Point_3 point = vertex->point();
		double minDist = 0.;
		int iMin = -1;
		for (int i = 0; i < numDirections; ++i)
		{
			double dist = (point - directions[i]).squared_length();
			if (dist < minDist || iMin == -1)
			{
				iMin = i;
				minDist = dist;
			}
		}
		DEBUG_PRINT("Found closest vertex #%d, dist = %lf", iMin,
				minDist);
		vertex->id = iMin;
	}

	std::vector<Eigen::Triplet<double>> triplets;
	int iCondition = 0;
	int numConditions = 0;
	for (auto halfedge = hull.halfedges_begin();
			halfedge != hull.halfedges_end(); ++halfedge)
	{
		int i = halfedge->prev()->vertex()->id;
		ASSERT(i >= 0);
		int j = halfedge->vertex()->id;
		ASSERT(j >= 0);
		addCondition(triplets, iCondition, i, j, directions[i]);
		++numConditions;
	}

	ALWAYS_PRINT(stdout, "Number of conditions: %d\n", numConditions);
	ALWAYS_PRINT(stdout, "Number of directions: %ld\n", numDirections);

	GardnerKiderlenSupportMatrix *matrix = new GardnerKiderlenSupportMatrix(
			numConditions, numDirections * 3);
	matrix->setFromTriplets(triplets.begin(), triplets.end());
	DEBUG_END;
	return matrix;
}

#endif