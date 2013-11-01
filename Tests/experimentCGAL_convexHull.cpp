/*
 * Copyright (c) 2009-2013 Ilya Palachev <iliyapalachev@gmail.com>
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
 * @file experimentCGAL_convexHull.cpp
 *
 * @brief Here the functionality of static convex hull algorithm from CGAL is
 * tested.
 */

#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "DebugPrint.h"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/point_generators_3.h>
#include <CGAL/algorithm.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/convex_hull_3.h>
#include <vector>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Polyhedron_3<K> Polyhedron_3;
typedef K::Segment_3 Segment_3;

/** Define point creator */
typedef K::Point_3 Point_3;
typedef CGAL::Creator_uniform_3<double, Point_3> PointCreator;

/**
 * A functor computing the plane containing a triangular facet
 */
struct Plane_from_facet
{
	Polyhedron_3::Plane_3 operator()(Polyhedron_3::Facet& f)
	{
		Polyhedron_3::Halfedge_handle h = f.halfedge();
		return Polyhedron_3::Plane_3(h->vertex()->point(),
				h->next()->vertex()->point(), h->opposite()->vertex()->point());
	}
};

/**
 * Prints the usage of this test.
 */
static void print_usage(int argc, char** argv)
{
	printf("Usage: %s <num_of_points>\n", argv[0]);
}

/**
 * Subtracts 2 timeval structures result = t2 - t1 (is used for time
 * measurements).
 *
 * @param result Resulting time structure
 * @param t2 The time structure for the end of measurement
 * @param t1 The time structure for the begin of measurement
 *
 * @retvalue 1 if the difference is negative
 * @retvalue 0 otherwise
 */
int timeval_subtract(struct timeval *result, struct timeval *t2,
		struct timeval *t1)
{
	long int diff = (t2->tv_usec + 1000000 * t2->tv_sec)
			- (t1->tv_usec + 1000000 * t1->tv_sec);
	result->tv_sec = diff / 1000000;
	result->tv_usec = diff % 1000000;

	return (diff < 0);
}

/**
 * Prints the time from timeval structure.
 *
 * @param tv The timeval structure
 */
void timeval_print(struct timeval *tv)
{
	char buffer[30];
	time_t curtime;

	printf("%ld.%06ld", tv->tv_sec, tv->tv_usec);
	curtime = tv->tv_sec;
	strftime(buffer, 30, "%m-%d-%Y  %T", localtime(&curtime));
	printf(" = %s.%06ld\n", buffer, tv->tv_usec);
}

/**
 * Defines the factor of points reduction, i. e. if it is = 10, then 90% of
 * points will be reduced.
 */
#define FACTOR_OF_VERTICES_REDUCTION 10


/**
 * Performs the testing of static convex hull of CGAL.
 */
int main(int argc, char** argv)
{
	DEBUG_START;
	struct timeval tvBegin, tvEnd, tvDiff;

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

	DEBUG_PRINT("1. Creating point generator:");
	CGAL::Random_points_on_sphere_3<Point_3, PointCreator> gen(100.0);

	/*
	 * generate <num_points> points randomly on a sphere of radius 100.0
	 * and copy them to a vector
	 */
	std::vector<Point_3> points;
	DEBUG_PRINT("2. Generating points on sphere:");
	CGAL::cpp11::copy_n(gen, num_points, std::back_inserter(points));

	int i_point = 0;
	for (auto it_point = points.begin(); it_point != points.end(); ++it_point)
	{
		DEBUG_VARIABLE double x = it_point->x();
		DEBUG_VARIABLE double y = it_point->y();
		DEBUG_VARIABLE double z = it_point->z();
		DEBUG_VARIABLE double norm = sqrt(x * x + y * y + z * z);
		DEBUG_PRINT("points[%d] = (%lf, %lf, %lf), norm = %lf\n",
				i_point, x, y, z, norm);
		++i_point;
	}

	/* define polyhedron to hold convex hull */
	Polyhedron_3 poly;

	/* compute convex hull of non-collinear points */

	/* begin time measurement */
	gettimeofday(&tvBegin, NULL);
	timeval_print(&tvBegin);

	DEBUG_PRINT("3. Constructing convex hull:");
	CGAL::convex_hull_3(points.begin(), points.end(), poly);

	std::cout << "The convex hull contains " << poly.size_of_vertices()
			<< " vertices" << std::endl;

	/* end time measurement */
	gettimeofday(&tvEnd, NULL);
	timeval_print(&tvEnd);

	/* calculate time difference */
	timeval_subtract(&tvDiff, &tvEnd, &tvBegin);
	printf("%ld.%06ld\n", tvDiff.tv_sec, tvDiff.tv_usec);

	/* begin time measurement */
	gettimeofday(&tvBegin, NULL);
	timeval_print(&tvBegin);

	if (0)
	{
		for (int i_point = 0;
				i_point < num_points - num_points / 
							FACTOR_OF_VERTICES_REDUCTION;
				++i_point)
		{
			srand((unsigned) time(0));
			int random_integer = rand();
			auto it_point = points.begin();
			for (int i_incr = 0; i_incr < random_integer; ++i_incr)
				++it_point;
			points.erase(it_point);
			CGAL::convex_hull_3(points.begin(), points.end(), poly);
		}
	}

	/* begin time measurement */
	gettimeofday(&tvEnd, NULL);
	timeval_print(&tvEnd);

	/* calculate time difference */
	timeval_subtract(&tvDiff, &tvEnd, &tvBegin);
	printf("%ld.%06ld\n", tvDiff.tv_sec, tvDiff.tv_usec);

	std::cout << "The convex hull contains " << poly.size_of_vertices()
			<< " vertices" << std::endl;

	/*
	 * assign a plane equation to each polyhedron facet using functor
	 * Plane_from_facet
	 */
	std::transform(poly.facets_begin(), poly.facets_end(), poly.planes_begin(),
			Plane_from_facet());
	DEBUG_END;
	return EXIT_SUCCESS;
}

