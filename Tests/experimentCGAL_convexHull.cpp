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

#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/point_generators_3.h>
#include <CGAL/algorithm.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/convex_hull_3.h>
#include <vector>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Polyhedron_3<K> Polyhedron_3;
typedef K::Segment_3 Segment_3;

// define point creator
typedef K::Point_3 Point_3;
typedef CGAL::Creator_uniform_3<double, Point_3> PointCreator;

//a functor computing the plane containing a triangular facet
struct Plane_from_facet
{
	Polyhedron_3::Plane_3 operator()(Polyhedron_3::Facet& f)
	{
		Polyhedron_3::Halfedge_handle h = f.halfedge();
		return Polyhedron_3::Plane_3(h->vertex()->point(),
				h->next()->vertex()->point(), h->opposite()->vertex()->point());
	}
};

static void print_usage(int argc, char** argv)
{
	printf("Usage: %s <num_of_points>\n", argv[0]);
}

/* Return 1 if the difference is negative, otherwise 0.  */
int timeval_subtract(struct timeval *result, struct timeval *t2,
		struct timeval *t1)
{
	long int diff = (t2->tv_usec + 1000000 * t2->tv_sec)
			- (t1->tv_usec + 1000000 * t1->tv_sec);
	result->tv_sec = diff / 1000000;
	result->tv_usec = diff % 1000000;

	return (diff < 0);
}

void timeval_print(struct timeval *tv)
{
	char buffer[30];
	time_t curtime;

	printf("%ld.%06ld", tv->tv_sec, tv->tv_usec);
	curtime = tv->tv_sec;
	strftime(buffer, 30, "%m-%d-%Y  %T", localtime(&curtime));
	printf(" = %s.%06ld\n", buffer, tv->tv_usec);
}

int main(int argc, char** argv)
{
	struct timeval tvBegin, tvEnd, tvDiff;

	if (argc != 2)
	{
		print_usage(argc, argv);
		return EXIT_FAILURE;
	}

	int numPoints = 0;
	if (sscanf(argv[1], "%d", &numPoints) != 1)
	{
		print_usage(argc, argv);
		return EXIT_FAILURE;
	}

	CGAL::Random_points_in_sphere_3<Point_3, PointCreator> gen(100.0);

	// generate <numPoints> points randomly on a sphere of radius 100.0
	// and copy them to a vector
	std::vector<Point_3> points;
	CGAL::cpp11::copy_n(gen, numPoints, std::back_inserter(points));

	// define polyhedron to hold convex hull
	Polyhedron_3 poly;

	// compute convex hull of non-collinear points

	// begin
	gettimeofday(&tvBegin, NULL);
	timeval_print(&tvBegin);

	CGAL::convex_hull_3(points.begin(), points.end(), poly);

	//end
	gettimeofday(&tvEnd, NULL);
	timeval_print(&tvEnd);

	// diff
	timeval_subtract(&tvDiff, &tvEnd, &tvBegin);
	printf("%ld.%06ld\n", tvDiff.tv_sec, tvDiff.tv_usec);

	std::cout << "The convex hull contains " << poly.size_of_vertices()
			<< " vertices" << std::endl;

	// assign a plane equation to each polyhedron facet using functor Plane_from_facet
	std::transform(poly.facets_begin(), poly.facets_end(), poly.planes_begin(),
			Plane_from_facet());
	return EXIT_SUCCESS;
}

