/*
 * Copyright (c) 2017 Ilya Palachev <iliyapalachev@gmail.com>
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
 * @file correct.cpp
 * @brief Corrections of polyhedrons by the information about their shadow and
 * blink contours.
 */

#include <stdio.h>
#include <stdlib.h>
#include <coin/IpIpoptApplication.hpp>
#include "DebugPrint.h"
#include "DebugAssert.h"
#include "PCLDumper.h"
#include "Polyhedron/Polyhedron.h"
#include "Polyhedron_3/Polyhedron_3.h"
#include "DataContainers/EdgeInfo/EdgeInfo.h"
#include "Correctors/EdgeCorrector/EdgeCorrector.h"

static void printUsage(const char *name)
{
	DEBUG_START;
	printf("Usage: %s <polyhedron.dat> <edges.dat>\n", name);
	DEBUG_END;
}

int main(int argc, char **argv)
{
	DEBUG_START;
	if (argc < 3)
	{
		printUsage(argv[0]);
		DEBUG_END;
		return EXIT_FAILURE;
	}

	const char *pathPolyhedron = argv[1];
	const char *pathEdgesData = argv[2];
	PolyhedronPtr p(new Polyhedron());
	if (!p->fscan_default_1_2(pathPolyhedron))
	{
		printf("Failed to read polyhedron data!\n");
		printUsage(argv[0]);
		DEBUG_END;
		return EXIT_FAILURE;
	}
	std::cout << "Number of vertices (custom): " << p->numVertices
		<< std::endl;
	globalPCLDumper.setNameBase(pathPolyhedron);
	globalPCLDumper.enableVerboseMode();
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG, "init-polyhedron.ply")
		<< *(new Polyhedron(p));
	Polyhedron_3 initP(p);
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG, "init-polyhedron_3.ply")
		<< initP;

	std::cout << "Successfully read the polyhedron..." << std::endl;

	std::vector<EdgeInfo> data;
	if (!readEdgeInfoFile(pathEdgesData, data))
	{
		printf("Failed to read edge info data!\n");
		printUsage(argv[0]);
		DEBUG_END;
		return EXIT_FAILURE;
	}
	if (!getInitialPosition(initP, data))
	{
		printf("Failed to initialize edges!\n");
		DEBUG_END;
		return EXIT_FAILURE;
	}
	std::cout << "Successfully read the edge info data..." << std::endl;

	std::vector<Plane_3> planes;
	for (auto I = initP.planes_begin(), E = initP.planes_end(); I != E; ++I)
		planes.push_back(*I);

	EdgeCorrector *EC = new EdgeCorrector(planes, data);
	Ipopt::IpoptApplication *app = IpoptApplicationFactory();

	/* Intialize the IpoptApplication and process the options */
	if (app->Initialize() != Ipopt::Solve_Succeeded)
	{
		MAIN_PRINT("*** Error during initialization!");
		return EXIT_FAILURE;
	}

	//app->Options()->SetStringValue("linear_solver", "ma57");
	app->Options()->SetNumericValue("tol", 1e-3);
	app->Options()->SetNumericValue("acceptable_tol", 1e-3);
	if (getenv("DERIVATIVE_TEST_FIRST"))
		app->Options()->SetStringValue("derivative_test", "first-order");
	else if (getenv("DERIVATIVE_TEST_SECOND"))
	{
		app->Options()->SetStringValue("derivative_test", "second-order");
		//app->Options()->SetIntegerValue("derivative_test_first_index", 144);
	}
	else if (getenv("DERIVATIVE_TEST_ONLY_SECOND"))
		app->Options()->SetStringValue("derivative_test", "only-second-order");
	if (getenv("HESSIAN_APPROX"))
		app->Options()->SetStringValue("hessian_approximation", "limited-memory");

	/* Ask Ipopt to solve the problem */
	auto status = app->OptimizeTNLP(EC);
	if (status != Ipopt::Solve_Succeeded
		&& status != Ipopt::Solved_To_Acceptable_Level)
	{
		MAIN_PRINT("** The problem FAILED!");
		return EXIT_FAILURE;
	}

	MAIN_PRINT("*** The problem solved!");
	auto resultingPlanes = EC->getResultingPlanes();
	Polyhedron_3 resultingP(resultingPlanes);
	globalPCLDumper(PCL_DUMPER_LEVEL_OUTPUT, "result.ply")
		<< resultingP;

	DEBUG_END;
	return EXIT_SUCCESS;
}
