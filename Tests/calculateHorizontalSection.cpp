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
 * @file calculateHorizontalSection.cpp
 * @brief Calculates corners of horizontal section between polyhedron and plane.
 */

#include "PolyhedraCorrectionLibrary.h"
#include <cstdlib>

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		fprintf(stderr, "Usage: %s <polyhedron> <z>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* Read the file path to polyhedron data (3rd-party, already recovered). */
	/* FIXME: Check the existance of the file. */
	char *pathPolyhedron = argv[1];

	/* Read the "z" value, i.e. the coordinate of plane on Oz axis. */
	char *mistake = NULL;
	double zValue = strtod(argv[2], &mistake);
	if (mistake && *mistake)
	{
		fprintf(stderr, "Error while reading z = %s\n", argv[2]);
		exit(EXIT_FAILURE);
	}

	PolyhedronPtr p(new Polyhedron());
	p->fscan_default_1_2(pathPolyhedron);
	Polyhedron_3 polyhedron(p);

	unsigned id = 0;
	for (auto I = polyhedron.halfedges_begin(), E = polyhedron.halfedges_end();
		 I != E; ++I)
	{
		I->id = id++;
	}

	std::set<unsigned> visited;
	std::vector<std::tuple<double, double, double>> corners;
	for (auto I = polyhedron.halfedges_begin(), E = polyhedron.halfedges_end();
		 I != E; ++I)
	{
		if (visited.find(I->id) != visited.end())
			continue;

		visited.insert(I->id);
		visited.insert(I->opposite()->id);
		auto A = I->vertex()->point();
		auto B = I->opposite()->vertex()->point();
		if ((A.z() - zValue) * (B.z() - zValue) >= 0.)
			continue;

		/* FIXME: Get rid of copy-paste. */
		double t = (zValue - B.z()) / (A.z() - B.z());

		/* Find z and y coordinates of A^intersect. */
		double x = t * A.x() + (1 - t) * B.x();
		double y = t * A.y() + (1 - t) * B.y();

		/* Support angle and support value. */
		double theta = atan2(y, x);
		corners.push_back(std::make_tuple(theta, x, y));
	}
	fprintf(stdout, "Number of corners in 3rd party model: %lu\n",
			corners.size());
	std::sort(corners.begin(), corners.end());

	for (const auto &corner : corners)
	{
		double theta = std::get<0>(corner);
		double x = std::get<1>(corner);
		double y = std::get<2>(corner);
		fprintf(stdout, "    x = %lf, y = %lf\n", x, y);
		fprintf(stdout, "    theta = %lf\n", theta);
	}
}
