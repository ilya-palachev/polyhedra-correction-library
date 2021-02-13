/*
 * Copyright (c) 2009-2017 Ilya Palachev <iliyapalachev@gmail.com>
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
 * @file NativeEstimatorCommonFunctions.cpp
 * @brief Common functions of native estimators (implementation).
 */
#include "DebugPrint.h"
#include "DebugAssert.h"
#include "PCLDumper.h"
#include "Polyhedron_3/Polyhedron_3.h"
#include <CGAL/Bbox_3.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include "NativeEstimatorCommonFunctions.h"

typedef CGAL::Triangulation_data_structure_3<
	CGAL::Triangulation_vertex_base_3<Kernel>,
	CGAL::Triangulation_cell_base_3<Kernel>, CGAL::Parallel_tag>
	Tds;
typedef CGAL::Delaunay_triangulation_3<Kernel, Tds> Delaunay;

std::vector<int> collectInnerPointsIDs(std::vector<Point_3> points)
{
	DEBUG_START;
	Delaunay::Lock_data_structure locking_ds(
		CGAL::Bbox_3(-1000., -1000., -1000., 1000., 1000., 1000.), 50);
	Delaunay triangulation(points.begin(), points.end(), &locking_ds);
	auto infinity = triangulation.infinite_vertex();

	std::vector<int> outerPlanesIDs;
	int numPlanes = points.size();
	for (int i = 0; i < numPlanes; ++i)
	{
		Point_3 point = points[i];
		Delaunay::Locate_type lt;
		int li, lj;
		Delaunay::Cell_handle c = triangulation.locate(point, lt, li, lj);

		auto vertex = c->vertex(li);
		ASSERT(lt == Delaunay::VERTEX && vertex->point() == point);
		if (!triangulation.is_edge(vertex, infinity, c, li, lj))
		{
			outerPlanesIDs.push_back(i);
		}
	}
	DEBUG_END;
	return outerPlanesIDs;
}

std::set<int> findTangientPointPlanesIDs(Polyhedron_3 *polyhedron,
										 Polyhedron_3::Vertex_iterator vertex,
										 std::vector<int> index)
{
	DEBUG_START;
	auto circulatorFirst = vertex->vertex_begin();
	auto circulator = circulatorFirst;
	std::set<int> planesIDs;
	std::vector<int> planesIDsVector;
	do
	{
		int iFacet = circulator->facet()->id;
		if (iFacet > (int)polyhedron->size_of_facets())
		{
			ERROR_PRINT("%d > %ld", iFacet, polyhedron->size_of_facets());
			exit(EXIT_FAILURE);
		}
		planesIDs.insert(index[iFacet]);
		planesIDsVector.push_back(index[iFacet]);
		++circulator;
	} while (circulator != circulatorFirst);

	if (planesIDs.size() != 3)
	{
		ERROR_PRINT("%d != %d", (int)planesIDs.size(), 3);
		std::cerr << "Indices:";
		for (int i : planesIDsVector)
			std::cerr << " " << i;
		std::cerr << std::endl;
		std::cerr << "degree of vertex " << vertex->id << " is "
				  << vertex->degree() << std::endl;
		exit(EXIT_FAILURE);
	}
	DEBUG_END;
	return planesIDs;
}

VectorXd calculateSolution(SupportFunctionDataPtr data, VectorXd values)
{
	DEBUG_START;
	VectorXd difference = values - data->supportValues();
	std::vector<double> epsilons;
	for (int i = 0; i < (int)difference.size(); ++i)
	{
		epsilons.push_back(difference(i));
	}

	auto points = data->getShiftedDualPoints_3(epsilons);
	auto innerIndex = collectInnerPointsIDs(points);
	std::cerr << innerIndex.size() << " points are inner" << std::endl;

	std::vector<Plane_3> planes;
	auto directions = data->supportDirections<Point_3>();
	for (int i = 0; i < (int)directions.size(); ++i)
	{
		auto direction = directions[i];
		Plane_3 plane(direction.x(), direction.y(), direction.z(), -values(i));
		planes.push_back(plane);
	}
	Polyhedron_3 polyhedron(planes);
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG, "recovered-by-native-estimator.ply")
		<< polyhedron;

	VectorXd solution = polyhedron.findTangientPointsConcatenated(directions);
	DEBUG_END;
	return solution;
}
