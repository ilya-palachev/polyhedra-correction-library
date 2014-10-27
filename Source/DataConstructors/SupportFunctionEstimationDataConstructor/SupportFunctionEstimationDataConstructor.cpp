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
 * @file SupportFunctionEstimationDataConstructor.p
 * @brief Constructor of data used for support function estimation process
 * - implementation.
 */

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "DataConstructors/SupportFunctionEstimationDataConstructor/SupportFunctionEstimationDataConstructor.h"

/**
 * Builds hull of directions by given support function data.
 *
 * @param data	Original data.
 * @return Hull of directions.
 */
static Polyhedon_3 buildDirectionsHull(SupportFunctionDataPtr data);

/**
 * Builds support matrix from given hull of support directions.
 *
 * @param hull	Hull of directions.
 * @return Support matrix.
 */

SupportFunctionEstimationDataConstructor::SupportFunctionEstimationDataConstructor()
{
	DEBUG_START;
	DEBUG_END;
}

SupportFunctionEstimationDataConstructor::~SupportFunctionEstimationDataConstructor()
{
	DEBUG_START;
	DEBUG_END;
}

SupportFunctionEstimationDataPtr SupportFunctionEstimationDataConstructor::run(
		SupportFunctionDataPtr data)
{
	DEBUG_START;
	/* Remove equal items from data. */
	SupportFunctionDataPtr dataNonequal = data->removeEqual();

	/* Build hull of directions. */
	Polyhedron_3 hull = buildDirectionsHull(dataNonequal);

	/* Build support matrix. */
	SparseMatrix supportMatrix = buildSupportMatrix(hull);

	/* Build support vector. */
	VectorXd supportVector = buildSupportVector(dataNonequal);

	/* Build starting vector. */
	VectorXd startingVector = buildStartingVector(dataNonequal);

	/* Build support directions. */
	std::vector<Vector3d> supportDirections;
	for (long int i = 0; i < data->size(); ++i)
	{
		Vector3d v = (*data)[i];
		supportDirections.push_back(v);
	}

	/* Construct data. */
	SupportFunctionEstimationDataPtr estimationData = new
		SupportFunctionEstimationData(supportMatrix, supportVector,
				startingVector, supportDirections);

	DEBUG_END;
	return estimationData;
}

static Polyhedon_3 buildDirectionsHull(SupportFunctionDataPtr data)
{
	DEBUG_START;

	/* Get vector of support directions. */
	std::vector<Point_3> directions;
	for (long int i = 0; i < data->size(); ++i)
	{
		Vector3d v = (*data)[i];
		directions.push_back(Point_3(v.x, v.y, v.z));
	}

	/* Construct convex hull of support directions. */
	Polyhedron_3 hull;
	CGAL::convex_hull_3(directions.begin(), directions.end(), hull);
	ASSERT(hull.size_of_vertices() == directions.size());

	/* Find correpondance between directions and vertices of polyhedron. */
	for (auto vertex = hull.vertices_begin(); vertex != hull.vertices_end();
		++vertex)
	{
		auto point = vertex->point();
		Vector3d vPoly(point.x(), point.y(), point.z());
		double distMin = 0.; long int iMin = 0;
		for (long int i = 0; i < data->size(); ++i)
		{
			Vector3d v = (*data)[i];
			double dist = sqrt(qmod(vPoly - v));
			if (dist < distMin || i == 0.)
			{
				distMin = dist;
				iMin = i;
			}
		}
		vertex->id = iMin;
		DEBUG_PRINT("Minimal distance for %-th vertex of hull is %le",
				distMin);
		ASSERT(equal(distMin, 0.));
	}

	DEBUG_END;
	return hull;
}
