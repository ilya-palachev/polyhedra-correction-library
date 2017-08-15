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

static Plane_3 normalizePlane(const Plane_3 &p)
{
	double a = p.a();
	double b = p.b();
	double c = p.c();
	double d = p.d();
	if (d > 0.)
	{
		a = -a;
		b = -b;
		c = -c;
		d = -d;
	}
	double l = sqrt(a * a + b * b + c * c);
	ASSERT(l > 1e-15);
	a /= l;
	b /= l;
	c /= l;
	d /= l;
	return Plane_3(a, b, c, d);
}

std::vector<Plane_3> correctPlanes(const std::vector<Plane_3> &planes,
		const std::vector<EdgeInfo> &data)
{
	EdgeCorrector *EC = new EdgeCorrector(planes, data);
	Ipopt::IpoptApplication *app = IpoptApplicationFactory();

	/* Intialize the IpoptApplication and process the options */
	if (app->Initialize() != Ipopt::Solve_Succeeded)
	{
		MAIN_PRINT("*** Error during initialization!");
		exit(EXIT_FAILURE);
	}

	app->Options()->SetStringValue("linear_solver", "ma57");
	app->Options()->SetNumericValue("tol", 1e-3);
	app->Options()->SetNumericValue("acceptable_tol", 1e-3);
	if (getenv("DERIVATIVE_TEST_FIRST"))
		app->Options()->SetStringValue("derivative_test", "first-order");
	else if (getenv("DERIVATIVE_TEST_SECOND"))
	{
		app->Options()->SetStringValue("derivative_test", "second-order");
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
		exit(EXIT_FAILURE);
	}

	MAIN_PRINT("*** The problem solved!");
	return EC->getResultingPlanes();
}

static double planeDist(const Plane_3 &p0, const Plane_3 &p1)
{
	double a = p0.a() - p1.a();
	double b = p0.b() - p1.b();
	double c = p0.c() - p1.c();
	double d = p0.d() - p1.d();
	return sqrt(a * a + b * b + c * c + d * d);
}

static Point_3 getCenter(const Polyhedron_3 &p)
{
	Point_3 C(0., 0., 0.);
	for (auto I = p.vertices_begin(), E = p.vertices_end(); I != E; ++I)
		C = C + (I->point() - CGAL::Origin());
	C = C * (1. / p.size_of_vertices());

	return C;
}

static std::vector<Plane_3> centerizePlanes(const std::vector<Plane_3> &planes,
		const Point_3 &C)
{
	std::vector<Plane_3> centeredPlanes;
	for (const Plane_3 &p : planes)
		centeredPlanes.push_back(Plane_3(p.a(), p.b(), p.c(),
			p.d() + p.a() * C.x() + p.b() * C.y()
			+ p.c() * C.z()));

	for (Plane_3 &p : centeredPlanes)
	{
		double a = p.a();
		double b = p.b();
		double c = p.c();
		double d = p.d();
		if (a * C.x() + b * C.y() + c * C.z() + d < 0.)
		{
			a = -a;
			b = -b;
			c = -c;
			d = -d;
		}

		p = Plane_3(a, b, c, d);
	}

	return centeredPlanes;
}

static void dumpResult(const std::vector<Plane_3> &planes,
		const std::vector<Plane_3> &resultingPlanes, const Point_3 &C)
{
	std::cout << "Number of initial planes: " << planes.size() << std::endl;
	std::cout << "Number of resulting planes: " << resultingPlanes.size()
		<< std::endl;
	std::cout << "Center: " << C;
	ASSERT(planes.size() == resultingPlanes.size());

	double maxDistance = 0.;
	for (unsigned i = 0; i < planes.size(); ++i)
	{
		Plane_3 p0 = planes[i];
		Plane_3 p = resultingPlanes[i];
		double distance = planeDist(p0, p);
		if (distance > 1e-6)
			std::cout << "Plane #" << i << ": " << p0
				<< " -> " << p << " # distance: " << distance
				<< std::endl;
		maxDistance = std::max(distance, maxDistance);
	}

	std::cout << "Maximal distance: " << maxDistance << std::endl;

	/* To compare with the result: */
	Polyhedron_3 initCenterizedP(centerizePlanes(planes, C));
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG, "init-polyhedron-centerized.ply")
		<< initCenterizedP;

	/*
	 * FIXME: translate to proper positions and make associations with
	 * initial facet IDs.
	 */
	Polyhedron_3 resultingP(centerizePlanes(resultingPlanes, C));
	globalPCLDumper(PCL_DUMPER_LEVEL_OUTPUT, "result.ply")
		<< resultingP;
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

	Polyhedron_3 initP(p);
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG, "init-polyhedron.ply")
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
		planes.push_back(normalizePlane(*I));

	std::vector<Plane_3> resultingPlanes = correctPlanes(planes, data);

	dumpResult(planes, resultingPlanes, getCenter(initP));

	DEBUG_END;
	return EXIT_SUCCESS;
}
