/*
 * Copyright (c) 2009-2016 Ilya Palachev <iliyapalachev@gmail.com>
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
 * @file EdgeCorrector.cpp
 * @brief Correction of polyhedron based on support function measurements and
 * edge heuristics (implementation).
 */

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "PCLDumper.h"
#include "Common.h"
#include "EdgeCorrector.h"
#include "Colouring.h"
#include "DataConstructors/SupportFunctionDataConstructor/SupportFunctionDataConstructor.h"
#include "IpoptTopologicalCorrector.h"

#define UNINITIALIZED_MAP_VALUE -1

EdgeCorrector::EdgeCorrector(Polyhedron_3 initialP, SupportFunctionDataPtr SData) : initialP(initialP), SData(SData)
{
	DEBUG_START;
	DEBUG_END;
}

static std::vector<SimpleEdge_3> getEdges(Polyhedron_3 P, std::map<int, int> &map)
{
	DEBUG_START;
	std::vector<bool> visited(P.size_of_halfedges());
	for (unsigned i = 0; i < visited.size(); ++i)
		visited[i] = false;
	std::cout << "getEdges: number of halfedges: " << P.size_of_halfedges() << std::endl;

	std::vector<SimpleEdge_3> edges;
	P.initialize_indices();
	CGAL::Origin origin;

	for (auto &i : map)
		i.second = UNINITIALIZED_MAP_VALUE;

	for (auto I = P.halfedges_begin(), E = P.halfedges_end(); I != E; ++I)
	{
		if (visited[I->id])
			continue;
		SimpleEdge_3 edge;
		edge.A = I->vertex()->point() - origin;
		edge.B = I->opposite()->vertex()->point() - origin;
		edge.iForward = I->facet()->id;
		edge.iBackward = I->opposite()->facet()->id;
		// FIXME: Remember indices of planes, not planes themselves,
		// and fix structure for this.
		map[I->id] = map[I->opposite()->id] = edges.size();
		edges.push_back(edge);
		visited[I->id] = visited[I->opposite()->id] = true;
	}
	std::cout << "getEdges: number of edges: " << edges.size() << std::endl;
	ASSERT(edges.size() * 2 == P.size_of_halfedges());

	DEBUG_END;
	return edges;
}

void associateEdges(std::vector<SimpleEdge_3> &edges, SupportFunctionDataPtr data)
{
	DEBUG_START;
	std::vector<Vector_3> directions = data->supportDirections<Vector_3>();
	std::vector<Plane_3> planes = data->supportPlanes();
	VectorXd values = data->supportValues();
	unsigned numItems = directions.size();

	for (unsigned i = 0; i < numItems; ++i)
	{
		Vector_3 u = directions[i];

		double productMax = 0.;
		double productMaxVice = 0.;
		unsigned jBest = edges.size();

		for (unsigned j = 0; j < edges.size(); ++j)
		{
			Vector_3 A = edges[j].A;
			Vector_3 B = edges[j].B;
			double product = std::max(A * u, B * u);
			double productVice = std::min(A * u, B * u);
			if (product >= productMax || (product == productMax && productVice >= productMaxVice))
			{
				productMax = product;
				productMaxVice = productVice;
				jBest = j;
			}
		}

		ASSERT(jBest != edges.size() && "Failed to find best edge");
		edges[jBest].tangients.push_back(planes[i]);
	}

	unsigned numTangientsMin = numItems;
	unsigned numTangientsMax = 0;
	unsigned numNonAssociatedEdges = 0;
	for (unsigned i = 0; i < edges.size(); ++i)
	{
		unsigned size = edges[i].tangients.size();
		if (size < numTangientsMin)
			numTangientsMin = size;
		if (size > numTangientsMax)
			numTangientsMax = size;
		if (size == 0)
			++numNonAssociatedEdges;
	}
	std::cout << "Minimal number of tangients: " << numTangientsMin << std::endl;
	std::cout << "Maximal number of tangients: " << numTangientsMax << std::endl;
	std::cout << "Number of non-associated edges: " << numNonAssociatedEdges << std::endl;

	DEBUG_END;
}

static void printAssociatedEdges(Polyhedron_3 polyhedron, std::vector<SimpleEdge_3> &edges,
								 std::map<int, int> &halfedgesMap)
{
	Colour red;
	red.red = 255;
	red.green = 0;
	red.blue = 0;

	unsigned numColouredHalfedges = 0;
	for (auto &i : halfedgesMap)
	{
		if (!edges[i.second].tangients.empty())
		{
			polyhedron.halfedgeColours[i.first] = red;
			++numColouredHalfedges;
		}
	}
	std::cout << "The number of coloured halfedges: " << numColouredHalfedges << std::endl;

	polyhedron.whetherPrintEdgeColouringFacets = true;
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG, "edge-corrector-associated-edges-coloured.ply") << polyhedron;
	polyhedron.whetherPrintEdgeColouringFacets = false;

	unsigned numNonAssociatedEdges = 0;
	unsigned numCriticalNonAssociatedEdges = 0;
	const double CRITICAL_LENGTH = 1.; // FIXME: hardcoded constant...
	double lengthMax = 0.;

	std::vector<Plane_3> allTangients;
	std::vector<std::vector<int>> clusters;
	int iPlaneCurrent = 0;
	for (auto &i : halfedgesMap)
	{
		assert(i.second != UNINITIALIZED_MAP_VALUE);
		SimpleEdge_3 edge = edges[i.second];
		if (edge.tangients.empty())
		{
			++numNonAssociatedEdges;
			double length = sqrt((edge.A - edge.B).squared_length());
			if (length >= CRITICAL_LENGTH)
				++numCriticalNonAssociatedEdges;
			if (length > lengthMax)
				lengthMax = length;
		}
		else
		{
			std::vector<int> cluster;
			for (const Plane_3 &plane : edge.tangients)
			{
				allTangients.push_back(plane);
				cluster.push_back(iPlaneCurrent++);
			}
			clusters.push_back(cluster);
		}
	}
	printColouredIntersection(allTangients, clusters, "tangient-clusters.ply");

	std::cout << "Number of extracted non-associated edges: " << numNonAssociatedEdges << std::endl;
	std::cout << "Number of critical extracted non-associated edges: " << numCriticalNonAssociatedEdges << std::endl;
	std::cout << "Maximal edge length for critical non-associated edges: " << lengthMax << std::endl;
	// ASSERT(numCriticalNonAssociatedEdges == 0
	//		&& "Some critical edges have no tangients!");
}

std::vector<SimpleEdge_3> extractEdges(std::vector<SimpleEdge_3> edges)
{
	DEBUG_START;
	std::vector<SimpleEdge_3> extractedEdges;
	double zLower = 0., zUpper = 0.;
	if (!tryGetenvDouble("Z_LOWER", zLower) || !tryGetenvDouble("Z_UPPER", zUpper))
	{
		DEBUG_END;
		return extractedEdges;
	}
	std::cout << "Z lower: " << zLower << std::endl;
	std::cout << "Z upper: " << zUpper << std::endl;
	double zMin = 1e10, zMax = 0.;
	unsigned numAssociated = 0;
	for (const SimpleEdge_3 &edge : edges)
	{
		double zA = edge.A.z();
		zMin = zA < zMin ? zA : zMin;
		zMax = zA > zMax ? zA : zMax;
		double zB = edge.B.z();
		zMin = zB < zMin ? zB : zMin;
		zMax = zB > zMax ? zB : zMax;
		if ((zA >= zLower && zA <= zUpper) || (zB >= zLower && zB <= zUpper))
		{
			extractedEdges.push_back(edge);
			if (edge.tangients.size() > 0)
				++numAssociated;
		}
	}
	std::cout << "Z minimal: " << zMin << std::endl;
	std::cout << "Z maximal: " << zMax << std::endl;

	std::cout << "From " << extractedEdges.size() << " extracted edges " << numAssociated << " have tangients"
			  << std::endl;

	DEBUG_END;
	return extractedEdges;
}

void printColouredExtractedPolyhedron(Polyhedron_3 polyhedron, const std::vector<SimpleEdge_3> &edges)
{
	DEBUG_START;
	std::set<int> cluster;
	for (const SimpleEdge_3 &edge : edges)
	{
		cluster.insert(edge.iForward);
		cluster.insert(edge.iBackward);
	}
	std::vector<std::set<int>> clusters;
	clusters.push_back(cluster);
	printColouredPolyhedron(polyhedron, clusters, "edge-corrector-extracted-edges.ply");
	DEBUG_END;
}

std::vector<Plane_3> renumerateFacets(Polyhedron_3 polyhedron, std::vector<SimpleEdge_3> &edges,
									  std::map<int, int> &indices)
{
	DEBUG_START;
	std::vector<Plane_3> planes;
	for (const SimpleEdge_3 &edge : edges)
	{
		indices.insert(std::pair<int, int>(edge.iForward, UNINITIALIZED_MAP_VALUE));
		indices.insert(std::pair<int, int>(edge.iBackward, UNINITIALIZED_MAP_VALUE));
	}

	int i = 0;
	for (auto &pair : indices)
	{
		pair.second = i++;
	}

	i = 0;
	for (auto facet = polyhedron.facets_begin(); facet < polyhedron.facets_end(); ++facet)
	{
		ASSERT(facet->id == i);
		if (indices.find(i) != indices.end())
		{
			planes.push_back(facet->plane());
		}
		++i;
	}
	for (auto &pair : indices)
	{
		std::cout << "map[" << pair.first << "] = " << pair.second << ", plane: " << planes[pair.second] << std::endl;
	}

	for (SimpleEdge_3 &edge : edges)
	{
		edge.iForward = indices[edge.iForward];
		edge.iBackward = indices[edge.iBackward];
	}
	DEBUG_END;
	return planes;
}

void printSetVector(std::vector<std::set<int>> &v)
{
	DEBUG_START;
	for (unsigned i = 0; i < v.size(); ++i)
	{
		std::cout << "  " << i << ": ";
		for (int j : v[i])
		{
			std::cout << j << " ";
		}
		std::cout << std::endl;
	}
	DEBUG_END;
}

void buildFacets(Polyhedron_3 polyhedron, std::vector<SimpleEdge_3> &edges, std::vector<Vector_3> &U,
				 std::vector<double> &H, std::map<int, int> &indices)
{
	DEBUG_START;
	std::vector<Plane_3> planes = renumerateFacets(polyhedron, edges, indices);
	for (const Plane_3 &plane : planes)
	{
		Vector_3 norm = plane.orthogonal_vector();
		double length = sqrt(norm.squared_length());
		norm = norm * (1. / length);
		double value = -plane.d() / length;
		ASSERT(value > 0.);
		U.push_back(norm);
		H.push_back(value);
	}
	DEBUG_END;
}

void buildMainTopology(std::vector<SimpleEdge_3> &edges, std::vector<Vector_3> &u, std::vector<double> &h,
					   std::vector<Vector_3> &points, FixedTopology *FT)
{
	DEBUG_START;
	unsigned iTangient = 0;
	for (unsigned i = 0; i < edges.size(); ++i)
	{
		Vector_3 A = edges[i].A;
		Vector_3 B = edges[i].B;
		for (const Plane_3 plane : edges[i].tangients)
		{
			Vector_3 norm = plane.orthogonal_vector();
			double length = sqrt(norm.squared_length());
			norm = norm * (1. / length);
			double value = -plane.d() / length;
			ASSERT(value > 0.);
			u.push_back(norm);
			h.push_back(value);

			if (norm * A > norm * B)
				FT->tangient[2 * i].insert(iTangient);
			else
				FT->tangient[2 * i + 1].insert(iTangient);
			++iTangient;
		}

		FT->incident[edges[i].iForward].insert(2 * i);
		FT->incident[edges[i].iForward].insert(2 * i + 1);
		FT->incident[edges[i].iBackward].insert(2 * i);
		FT->incident[edges[i].iBackward].insert(2 * i + 1);
		points.push_back(A);
		points.push_back(B);
	}
	ASSERT(iTangient > 0);
	DEBUG_END;
}

void buildInfluents(std::vector<SimpleEdge_3> &edges, FixedTopology *FT)
{
	DEBUG_START;
	for (unsigned i = 0; i < edges.size(); ++i)
	{
		ASSERT(edges[i].iForward != edges[i].iBackward);
		int iForward = edges[i].iForward;
		int iBackward = edges[i].iBackward;
		const std::set<int> &incForward = FT->incident[iForward];
		const std::set<int> &incBackward = FT->incident[iBackward];

		for (int j : incForward)
		{
			if (incBackward.find(j) == incBackward.end())
				FT->influent[iBackward].insert(j);
		}

		for (int j : incBackward)
		{
			if (incForward.find(j) == incForward.end())
				FT->influent[iForward].insert(j);
		}
	}
	DEBUG_END;
}

void buildNeighbors(std::vector<SimpleEdge_3> &edges, std::vector<Vector_3> points, FixedTopology *FT)
{
	DEBUG_START;
	unsigned numOppositeEqual = 0;
	bool allNeighbors = (getenv("ALL_NEIGHBORS") != nullptr);
	for (unsigned i = 0; i < points.size(); ++i)
	{
		Vector_3 A = points[i];
		unsigned iOpposite = i % 2 ? i - 1 : i + 1;
		Vector_3 Aop = points[iOpposite];
		for (unsigned j = 0; j < points.size(); ++j)
		{
			if (i == j || iOpposite == j)
				continue;
			Vector_3 B = points[j];
			unsigned jOpposite = j % 2 ? j - 1 : j + 1;
			ASSERT(iOpposite != jOpposite);
			ASSERT(iOpposite != j);
			ASSERT(i != jOpposite);
			Vector_3 Bop = points[jOpposite];
			double distance = (A - B).squared_length();
			double distanceOp = (Aop - Bop).squared_length();
			if (!allNeighbors && distance < 1e-16)
			{
				FT->neighbors[i].insert(jOpposite);
				FT->neighbors[j].insert(iOpposite);
			}

			if (allNeighbors && distance > 1e-16)
			{
				FT->neighbors[i].insert(j);
			}

			if (distance < 1e-16 && distanceOp < 1e-16)
			{
				std::cout << "Opposite are equal: " << distanceOp << std::endl;
				++numOppositeEqual;
			}
		}
	}
	std::cout << "Number of equal opposites: " << numOppositeEqual << std::endl;
	ASSERT(numOppositeEqual == 0 && "To small distance...");
	DEBUG_END;
}

void checkConsistencyConstraints(std::vector<Vector_3> u, std::vector<double> h, std::vector<Vector_3> U,
								 std::vector<double> H, std::vector<Vector_3> points, FixedTopology *FT, bool Strict)
{
	DEBUG_START;
	std::cout << "Checking consistency constraints..." << std::endl;
	unsigned numViolations = 0;
	unsigned numConsistencyConstraints = 0;
	for (unsigned i = 0; i < points.size(); ++i)
	{
		std::set<int> &tangients = FT->tangient[i];
		std::set<int> &neighbors = FT->neighbors[i];
		std::set<int> falseTangients;
		std::set<int> falseNeighbors;
		for (int j : tangients)
		{
			bool violationFound = false;
			for (int k : neighbors)
			{
				++numConsistencyConstraints;
				double value = u[j] * (points[i] - points[k]);

				if (value < 0.)
				{
					violationFound = true;
					falseNeighbors.insert(k);
					std::cout << "  consistency violation: " << std::endl;
					std::cout << "    major point: " << i << " " << points[i] << std::endl;
					std::cout << "    minor point: " << j << " " << points[j] << std::endl;
					std::cout << "    on direction: " << k << " " << u[j] << std::endl;
					std::cout << "    value: " << value << std::endl;
					++numViolations;
				}
			}
			if (violationFound)
				falseTangients.insert(j);
		}
	}
	std::cout << "Number of violations: " << numViolations << " from " << numConsistencyConstraints
			  << " consistency constraints..." << std::endl;
	ASSERT((!Strict || numViolations == 0) && "Bad starting point");
	DEBUG_END;
}

void shortenEdges(std::vector<Vector_3> &points)
{
	DEBUG_START;
	// FIXME: Make this variable parametrizable
	const double relativeShift = 0.9;
	ASSERT(points.size() % 2 == 0);
	for (unsigned i = 0; i < points.size() / 2; ++i)
	{
		Vector_3 A = points[2 * i];
		Vector_3 B = points[2 * i + 1];
		points[2 * i] = A + (B - A) * relativeShift;
		points[2 * i + 1] = B + (A - B) * relativeShift;
	}
	DEBUG_END;
}

void printFixedTopology(FixedTopology *FT)
{
	DEBUG_START;
	std::cout << "FT->tangient:" << std::endl;
	printSetVector(FT->tangient);
	std::cout << "FT->incident:" << std::endl;
	printSetVector(FT->incident);
	std::cout << "FT->influent:" << std::endl;
	printSetVector(FT->influent);
	std::cout << "FT->neighbors:" << std::endl;
	printSetVector(FT->neighbors);
	DEBUG_END;
}

FixedTopology *buildTopology(Polyhedron_3 polyhedron, std::vector<SimpleEdge_3> &edges, std::vector<Vector_3> &u,
							 std::vector<Vector_3> &U, std::vector<Vector_3> &points, std::vector<double> &h,
							 std::vector<double> &H, std::map<int, int> &indices)
{
	DEBUG_START;
	buildFacets(polyhedron, edges, U, H, indices);

	FixedTopology *FT = new FixedTopology();
	FT->tangient.resize(2 * edges.size());
	FT->incident.resize(U.size());
	FT->influent.resize(U.size());
	FT->neighbors.resize(2 * edges.size());

	buildMainTopology(edges, u, h, points, FT);
	buildInfluents(edges, FT);
	buildNeighbors(edges, points, FT);

	// shortenEdges(points);
	if (getenv("CHECK_STARTING_POINT"))
	{
		// checkConsistencyConstraints(u, h, U, H, points, FT, false);
		checkConsistencyConstraints(u, h, U, H, points, FT, true);
	}
	// printFixedTopology(FT);
	DEBUG_END;
	return FT;
}

Polyhedron_3 obtainPolyhedron(Polyhedron_3 initialP, std::map<int, int> map, IpoptTopologicalCorrector *FTNLP)
{
	DEBUG_START;
	std::vector<Vector_3> directions = FTNLP->getDirections();
	std::vector<double> values = FTNLP->getValues();
	std::vector<Plane_3> planes(initialP.size_of_facets());
	unsigned iFacet = 0;
	for (auto I = initialP.facets_begin(), E = initialP.facets_end(); I != E; ++I)
	{
		auto it = map.find(iFacet);
		if (it != map.end())
		{
			int i = it->second;
			Vector_3 u = directions[i];
			double h = values[i];
			ASSERT(h > 0);
			planes[iFacet] = Plane_3(-u.x(), -u.y(), -u.z(), h);
			std::cout << "Changing plane #" << iFacet << ": " << I->plane() << " |--> " << planes[iFacet] << std::endl;
		}
		else
		{
			planes[iFacet] = I->plane();
		}
		++iFacet;
	}

	Polyhedron_3 intersection(planes);
	std::cout << "Change in facets number: " << initialP.size_of_facets() << " -> " << intersection.size_of_facets()
			  << std::endl;
	ASSERT(initialP.size_of_facets() - intersection.size_of_facets() < map.size() &&
		   "It seems that all extracted facets have gone");
	DEBUG_END;
	return intersection;
}

Polyhedron_3 EdgeCorrector::run()
{
	DEBUG_START;
	std::map<int, int> halfedgesMap;
	std::vector<SimpleEdge_3> edges = getEdges(initialP, halfedgesMap);
	associateEdges(edges, SData);

	printAssociatedEdges(initialP, edges, halfedgesMap);

	std::vector<SimpleEdge_3> mainEdges = extractEdges(edges);
	std::cout << "Number of extracted edges: " << mainEdges.size() << std::endl;
	if (mainEdges.size() == 0)
	{
		DEBUG_END;
		return initialP;
	}

	printColouredExtractedPolyhedron(initialP, mainEdges);

	std::vector<Vector_3> u, U, points;
	std::vector<double> h, H;
	std::map<int, int> map;
	FixedTopology *FT = buildTopology(initialP, mainEdges, u, U, points, h, H, map);
	IpoptTopologicalCorrector *FTNLP = new IpoptTopologicalCorrector(u, h, U, H, points, FT);
	FTNLP->enableModeZfixed();

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

	/* Ask Ipopt to solve the problem */
	auto status = app->OptimizeTNLP(FTNLP);
	if (status != Solve_Succeeded && status != Solved_To_Acceptable_Level)
	{
		MAIN_PRINT("** The problem FAILED!");
		DEBUG_END;
		return initialP;
	}

	MAIN_PRINT("*** The problem solved!");
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG, "INSIDE_FT_NLP-initial.ply") << initialP;
	Polyhedron_3 correctedP = obtainPolyhedron(initialP, map, FTNLP);
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG, "INSIDE_FT_NLP-from-planes.ply") << correctedP;

	delete FTNLP;
	DEBUG_END;
	return correctedP;
}

#undef UNINITIALIZED_MAP_VALUE
