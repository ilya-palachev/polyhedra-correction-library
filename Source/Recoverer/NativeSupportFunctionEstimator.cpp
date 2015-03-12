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
 * @file NativeSupportFunctionEstimator.cpp
 * @brief Native estimation engine (declaration).
 */

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "Recoverer/NativeSupportFunctionEstimator.h"

NativeSupportFunctionEstimator::NativeSupportFunctionEstimator(
		SupportFunctionEstimationDataPtr data) :
	SupportFunctionEstimator(data)
{
	DEBUG_START;
	DEBUG_END;
}

NativeSupportFunctionEstimator::~NativeSupportFunctionEstimator()
{
	DEBUG_START;
	DEBUG_END;
}

#include <CGAL/Bbox_3.h>
#include <CGAL/Delaunay_triangulation_3.h>
typedef CGAL::Triangulation_data_structure_3<
	CGAL::Triangulation_vertex_base_3<Kernel>,
		CGAL::Triangulation_cell_base_3<Kernel>,
			CGAL::Parallel_tag> Tds;
typedef CGAL::Delaunay_triangulation_3<Kernel, Tds> Delaunay;

static bool checkEpsilon(SupportFunctionDataPtr data, double epsilon)
{
	DEBUG_START;
	/* Get duals of higher and lower support planes */
	auto pointsHigher = data->getShiftedDualPoints_3(epsilon);
	auto pointsLower = data->getShiftedDualPoints_3(-epsilon);

#if 0
	CGALPolyhedron hull;
	CGAL::convex_hull_3(pointsHigher.begin(), pointsHigher.end(), hull);
	Tree tree(faces(hull).first, faces(hull).second, hull);
	tree.accelerate_distance_queries();
#endif
	/* Construct 3D Delaunay triangulation of points. */
	Delaunay::Lock_data_structure locking_ds(
			CGAL::Bbox_3(-1000., -1000., -1000., 1000., 1000.,
				1000.), 50);
	Delaunay triangulation(pointsHigher.begin(), pointsHigher.end(),
			&locking_ds);
	auto infinity = triangulation.infinite_vertex();

	bool result = true;
	int DEBUG_VARIABLE iPoint = 0;
	for (auto &point: pointsLower)
	{
#if 0
		Point query(point);
		Point_and_primitive_id pp = tree.closest_point_and_primitive(
				query);
		CGALPolyhedron::Face_handle f = pp.second;
		if (f->plane().oriented_side(query) == CGAL::ON_NEGATIVE_SIDE)
		{
			ALWAYS_PRINT(stderr, "stop at point #%d\n", iPoint);
			result = false;
			break;
		}
#endif
		auto cell = triangulation.locate(Point_3(point.x(), point.y(),
					point.z()), infinity);
		if (!triangulation.is_infinite(cell))
		{
			std::cerr << "stop at point #" << iPoint << std::endl;
			result = false;
			break;
		}
		++iPoint;
	}

	DEBUG_END;
	return result;
}

static VectorXd calculateSolution(SupportFunctionDataPtr data, double epsilon)
{
	DEBUG_START;
	VectorXd solution(3 * data->size());
	for (int i = 0; i < data->size(); ++i)
	{
		auto item = (*data)[i];
		Vector3d tangient = item.direction * (item.value - epsilon);
		solution(3 * i) = tangient.x;
		solution(3 * i + 1) = tangient.y;
		solution(3 * i + 2) = tangient.z;
	}
	DEBUG_END;
	return solution;
}

const double BEST_EPSILON_PRECISION = 1e-6;
const double SEARCH_MULTIPLIER = 0.5;

VectorXd NativeSupportFunctionEstimator::run(void)
{
	DEBUG_START;
	SupportFunctionDataPtr supportData = data->supportData();
	double startingEpsilon = data->startingEpsilon();

	double badEpsilon = 0.;
	double goodEpsilon = startingEpsilon;
	int iIteration = 0;
	std::cout << "startingEpsilon = " << startingEpsilon <<std::endl;
	while (goodEpsilon - badEpsilon > BEST_EPSILON_PRECISION)
	{
		double epsilon = badEpsilon +
			(goodEpsilon - badEpsilon) * SEARCH_MULTIPLIER;
		std::cerr << "Native estimator: iteration #" << iIteration <<
			": epsilon = " << epsilon << "... ";
		if (checkEpsilon(supportData, epsilon))
		{
			goodEpsilon = epsilon;
			std::cerr << "SUCCESS" << std::endl;
		}
		else
		{
			badEpsilon = epsilon;
		}
		++iIteration;
	}

	VectorXd solution = calculateSolution(supportData, goodEpsilon);
	DEBUG_END;
	return solution;
}