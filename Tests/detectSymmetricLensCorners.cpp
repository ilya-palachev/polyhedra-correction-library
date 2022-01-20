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
 * @file detectSymmetricLensCorners.cpp
 * @brief Detection of corners in the horizontal lens, i.e. intersection of two
 * 2D disks:
 *   (x - 2)^2 + y^2 <= 9
 * and
 *   (x + 2)^2 + y^2 <= 9.
 * This is an example for original Halls and Turlach paper.
 */

#include <cstdlib>
#include <random>
#include "PolyhedraCorrectionLibrary.h"
#include "Recoverer/HallTurlachDetector.h"

static ItemsVector generateSymmetricLensItems(unsigned N, double noiseSigma)
{
	ItemsVector items;
	std::default_random_engine generator;
	std::normal_distribution<double> distribution(1.0, noiseSigma * noiseSigma);
	for (unsigned i = 0; i < N; ++i)
	{
		double theta = (2. * M_PI * (double(i) + 0.5)) / N - M_PI;
		double h = 0.;
		double arctg = atan2(sqrt(5), 2);
		double theta1entry = M_PI * 0.5 - arctg;
		double theta1exit = M_PI - theta1entry;
		double theta2entry = -theta1exit;
		double theta2exit = -theta1entry;
		if (theta1entry <= theta && theta <= theta1exit)
			h = sqrt(5) * sin(theta);
		else if (theta2entry <= theta && theta <= theta2exit)
			h = -sqrt(5) * sin(theta);
		else if (theta2exit <= theta && theta <= theta1entry)
			h = 3. - 2. * cos(theta);
		else
			h = 3. + 2. * cos(theta);
		h *= distribution(generator);
		items.push_back(std::make_pair(theta, h));
	}
	return items;
}

int main(int argc, char **argv)
{
	DEBUG_START;
	/* Parse command line. */
	if (argc != 8)
	{
		fprintf(stderr, "Usage: %s N m t l q noise_sigma s\n", argv[0]);
		return EXIT_FAILURE;
	}
	/* Read the "N" value, i.e. number of thetas. */
	unsigned NParameter = atoi(argv[1]);

	/* Read the "m" value, i.e. p'' + p averaging parameter. */
	unsigned mParameter = atoi(argv[2]);

	/* Read the "t" value, i.e. the upper bound for small numbers. */
	char *mistake = NULL;
	double tParameter = strtod(argv[3], &mistake);
	if (mistake && *mistake)
	{
		fprintf(stderr, "Error while reading t = %s\n", argv[3]);
		exit(EXIT_FAILURE);
	}

	/* Read the "l" value, i.e. step for p' estimate calculation. */
	unsigned lParameter = atoi(argv[4]);

	/* Read the "q" value, i.e. the final estimate averaging parameter. */
	mistake = NULL;
	double qParameter = strtod(argv[5], &mistake);
	if (mistake && *mistake)
	{
		fprintf(stderr, "Error while reading q = %s\n", argv[5]);
		exit(EXIT_FAILURE);
	}

	/* Read the noise sigma. */
	mistake = NULL;
	double noiseSigmaParameter = strtod(argv[6], &mistake);
	if (mistake && *mistake)
	{
		fprintf(stderr, "Error while reading noise_sigma = %s\n", argv[6]);
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

	auto items = generateSymmetricLensItems(NParameter, noiseSigmaParameter);
	auto corners = estimateCorners(items, mParameter, tParameter, lParameter, qParameter, sParameter, false);
	fprintf(stdout, "%lu corners have been detected.\n", corners.size());
	DEBUG_END;
	return EXIT_SUCCESS;
}
