/*
 * Copyright (c) 2009-2022 Ilya Palachev <iliyapalachev@gmail.com>
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
 * @file probeBody.cpp
 * @brief Probe a given 3D body to get measurements of its support or gauge function.
 */

#include <iostream>
#include <sstream>
#include <random>

#include "Common.h"
#include "DataConstructors/SupportFunctionDataConstructor/SupportFunctionDataConstructor.h"
#include "PolyhedraCorrectionLibrary.h"
#include "Recoverer/SupportPolyhedronCorrector.h"
#include "SupportFunction.h"
#include "TractableFitter/AlternatingMinimization.h"

template <typename VectorT> std::vector<VectorT> generateDirections(int n)
{
	std::default_random_engine generator;
	std::normal_distribution<double> distribution(0., 1.);

	std::vector<VectorT> directions;
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

		VectorT v(x, y, z);
		directions.push_back(v);
	}

	return directions;
}

SupportFunctionDataPtr generateSupportData(std::vector<Vector3d> &directions, std::vector<Vector3d> &targetPoints,
										   double variance)
{

	std::default_random_engine generator;
	std::normal_distribution<double> noise(0., variance);
	std::vector<SupportFunctionDataItem> items;

	for (const auto &direction : directions)
	{
		double value = calculateSupportFunction(targetPoints, direction);
		ASSERT(value > 0.);
		double noisyValue = value + noise(generator);
		ASSERT(noisyValue > 0.);
		items.push_back(SupportFunctionDataItem(direction, noisyValue));
	}
	SupportFunctionDataPtr data(new SupportFunctionData(items));
	return data;
}


void printUsage(const std::string &executable)
{
	std::cerr << "Usage: " << executable << " INPUT_PATH MEASUREMENT_PARAMS" << std::endl;
    std::cerr << "  where INPUT_PATH is a path the file with body in PLY format" << std::endl;
	std::cerr << "  where MEASUREMENT_PARAMS looks like: "
				 "MEASUREMENT_MODE:param1=value1:...:paramN=valueN"
			  << std::endl;
}

std::vector<std::string> split(const std::string &string, char delimiter)
{
	std::vector<std::string> strings;
	std::istringstream stream(string);
	std::string current_string;
	while (getline(stream, current_string, delimiter))
	{
		strings.push_back(current_string);
	}
	return strings;
}

std::map<std::string, std::string> splitParams(const std::vector<std::string> &description,
											   const std::string &executable)
{
	std::map<std::string, std::string> params;
	for (const auto &word : description)
	{
		auto pair = split(word, '=');
		if (pair.size() != 2)
		{
			std::cerr << "Invalid parameter: " << word << std::endl;
			printUsage(executable);
			abort();
		}
		auto key = pair[0];
		auto value = pair[1];
		params[key] = value;
	}
	return params;
}

int main(int argc, char **argv)
{
	std::string executable = argv[0];
	if (argc != 3)
	{
		printUsage(executable);
		return EXIT_FAILURE;
	}

	auto measurementDescription = split(argv[2], ':');

	auto measurementParams = splitParams(measurementDescription, executable);

	// Parsing functions require some fake polyhedron as a container. It will not be used further, just a legacy.
	PolyhedronPtr p(new Polyhedron());
    std::string inputPath = argv[1];
	std::cout << "Scanning PLY file format..." << std::endl;
    p->fscan_ply(inputPath.c_str());
	std::cout << "Scanning PLY file format... done" << std::endl;

	if (p->numVertices == 0)
	{
		exit(EXIT_FAILURE);
	}
	SupportFunctionDataPtr data;

    auto num_measurements =
        measurementParams.count("num_measurements") ? std::stoi(measurementParams["num_measurements"]) : 100;
    auto directions = generateDirections<Vector3d>(num_measurements);

    double noise_variance =
        measurementParams.count("noise_variance") ? std::stod(measurementParams["noise_variance"]) : 0.01;

    std::vector<Vector3d> vertices(p->vertices, p->vertices + p->numVertices);
    data = generateSupportData(directions, vertices, noise_variance);

	std::cout << "Completed measurement data collection. Number of collected measurements: " << data->size() << std::endl;

    return EXIT_SUCCESS;
}
