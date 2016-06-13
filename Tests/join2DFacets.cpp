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
 * @file join2DFacets.cpp
 * @brief Prototype for best fitting polygon in support funciton metrics.
 */
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <cstdio>

#include <CGAL/Cartesian.h>
typedef CGAL::Cartesian<double> K;
typedef CGAL::Point_2<K> Point_2;
typedef CGAL::Line_2<K> Line_2;
typedef CGAL::Vector_2<K> Vector_2;
#include <CGAL/convex_hull_2.h>
#include <CGAL/intersections.h>

#include "Constants.h"
#include "DebugPrint.h"
#include "DebugAssert.h"
#include "PCLDumper.h"

/**
 * Generates random number d such that |d| <= maxDelta
 *
 * @param maxDelta	maximum absolute limit of generated number
 */
static double genRandomDouble(double maxDelta)
{
	DEBUG_START;
	//srand((unsigned) time(0));
	struct timeval t1;
	gettimeofday(&t1, NULL);
	srand(t1.tv_usec * t1.tv_sec);
	
	int randomInteger = rand();
	double randomDouble = randomInteger;
	const double halfRandMax = RAND_MAX * 0.5;
	randomDouble -= halfRandMax;
	randomDouble /= halfRandMax;

	randomDouble *= maxDelta;

	DEBUG_END;
	return randomDouble;
}

#define MAX_DELTA 10.

std::vector<Point_2> generateExtremePoints(int numPoints)
{
	std::vector<Point_2> points;
#if 0
	for (int i = 0; i < numPoints; ++i)
	{
		do
		{
			Point_2 point(genRandomDouble(MAX_DELTA),
					genRandomDouble(MAX_DELTA));
			points.push_back(point);
			std::vector<Point_2> hull;
			convex_hull_2(points.begin(), points.end(),
					std::back_inserter(hull));
			if (hull.size() == points.size())
				break;
			points.pop_back();
		}
		while (1);
	}
#endif
	double maxDelta = 2. / ((double) numPoints);
	for (int i = 0; i < numPoints; ++i)
	{
		double angle = 2. * M_PI * ((double) i) / ((double) numPoints);
		double xShift = genRandomDouble(maxDelta);
		double yShift = genRandomDouble(maxDelta);
		Point_2 point(cos(angle) + xShift, sin(angle) + yShift);
		points.push_back(point);
	}
	std::vector<Point_2> hull;
	convex_hull_2(points.begin(), points.end(), std::back_inserter(hull));
	assert(hull.size() == numPoints);
	std::cout << "Number of points in the hull: " << hull.size() << std::endl;
	return points;
}

void centerizeExtremePoints(std::vector<Point_2> &points)
{
	double centerX = 0., centerY = 0.;
	for (auto point: points)
	{
		centerX += point.x();
		centerY += point.y();
	}
	centerX /= points.size();
	centerY /= points.size();
	Vector_2 center(centerX, centerY);
	for (auto &point: points)
	{
		point = point - center;
	}
}

std::pair<std::vector<Vector_2>, std::vector<double>>
prepareSupportData(std::vector<Point_2> points)
{
	std::vector<Vector_2> directions;
	std::vector<double> values;
	int numPoints = points.size();
	for (int i = 0; i < numPoints; ++i)
	{
		int iNext = (i + 1) % numPoints;
		Line_2 line(points[i], points[iNext]);
		double a = line.a();
		double b = line.b();
		double c = line.c();
		if (a * points[i].x() + b * points[i].y() + c < 0)
		{
			a = -a;
			b = -b;
			c = -c;
		}
		Vector_2 normal(line.a(), line.b());
		double length = sqrt(normal.squared_length());
		Vector_2 direction(normal.x() / length, normal.y() / length);
		directions.push_back(direction);
		double value = -line.c() / length;
		values.push_back(value);
	}
	return std::make_pair(directions, values);
}

int main(int argc, char **argv)
{
	if (argc != 2)
		return EXIT_FAILURE;
	int numPoints = atoi(argv[1]);
	std::cout << "Convex hull of " << numPoints << " will be generated"
		<< std::endl;
	if (numPoints <= 0)
		return EXIT_FAILURE;

	auto points = generateExtremePoints(numPoints);
	centerizeExtremePoints(points);

	std::sort(points.begin(), points.end(),
		[](const Point_2 &a, const Point_2 &b)
		{
			return atan2(a.y(), a.x()) > atan2(b.y(), b.x());
		});
	std::vector<Vector_2> directions;
	std::vector<double> values;
	std::tie(directions, values) = prepareSupportData(points);
	return EXIT_SUCCESS;
}
