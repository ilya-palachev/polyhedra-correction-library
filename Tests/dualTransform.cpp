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
 * @file dualTransform.cpp
 * @brief Read polyhedron, build a dual polyhedron, and write it to file
 */

#include <fstream>
#include <iostream>
#include <sstream>

#include "Common.h"
#include "PolyhedraCorrectionLibrary.h"


void printUsage(const std::string &executable)
{
	std::cerr << "Usage: " << executable << " INPUT_PATH OUTPUT_PATH" << std::endl;
	std::cerr << "  where INPUT_PATH is a path to the file with body in PLY format" << std::endl;
	std::cerr << "  where OUTPUT_PATH is a path to the file where dual body will be stored" << std::endl;
}

int main(int argc, char **argv)
{
	std::string executable = argv[0];
	if (argc != 3)
	{
		printUsage(executable);
		return EXIT_FAILURE;
	}

	// Parsing functions require some fake polyhedron as a container. It will not be used further, just a legacy.
	PolyhedronPtr p(new Polyhedron());
	std::string inputPath = argv[1];
	std::cout << "Scanning PLY file format..." << std::endl;
	p->fscan_ply(inputPath.c_str());
	std::cout << "Scanning PLY file format... done" << std::endl;

	if (p->numVertices == 0)
	{
		std::cerr << "Failed to parse the file " << inputPath << "; the parsed body has 0 vertices." << std::endl;
		exit(EXIT_FAILURE);
	}

    Polyhedron_3 dualP = Polyhedron_3::dual(*p);

	std::ofstream outputFile;
	outputFile.open(argv[2]);
    outputFile << dualP;
	outputFile.close();
	return EXIT_SUCCESS;
}
