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
 * @file experimentCGAL_convexHullDynamic.cpp
 *
 * @brief Here the functionality of dynamic convex hull algorithm from CGAL is
 * tested.
 *
 * @note This file is based on example from CGAL manual:
 * http://doc.cgal.org/latest/Convex_hull_3/index.html#Chapter_3D_Convex_Hulls
 */

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/point_generators_3.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/convex_hull_3_to_polyhedron_3.h>
#include <CGAL/algorithm.h>
#include <list>

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "TimeMeasurer/TimeMeasurer.h"

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_3 Point_3;
typedef CGAL::Delaunay_triangulation_3<K> Delaunay;
typedef Delaunay::Vertex_handle Vertex_handle;
typedef CGAL::Polyhedron_3<K> Polyhedron_3;

/**
 * Prints the usage of this test.
 */
static void print_usage(int argc, char** argv)
{
	printf("Usage: %s <num_of_points>\n", argv[0]);
}

/**
 * Defines the factor of points reduction, i. e. if it is = 10, then 90% of
 * points will be reduced.
 */
#define FACTOR_OF_VERTICES_REDUCTION 10


/**
 * Performs the testing of dynamic convex hull of CGAL.
 */
int main(int argc, char** argv)
{
	DEBUG_START;
	if (argc != 2)
	{
		print_usage(argc, argv);
		DEBUG_END;
		return EXIT_FAILURE;
	}

	int num_points = 0;
	if (sscanf(argv[1], "%d", &num_points) != 1)
	{
		print_usage(argc, argv);
		DEBUG_END;
		return EXIT_FAILURE;
	}

	CGAL::Random_points_on_sphere_3<Point_3> gen(100.0);
	std::list<Point_3> points;
	
	/*
	 * generate <num_points> points randomly on a sphere of radius 100.0
	 * and copy them to a std::vector
	 */
	CGAL::cpp11::copy_n(gen, num_points, std::back_inserter(points) );
	
	/* begin time measurement */
	TimeMeasurer timer;
	timer.pushTimer();
	Delaunay T;

	T.insert(points.begin(), points.end());
	std::list<Vertex_handle> vertices;
	T.incident_vertices(T.infinite_vertex(), std::back_inserter(vertices));
	
	/*
	 * copy the convex hull of points into a polyhedron and use it
	 * to get the number of points on the convex hull
	 */
	Polyhedron_3 chull0;
	CGAL::convex_hull_3_to_polyhedron_3(T,chull0);
	
	std::cout << "The convex hull contains " << chull0.size_of_vertices()
		<< " vertices" << std::endl;
	
	/* end time measurement */
	double timeFirst = timer.popTimer();
	printf("Time for initial construction of convex hull: %lf\n", timeFirst);
	
	/* begin time measurement */
	timer.pushTimer();
	
	/* Remove 90% of points. */
	float nReduced = (1. - 1. / (float) FACTOR_OF_VERTICES_REDUCTION) *
		chull0.size_of_vertices();
		
	std::list<Vertex_handle>::iterator v_set_it = vertices.begin();
	for (int i = 0; i < nReduced; i++)
	{
		T.remove(*v_set_it);
		v_set_it++;
	}

	/*
	 * copy the convex hull of points into a polyhedron and use it
	 * to get the number of points on the convex hull
	 */
	Polyhedron_3 chull;
	CGAL::convex_hull_3_to_polyhedron_3(T,chull);
	
	/* end time measurement */
	double timeSecond = timer.popTimer();
	printf("Time for recalculating of convex hull: %lf\n", timeSecond);
	
	std::cout << "The convex hull contains " << chull.size_of_vertices()
		<< " vertices" << std::endl;

	DEBUG_END;
	return 0;
}
