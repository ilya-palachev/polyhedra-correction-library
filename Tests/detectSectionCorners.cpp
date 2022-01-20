/*
 * Copyright (c) 2009-2018 Ilya Palachev <iliyapalachev@gmail.com>
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
 * @file detectSectionCorners.cpp
 * @brief Detection of corners in the horizontal planar section of polyhedron,
 * for which we know only its shadow contours.
 */

#include <cstdlib>
#include "PolyhedraCorrectionLibrary.h"
#include "Recoverer/HallTurlachDetector.h"

static ItemsVector getItems(char *path, double z)
{
	/* Create fake empty polyhedron. */
	PolyhedronPtr p(new Polyhedron());

	/* Create shadow contour data and associate p with it. */
	ShadowContourDataPtr SCData(new ShadowContourData(p));

	/* Read shadow contours data from file. */
	SCData->fscanDefault(path);

	int numContoursIntersecting = 0;
	ItemsVector items;
	for (int iContour = 0; iContour < SCData->numContours; ++iContour)
	{
		SContour &contour = SCData->contours[iContour];
		int numIntersecting = 0;
		for (int iSide = 0; iSide < contour.ns; ++iSide)
		{
			SideOfContour &side = contour.sides[iSide];
			double diff1 = side.A1.z - z;
			double diff2 = side.A2.z - z;
			if (diff1 * diff2 >= 0.)
				continue;

			++numIntersecting;

			/* Find t, for which A^intersect = t * A1 + (1-t) * A2. */
			/* FIXME: Prevent zero division. */
			double t = (z - side.A2.z) / (side.A1.z - side.A2.z);

			/* Find z and y coordinates of A^intersect. */
			double x = t * side.A1.x + (1 - t) * side.A2.x;
			double y = t * side.A1.y + (1 - t) * side.A2.y;

			/* Support angle and support value. */
			double theta = atan2(y, x);
			double h = sqrt(x * x + y * y);
			items.push_back(std::make_pair(theta, h));
		}
		if (numIntersecting > 0)
			++numContoursIntersecting;
	}
	fprintf(stdout,
			"Number of contours intersecting the z value "
			"barrier: %d\n",
			numContoursIntersecting);

	if (numContoursIntersecting < SCData->numContours)
	{
		fprintf(stderr, "Error: Not all contours intersect plane {z = %lf}\n", z);
		exit(EXIT_FAILURE);
	}

	return items;
}

int main(int argc, char **argv)
{
	DEBUG_START;
	/* Parse command line. */
	if (argc != 8)
	{
		fprintf(stderr, "Usage: %s countours_file z m t l q s\n", argv[0]);
		return EXIT_FAILURE;
	}
	/* Read the file path to contours. */
	/* FIXME: Check the existance of the file. */
	char *path = argv[1];

	/* Read the "z" value, i.e. the coordinate of plane on Oz axis. */
	char *mistake = NULL;
	double zParameter = strtod(argv[2], &mistake);
	if (mistake && *mistake)
	{
		fprintf(stderr, "Error while reading z = %s\n", argv[2]);
		exit(EXIT_FAILURE);
	}

	/* Read the "m" value, i.e. p'' + p averaging parameter. */
	unsigned mParameter = atoi(argv[3]);

	/* Read the "t" value, i.e. the upper bound for small numbers. */
	double tParameter = strtod(argv[4], &mistake);
	if (mistake && *mistake)
	{
		fprintf(stderr, "Error while reading t = %s\n", argv[4]);
		exit(EXIT_FAILURE);
	}

	/* Read the "l" value, i.e. step for p' estimate calculation. */
	unsigned lParameter = atoi(argv[5]);

	/* Read the "q" value, i.e. the final estimate averaging parameter. */
	double qParameter = strtod(argv[6], &mistake);
	if (mistake && *mistake)
	{
		fprintf(stderr, "Error while reading q = %s\n", argv[6]);
		exit(EXIT_FAILURE);
	}

	/* Read the minimal size of cluster. */
	mistake = NULL;
	double sParameter = strtod(argv[7], &mistake);
	if (mistake && *mistake)
	{
		fprintf(stderr, "Error while reading s = %s\n", argv[7]);
		exit(EXIT_FAILURE);
	}

	auto items = getItems(path, zParameter);
	auto corners = estimateCorners(items, mParameter, tParameter, lParameter, qParameter, sParameter, false);
	fprintf(stdout, "%lu corners have been detected.\n", corners.size());
	DEBUG_END;
	return EXIT_SUCCESS;
}
