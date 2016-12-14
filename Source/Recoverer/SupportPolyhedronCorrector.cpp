/*
 * Copyright (c) 2009-2015 Ilya Palachev <iliyapalachev@gmail.com>
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
 * @file SupportPolyhedronCorrector.cpp
 * @brief Correction of polyhedron based on support function measurements
 * (implementation).
 */

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "PCLDumper.h"
#include "SupportPolyhedronCorrector.h"
#include "FixedTopologyNLP.h"

SupportPolyhedronCorrector::SupportPolyhedronCorrector(Polyhedron_3 initialP,
		SupportFunctionDataPtr SData) : initialP(initialP), SData(SData)
{
	DEBUG_START;
	DEBUG_END;
}

Polyhedron_3 obtainPolyhedron(FixedTopologyNLP *FTNLP)
{
	DEBUG_START;
	std::vector<Vector_3> directions = FTNLP->getDirections();
	std::vector<double> values = FTNLP->getValues();
	std::vector<Plane_3> planes(values.size());
	for (unsigned i = 0; i < values.size(); ++i)
		planes[i] = Plane_3(-directions[i].x(), -directions[i].y(),
				-directions[i].z(), values[i]);
	Polyhedron_3 intersection(planes);
	std::cout << "Intersection has " << intersection.size_of_vertices()
		<< " vertices, " << intersection.size_of_facets() << " facets."
		<< std::endl;

	std::vector<Vector_3> pointsAsVectors = FTNLP->getPoints();
	std::vector<Point_3> points(pointsAsVectors.size());
	for (const Vector_3 &v : pointsAsVectors)
		points.push_back(Point_3(v.x(), v.y(), v.z()));

	DEBUG_END;
	return intersection;
}

Polyhedron_3 SupportPolyhedronCorrector::run()
{
	DEBUG_START;
	IpoptApplication *app = IpoptApplicationFactory();

	/* Intialize the IpoptApplication and process the options */
	if (app->Initialize() != Solve_Succeeded)
	{
		MAIN_PRINT("*** Error during initialization!");
		return initialP;
	}

	app->Options()->SetStringValue("linear_solver", "ma57");
	if (getenv("DERIVATIVE_TEST_FIRST"))
		app->Options()->SetStringValue("derivative_test", "first-order");
	else if (getenv("DERIVATIVE_TEST_SECOND"))
		app->Options()->SetStringValue("derivative_test", "second-order");
	else if (getenv("DERIVATIVE_TEST_ONLY_SECOND"))
		app->Options()->SetStringValue("derivative_test", "only-second-order");
	if (getenv("HESSIAN_APPROX"))
		app->Options()->SetStringValue("hessian_approximation", "limited-memory");

	//app->Options()->SetStringValue("derivative_test_print_all", "yes");

	/* Prepare the NLP for solving. */
	auto u = SData->supportDirections<Vector_3>();
	VectorXd values = SData->supportValues();
	ASSERT(u.size() == unsigned(values.size()));
	std::vector<double> h(values.size());
	for (unsigned i = 0; i < values.size(); ++i)
		h[i] = values(i);
	std::vector<Vector_3> U;
	std::vector<double> H;
	for (auto I = initialP.facets_begin(), E = initialP.facets_end();
			I != E; ++I)
	{
		Plane_3 plane = I->plane();
		Vector_3 norm = plane.orthogonal_vector();
		double length = sqrt(norm.squared_length());
		norm = norm * (1. / length);
		double value = -plane.d() / length;
		// FIXME: Handle cases with small lengths.
		U.push_back(norm);
		H.push_back(value);
	}
	std::vector<Vector_3> points;
	for (auto I = initialP.vertices_begin(), E = initialP.vertices_end();
			I != E; ++I)
	{
		Point_3 point = I->point();
		points.push_back(Vector_3(point.x(), point.y(), point.z()));
	}

	FixedTopology *FT = new FixedTopology(initialP, SData);

	FixedTopologyNLP *FTNLP = new FixedTopologyNLP(u, h, U, H, points, FT);

	/* Ask Ipopt to solve the problem */
	if (app->OptimizeTNLP(FTNLP) != Solve_Succeeded)
	{
		MAIN_PRINT("** The problem FAILED!");
		DEBUG_END;
		return initialP;
	}

	app->ReOptimizeTNLP(FTNLP);

	MAIN_PRINT("*** The problem solved!");
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG, "INSIDE_FT_NLP-initial.ply") << initialP;
	Polyhedron_3 correctedP = obtainPolyhedron(FTNLP);
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG, "INSIDE_FT_NLP-from-planes.ply") << correctedP;

	delete FTNLP;
	DEBUG_END;
	return correctedP;
}
