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

#include "Common.h"
#include "Correctors/EdgeCorrector/EdgeCorrector.h"
#include "DataContainers/EdgeInfo/EdgeInfo.h"
#include "DebugAssert.h"
#include "DebugPrint.h"
#include "PCLDumper.h"
#include "Polyhedron/Facet/Facet.h"
#include "Polyhedron/Polyhedron.h"
#include "Polyhedron_3/Polyhedron_3.h"
#include <coin/IpIpoptApplication.hpp>
#include <stdio.h>
#include <stdlib.h>

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

void setParameters(Ipopt::SmartPtr<Ipopt::OptionsList> options)
{
	const char *iptoptLinearSolver = getenv("IPOPT_LINEAR_SOLVER");
	if (!iptoptLinearSolver)
		iptoptLinearSolver = "ma57";
	options->SetStringValue("linear_solver", iptoptLinearSolver);
	if (!strcmp(iptoptLinearSolver, "ma27"))
	{
		/* This helps to improve performance from 0.6 to 0.5 sec */
		options->SetNumericValue("ma27_liw_init_factor", 100.);
		options->SetNumericValue("ma27_la_init_factor", 100.);
	}
	else if (!strcmp(iptoptLinearSolver, "ma57"))
	{
		/*
		 * Decreases performance from 0.04 to 0.13 sec:
		 * options->SetStringValue("ma57_automatic_scaling", "yes");
		 */

		/*
		 * Not evident, whether this helps to increase the performance,
		 * but it already helped to avoid memory reallocations.
		 */
		options->SetNumericValue("ma57_pre_alloc", 100.);
	}

	double ipoptTol = 0.;
	if (tryGetenvDouble("IPOPT_TOL", ipoptTol))
		options->SetNumericValue("tol", ipoptTol);

	double ipoptAcceptableTol = 0.;
	if (tryGetenvDouble("IPOPT_ACCEPTABLE_TOL", ipoptAcceptableTol))
		options->SetNumericValue("acceptable_tol", 1e-3);

	if (getenv("DERIVATIVE_TEST_FIRST"))
		options->SetStringValue("derivative_test", "first-order");
	else if (getenv("DERIVATIVE_TEST_SECOND"))
		options->SetStringValue("derivative_test", "second-order");
	else if (getenv("DERIVATIVE_TEST_ONLY_SECOND"))
		options->SetStringValue("derivative_test", "only-second-order");
	if (getenv("HESSIAN_APPROX"))
		options->SetStringValue("hessian_approximation", "limited-memory");
}

std::vector<Plane_3> correctPlanes(const std::vector<Plane_3> &planes,
								   const std::vector<EdgeInfo> &data)
{
	bool doEdgeScaling = (getenv("DISABLE_EDGE_SCALING") == nullptr);
	EdgeCorrector *EC = new EdgeCorrector(doEdgeScaling, planes, data);
	Ipopt::IpoptApplication *app = IpoptApplicationFactory();

	/* Intialize the IpoptApplication and process the options */
	if (app->Initialize() != Ipopt::Solve_Succeeded)
	{
		MAIN_PRINT("*** Error during initialization!");
		exit(EXIT_FAILURE);
	}

	setParameters(app->Options());
	/* Ask Ipopt to solve the problem */
	auto status = app->OptimizeTNLP(EC);
	if (status != Ipopt::Solve_Succeeded &&
		status != Ipopt::Solved_To_Acceptable_Level)
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
	Vector_3 C(0., 0., 0.);
	auto O = CGAL::Origin();
	for (auto I = p.vertices_begin(), E = p.vertices_end(); I != E; ++I)
		C = C + (I->point() - O);
	C = C * (1. / p.size_of_vertices());
	return O + C;
}

static double signedDist(const Plane_3 &p, const Point_3 &C)
{
	return p.a() * C.x() + p.b() * C.y() + p.c() * C.z() + p.d();
}

static Plane_3 centerizePlane(const Plane_3 &p, const Point_3 &C, double sign)
{
	Plane_3 pp(p.a(), p.b(), p.c(),
			   p.d() + p.a() * C.x() + p.b() * C.y() + p.c() * C.z());

	double a = pp.a();
	double b = pp.b();
	double c = pp.c();
	double d = pp.d();
	if (sign * signedDist(pp, C) < 0.)
	{
		a = -a;
		b = -b;
		c = -c;
		d = -d;
	}

	pp = Plane_3(a, b, c, d);

	return pp;
}

static std::vector<Plane_3> centerizePlanes(const std::vector<Plane_3> &planes,
											const Point_3 &C)
{
	std::vector<Plane_3> centeredPlanes;
	for (const Plane_3 &p : planes)
		centeredPlanes.push_back(centerizePlane(p, C, 1.));
	return centeredPlanes;
}

std::ostream &operator<<(std::ostream &stream, std::shared_ptr<Polyhedron> p)
{
	stream << *p;
	return stream;
}

/* FIXME: Copied from Polyhedron_io.cpp with slight modifications. */
static std::shared_ptr<Polyhedron>
convertWithAssociation(Polyhedron_3 p, const Point_3 &C,
					   const std::vector<Plane_3> &initPlanes)
{
	/* Check for non-emptiness. */
	ASSERT(p.size_of_vertices());
	ASSERT(p.size_of_facets());

	int numVertices = p.size_of_vertices();
	int numFacets = p.size_of_facets();

	/* Allocate memory for arrays. */
	Vector3d *vertices = new Vector3d[numVertices];
	Facet *facets = new Facet[numFacets];

	/* Transform vertexes. */
	int iVertex = 0;
	for (auto vertex = p.vertices_begin(); vertex != p.vertices_end(); ++vertex)
	{
		auto O = CGAL::Origin();
		Point_3 point = C + (vertex->point() - O);
		vertices[iVertex++] = Vector3d(point.x(), point.y(), point.z());
	}

	/*
	 * Transform facets.
	 * This algorithm is based on example kindly provided at CGAL online user
	 * manual. See example Polyhedron/polyhedron_prog_off.cpp
	 */
	int iFacet = 0;
	auto plane = p.planes_begin();
	auto facet = p.facets_begin();
	/* Iterate through the std::lists of planes and facets. */
	do
	{
		int id = p.indexPlanes_[iFacet];

		facets[id].id = id;

		/* Transform current plane. */
		Plane_3 pi = centerizePlane(*plane, Point_3(-C.x(), -C.y(), -C.z()),
									signedDist(initPlanes[id], C));
		facets[id].plane = Plane(Vector3d(pi.a(), pi.b(), pi.c()), pi.d());

		/*
		 * Iterate through the std::list of halfedges incident to the curent
		 * CGAL facet.
		 */
		auto halfedge = facet->facet_begin();

		/* Facets in polyhedral surfaces are at least triangles. 	*/
		CGAL_assertion(CGAL::circulator_size(halfedge) >= 3);

		facets[id].numVertices = CGAL::circulator_size(halfedge);
		facets[id].indVertices = new int[3 * facets[id].numVertices + 1];
		/*
		 * TODO: It's too unsafe architecture if we do such things as setting
		 * the size of internal array outside the API functions. Moreover, it
		 * can cause us to write memory leaks.
		 * indFacets and numFacets should no be public members.
		 */

		int iFacetVertex = 0;
		do
		{
			facets[id].indVertices[iFacetVertex++] =
				std::distance(p.vertices_begin(), halfedge->vertex());
		} while (++halfedge != facet->facet_begin());

		/* Add cycling vertex to avoid assertion during printing. */
		facets[id].indVertices[facets[id].numVertices] =
			facets[id].indVertices[0];

		ASSERT(facets[id].correctPlane());

		/* Increment the ID of facet. */
		++iFacet;

	} while (++plane != p.planes_end() && ++facet != p.facets_end());

	return std::make_shared<Polyhedron>(numVertices, numFacets, vertices,
										facets);
}
static void dumpResult(const std::vector<Plane_3> &planes,
					   const std::vector<Plane_3> &resultingPlanes,
					   const Point_3 &C)
{
	std::cout << "Number of initial planes: " << planes.size() << std::endl;
	std::cout << "Number of resulting planes: " << resultingPlanes.size()
			  << std::endl;
	std::cout << "Center: " << C << std::endl;
	ASSERT(planes.size() == resultingPlanes.size());

	double maxDistance = 0.;
	for (unsigned i = 0; i < planes.size(); ++i)
	{
		Plane_3 p0 = planes[i];
		Plane_3 p = resultingPlanes[i];
		double distance = planeDist(p0, p);
		if (distance > 1e-6)
			std::cout << "Plane #" << i << ": " << p0 << " -> " << p
					  << " # distance: " << distance << std::endl;
		maxDistance = std::max(distance, maxDistance);
	}

	std::cout << "Maximal distance: " << maxDistance << std::endl;

	/* To compare with the result: */
	Polyhedron_3 initCenterizedP(centerizePlanes(planes, C));
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG, "init-polyhedron-centerized.ply")
		<< initCenterizedP;

	auto centerizedPlanes = centerizePlanes(resultingPlanes, C);
	Polyhedron_3 resultingP(centerizedPlanes);
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG, "result-raw.ply") << resultingP;
	resultingP.initialize_indices(centerizedPlanes);

	auto result = convertWithAssociation(resultingP, C, planes);
	globalPCLDumper(PCL_DUMPER_LEVEL_OUTPUT, "result.ply") << result;

	result->fprint_default_1_2("result.dat");
	std::cout << "=========================================\n";
	std::cout << "|| The result is printed to result.dat ||\n";
	std::cout << "=========================================\n";
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
	std::cout << "Number of vertices (custom): " << p->numVertices << std::endl;
	globalPCLDumper.setNameBase(pathPolyhedron);
	globalPCLDumper.enableVerboseMode();

	Polyhedron_3 initP(p);
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG, "init-polyhedron.ply") << initP;

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
