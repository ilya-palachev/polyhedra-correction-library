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
 * @file SupportMatrix.cpp
 * @brief General support matrix that contains sparse matrix
 * - implementation.
 */

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "Vector3d.h"
#include "DataContainers/SupportFunctionEstimationData/SupportMatrix.h"

SupportMatrix::SupportMatrix() : SparseMatrix(1, 1)
{
	DEBUG_START;
	DEBUG_END;
}

SupportMatrix::SupportMatrix(long int numRows, long int numColumns) : SparseMatrix(numRows, numColumns)
{
	DEBUG_START;
	DEBUG_END;
}

SupportMatrix::~SupportMatrix()
{
	DEBUG_START;
	DEBUG_END;
}

Polyhedron_3 buildDirectionsHull(std::vector<Point_3> directions)
{
	DEBUG_START;
	/* Construct convex hull of support directions. */
	Polyhedron_3 hull;
	CGAL::convex_hull_3(directions.begin(), directions.end(), hull);
	ASSERT(hull.size_of_vertices() == directions.size());

	/* Find correpondance between directions and vertices of polyhedron. */
	for (auto vertex = hull.vertices_begin(); vertex != hull.vertices_end(); ++vertex)
	{
		auto point = vertex->point();
		/* Find the direction that is nearest to the point. */
		double distMin = 0.; /* Minimal distance. */
		long int iMin = 0;	 /* ID os nearest direction. */
		for (unsigned int i = 0; i < directions.size(); ++i)
		{
			double dist = (directions[i] - point).squared_length();
			if (dist < distMin || i == 0)
			{
				distMin = dist;
				iMin = i;
			}
		}
		vertex->id = iMin; /* Save ID of nearest direction in vertex */
		ASSERT(equal(distMin, 0.));
		ASSERT(equal(point.x(), directions[iMin].x()));
		ASSERT(equal(point.y(), directions[iMin].y()));
		ASSERT(equal(point.z(), directions[iMin].z()));
	}
	ASSERT(checkDirectionsHull(hull));
	DEBUG_END;
	return hull;
}

bool checkDirectionsHull(Polyhedron_3 hull)
{
	DEBUG_START;
	/* Check that all vertices have different IDs. */
	for (auto vertex = hull.vertices_begin(); vertex != hull.vertices_end(); ++vertex)
	{
		for (auto vertexPrev = hull.vertices_begin(); vertexPrev != vertex; ++vertexPrev)
			if (vertexPrev->id == vertex->id)
			{
				DEBUG_END;
				return false;
			}
	}

	/* Check that all facets are triangles. */
	for (auto facet = hull.facets_begin(); facet != hull.facets_end(); ++facet)
		if (!facet->is_triangle())
		{
			DEBUG_END;
			return false;
		}

	DEBUG_END;
	return true;
}
