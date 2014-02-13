/*
 * Copyright (c) 2009-2014 Ilya Palachev <iliyapalachev@gmail.com>
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
 * @file ShadowContourCluterizer_test.cpp
 * @brief Unit test for shadow contour clusterizer.
 */

#include <CGAL/Plane_3.h>

#include "PolyhedraCorrectionLibrary.h"

#define NUM_ARGS_EXPECTED 3

void printUsage(void)
{
	DEBUG_START;
	printf("Usage:\n");
	printf("./ShadowContourCluterizer_test <input file> <output file>\n");
	DEBUG_END;
}

int main(int argc, char** argv)
{
	DEBUG_START;
	if (argc != NUM_ARGS_EXPECTED)
	{
		ERROR_PRINT("Wrong arguments provided to program.");
		printUsage();
		DEBUG_END;
		return EXIT_FAILURE;
	}
	
	/* TODO: Here should be a good command-line parser function. */

	PolyhedronPtr p(new Polyhedron());
	ShadeContourDataPtr SCData(new ShadeContourData(p));
	SCData->fscanDefault(argv[1]);
	ShadowContourClusterizerPtr clusterizer(new ShadowContourClusterizer(p));
	clusterizer->buildPlot(SCData, argv[2]);
	
	DEBUG_END;
	return EXIT_SUCCESS;
}