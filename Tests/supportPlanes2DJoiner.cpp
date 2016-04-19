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
 * @file supportLines2DJoiner.cpp
 * @brief "Proof of concept" test for 2D support lines merging procedure.
 */
#include <algorithm>

#include <CGAL/Simple_cartesian.h>
typedef CGAL::Simple_cartesian<double> K;
typedef K::Point_2 Point_2;
typedef K::Line_2 Line_2;

double lineNorm(Line_2 l)
{
	return sqrt(l.a() * l.a() + l.b() * l.b());
}

Point_2 lineNormal(Line_2 l)
{
	return Point_2(l.a(), l.b());
}

#include <CGAL/Origin.h>
#include <CGAL/convex_hull_2.h>
#include <CGAL/basic.h>
#include <CGAL/Filtered_kernel.h>

#include "Common.h"

#define NUM_ARGUMENTS_DEFAULT 4

/**
 * Prints the usage information for the program.
 *
 * @param argv	Program arguments.
 */
void printUsage(const char *path)
{
	std::cerr << "Usage: " << path << " <number of support lines>"
		<< std::endl;
}

void errorExit(const char *path, const char *message)
{
	std::cerr << "Error: " << message << std::endl;
	printUsage(path);
	exit(EXIT_FAILURE);
}

unsigned long scanInteger(const char *path, const char *string)
{
	char *end;
	errno = 0;
	unsigned long value = strtoul(string, &end, 0);
	if (errno == ERANGE && value == ULONG_MAX)
	{
		perror("strtol");
		errorExit(path, "Invalid number.");
	}

	if (end == string)
	{
		errorExit(path, "Invalid number: no digits were found.");
	}

	return value;
}

#define MAX_POINT_COORDINATE 100.

std::vector<Point_2> generateExtremePoints(unsigned long numPoints)
{
	std::vector<Point_2> hull;
	while (hull.size() != numPoints)
	{
		unsigned long numPointsLack = numPoints - hull.size();
		if (numPointsLack == 0)
			continue;
		std::vector<Point_2> points;
		for (unsigned long i = 0; i < numPointsLack; ++i)
		{
			double x = genRandomDouble(MAX_POINT_COORDINATE);
			double y = genRandomDouble(MAX_POINT_COORDINATE);
			points.push_back(Point_2(x, y));
		}
		convex_hull_2(points.begin(), points.end(),
				std::back_inserter(hull));
	}
	return hull;
}

void sortPointsByAngle(std::vector<Point_2> &points)
{
	std::sort(points.begin(), points.end(), [](Point_2 a, Point_2 b)
	{
		return atan2(a.y(), a.x()) < atan2(b.y(), b.x());
	});
}

int main(int argc, char **argv)
{
	if (argc != NUM_ARGUMENTS_DEFAULT)
	{
		errorExit(argv[0], "Wrong number of arguments");
	}
	unsigned long numLines = scanInteger(argv[0], argv[1]);
	std::cout << "Number of support lines: " << numLines << std::endl;
	auto points = generateExtremePoints(numLines);
	sortPointsByAngle(points);

	unsigned long iLineMergeBegin = scanInteger(argv[0], argv[2]);
	unsigned long iLineMergeEnd = scanInteger(argv[0], argv[3]);
	if (iLineMergeBegin > iLineMergeEnd)
	{
		std::swap(iLineMergeBegin, iLineMergeEnd);
	}

	std::cout << "Merging lines from " << iLineMergeBegin << " and "
		<< iLineMergeEnd << std::endl;

	bool outOfRangeBegin = iLineMergeBegin < 0
		|| iLineMergeBegin >= numLines;
	bool outOfRangeEnd = iLineMergeEnd < 0
		|| iLineMergeEnd >= numLines;
	bool outOfRangeAny = outOfRangeBegin || outOfRangeEnd;
	if (outOfRangeAny)
		errorExit(argv[0], "Indices out of range");

	unsigned long numLinesMerged = iLineMergeEnd - iLineMergeBegin + 1;
	std::cout << "The number of lines to be merged: " << numLinesMerged
		<< std::endl;
	if (numLinesMerged > numLines - 2)
		errorExit(argv[0], "At least two lines should stay unmerged");

	unsigned long iLineLeft = (numLines + iLineMergeBegin - 1)
		% numLines;
	unsigned long iLineRight = (numLines + iLineMergeEnd + 1)
		% numLines;
	std::cout << "Left neighbor: " << iLineLeft << std::endl;
	std::cout << "Right neighbor: " << iLineRight << std::endl;

	std::vector<Line_2> lines;
	for (auto point: points)
	{
		double x = point.x();
		double y = point.y();
		double norm = sqrt(x * x + y * y);
		Line_2 line(x / norm, y / norm, -norm);
		lines.push_back(line);
	}

	Line_2 lineLeft = lines[iLineLeft];
	Line_2 lineRight = lines[iLineRight];
	std::cout << "Left line: " << lineLeft << std::endl;
	std::cout << "Right line: " << lineRight << std::endl;
	if (lineNormal(lineLeft) * lineNormal(lineRight) <= 0.)
		errorExit(argv[0], "Wrong position of neighbor planes");

	/*
	 * TODO:
	 *
	 * 1. Generate numLines support lines so that they are:
	 *  a. In general position (i. e. incosistent).
	 *  b. Consistent (this has higher priority).
	 * (depending on an option)
	 *
	 * 2. Sort them by polar angle.
	 *
	 * 3. Select begin and end lines counts (depending on two more options)
	 *
	 * 4. Try to join select group of lines.
	 */
	return EXIT_SUCCESS;
}
