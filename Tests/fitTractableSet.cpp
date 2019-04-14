/*
 * Copyright (c) 2009-2019 Ilya Palachev <iliyapalachev@gmail.com>
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
 * @file fitTractableSet.cpp
 * @brief Fitting tractable convex sets to support funciton evaluations.
 */

#include <iostream>
#include <random>
#include "PolyhedraCorrectionLibrary.h"

std::vector<Vector3d> generateDirections(int n)
{
	std::default_random_engine generator;
	std::normal_distribution<double> distribution(0., 1.);

	std::vector<Vector3d> directions;
	for (int i = 0; i < n; ++i)
	{
		double x = 0., y = 0., z = 0., lambda = 0.;
		do
		{
			x = distribution(generator);
			y = distribution(generator);
			z = distribution(generator);
			lambda = sqrt(x * x + y * y + z * z);
		} while (lambda < 1e-16);

		x /= lambda;
		y /= lambda;
		z /= lambda;
		// Now (x, y, z) is uniformly distributed on the sphere
		
		Vector3d v(x, y, z);
		directions.push_back(v);
	}

	return directions;
}

std::vector<Vector3d> generateL1Ball()
{
	std::vector<Vector3d> l1ball;
	l1ball.push_back(Vector3d(1., 0., 0.));
	l1ball.push_back(Vector3d(0., 1., 0.));
	l1ball.push_back(Vector3d(0., 0., 1.));
	l1ball.push_back(Vector3d(-1., 0., 0.));
	l1ball.push_back(Vector3d(0., -1., 0.));
	l1ball.push_back(Vector3d(0., 0., -1.));
	return l1ball;
}

double calculateSupportFunction(const std::vector<Vector3d> &vertices,
			const Vector3d &direction)
{
	double result = vertices[0] * direction;
	for (const auto &vertex : vertices)
	{
		double product = vertex * direction;
		result = std::max(product, result);
	}
	return result;
}

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cerr << "Expected 1 argument" << std::endl;
		return EXIT_FAILURE;
	}

	int n = atoi(argv[1]);
	
	if (n <= 0)
	{
		std::cerr << "Expected positive number of measurements"
			<< std::endl;
		return EXIT_FAILURE;
	}

	auto directions = generateDirections(n);
	auto l1ball = generateL1Ball();

	std::default_random_engine generator;
	std::normal_distribution<double> noise(0., sqrt(0.1));
	for (const auto &direction : directions)
	{
		double value = calculateSupportFunction(l1ball, direction);
		std::cout << direction << ": " << value << " -> "
			<< value + noise(generator) << std::endl;
	}

	return EXIT_SUCCESS;
}
