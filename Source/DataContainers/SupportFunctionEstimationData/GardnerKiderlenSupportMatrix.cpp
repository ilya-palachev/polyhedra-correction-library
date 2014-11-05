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
	long int numValues = data->size();
	long int numConditions = numValues * numValues - numValues;
	GardnerKiderlenSupportMatrix *matrix = new GardnerKiderlenSupportMatrix(
			numConditions, numValues);

	std::vector<Vector3d> directions = data->supportDirections();
	VectorXd values = data->supportValues();

	std::vector<Eigen::Triplet<double>> triplets;
	int iCondition = 0;
	for (int i = 0; i < numValues; ++i)
	{
		for (int j = 0; j < numValues; ++j)
		{
			if (j == i)
				continue;
			triplets.push_back(Eigen::Triplet<double>(
				iCondition, i, 1.));
			triplets.push_back(Eigen::Triplet<double>(
				iCondition, j, -directions[i] * directions[j]));
			++iCondition;
		}
	}
	matrix->setFromTriplets(triplets.begin(), triplets.end());
	DEBUG_END;
	return matrix;
}

GardnerKiderlenSupportMatrix *constructReducedGardnerKiderlenSupportMatrix(
		SupportFunctionDataPtr data)
{
	DEBUG_START;
	Polyhedron_3 hull = buildDirectionsHull(data->supportDirectionsCGAL());
	ASSERT(hull.size_of_vertices() == (unsigned) data->size());
	long int numValues = hull.size_of_vertices();
	long int numConditions = hull.size_of_halfedges();
	GardnerKiderlenSupportMatrix *matrix = new GardnerKiderlenSupportMatrix(
			numConditions, numValues);

	std::vector<Eigen::Triplet<double>> triplets;
	int iCondition = 0;
	for (auto halfedge = hull.halfedges_begin();
			halfedge != hull.halfedges_end(); ++halfedge)
	{
		Point_3 begin = halfedge->prev()->vertex()->point();
		int idBegin = halfedge->prev()->vertex()->id;
		Point_3 end = halfedge->vertex()->point();
		int idEnd = halfedge->vertex()->id;

		double product = (begin - CGAL::ORIGIN) * (end - CGAL::ORIGIN);

		triplets.push_back(Eigen::Triplet<double>(
					iCondition, idBegin, 1.));
		triplets.push_back(Eigen::Triplet<double>(
					iCondition, idEnd, product));
		++iCondition;
	}
	ASSERT(iCondition == numConditions);
	matrix->setFromTriplets(triplets.begin(), triplets.end());
	DEBUG_END;
	return matrix;
}
