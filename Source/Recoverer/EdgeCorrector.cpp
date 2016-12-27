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

EdgeCorrector::EdgeCorrector(Polyhedron_3 initialP,
		SupportFunctionDataPtr SData) :
	initialP(initialP), SData(SData)
{
	DEBUG_START;
	DEBUG_END;
}

static std::vector<SimpleEdge_3> getEdges(Polyhedron_3 P)
{
	DEBUG_START;
	std::vector<bool> visited(P.size_of_halfedges());
	for (unsigned i = 0 ; i < visited.size(); ++i)
		visited[i] = false;
	std::cout << "getEdges: number of halfedges: " << P.size_of_halfedges()
		<< std::endl;

	std::vector<SimpleEdge_3> edges;
	P.initialize_indices();
	CGAL::Origin origin;
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
		edges.push_back(edge);
		visited[I->id] = visited[I->opposite()->id] = true;
	}
	std::cout << "getEdges: number of edges: " << edges.size() << std::endl;
	ASSERT(edges.size() * 2 == P.size_of_halfedges());

	DEBUG_END;
	return edges;
}

void associateEdges(std::vector<SimpleEdge_3> &edges,
		SupportFunctionDataPtr data)
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
		unsigned jBestPair = edges.size();
		unsigned jBest = edges.size();
		double productMaxPair = 0.;
		
		for (unsigned j = 0; j < edges.size(); ++j)
		{
			double productPair = edges[j].A * u + edges[j].B * u;
			if (productPair > productMaxPair)
			{
				productMaxPair = productPair;
				jBestPair = j;
			}	
			if (edges[j].A * u > productMax)
			{
				productMax = edges[j].A * u;
				jBest = j;
			}
			if (edges[j].B * u > productMax)
			{
				productMax = edges[j].B * u;
				jBest = j;
			}
		}

		bool safe = true;
#if 0
		for (const SimpleEdge_3 &edge : edges)
		{
			Vector_3 A = edges[jBestPair].A;
			Vector_3 B = edges[jBestPair].B;
			if (edge.A * u > A * u || edge.A * u > B * u
				|| edge.B * u > A * u || edge.B * u > B * u)
				safe = false;
		}
#endif
		ASSERT(jBestPair != edges.size() && "Failed to find best edge");
		if (safe && jBest == jBestPair &&
				sqrt(productMaxPair - 2. * values(i)) < 1e-1)
			edges[jBestPair].tangients.push_back(planes[i]);
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
	std::cout << "Minimal number of tangients: " << numTangientsMin
		<< std::endl;
	std::cout << "Maximal number of tangients: " << numTangientsMax
		<< std::endl;
	std::cout << "Number of non-associated edges: " << numNonAssociatedEdges
		<< std::endl;
	DEBUG_END;
}

std::vector<SimpleEdge_3> extractEdges(std::vector<SimpleEdge_3> edges)
{
	DEBUG_START;
	std::vector<SimpleEdge_3> extractedEdges;
	double zLower = 0., zUpper = 0.;
	if (!tryGetenvDouble("Z_LOWER", zLower)
			|| !tryGetenvDouble("Z_UPPER", zUpper))
	{
		DEBUG_END;
		return extractedEdges;
	}
	std::cout << "Z lower: " << zLower << std::endl;
	std::cout << "Z upper: " << zUpper << std::endl;
	double zMin = 1e10, zMax = 0.;
	for (const SimpleEdge_3 &edge: edges)
	{
		double zA = edge.A.z();
		zMin = zA < zMin ? zA : zMin;
		zMax = zA > zMax ? zA : zMax;
		double zB = edge.B.z();
		zMin = zB < zMin ? zB : zMin;
		zMax = zB > zMax ? zB : zMax;
		if ((zA >= zLower && zA <= zUpper)
				|| (zB >= zLower && zB <= zUpper))
		{
			extractedEdges.push_back(edge);
		}
	}
	std::cout << "Z minimal: " << zMin << std::endl;
	std::cout << "Z maximal: " << zMax << std::endl;


	DEBUG_END;
	return extractedEdges;
}

void printColouredExtractedPolyhedron(Polyhedron_3 polyhedron,
		const std::vector<SimpleEdge_3> &edges)
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
	printColouredPolyhedron(polyhedron, clusters,
			"edge-corrector-extracted-edges.ply");
	DEBUG_END;
}

std::vector<Plane_3> renumerateFacets(Polyhedron_3 polyhedron,
		std::vector<SimpleEdge_3> &edges, std::map<int, int> &indices)
{
	DEBUG_START;
	std::vector<Plane_3> planes;
#define UNINITIALIZED_MAP_VALUE -1
	for (const SimpleEdge_3 &edge : edges)
	{
		indices.insert(std::pair<int, int>(edge.iForward,
					UNINITIALIZED_MAP_VALUE));
		indices.insert(std::pair<int, int>(edge.iBackward,
					UNINITIALIZED_MAP_VALUE));
	}
#undef UNINITIALIZED_MAP_VALUE

	int i = 0;
	for (auto &pair : indices)
	{
		pair.second = i++;
	}

	i = 0;
	for (auto facet = polyhedron.facets_begin();
			facet < polyhedron.facets_end(); ++facet)
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
		std::cout << "map[" << pair.first << "] = " << pair.second
			<< ", plane: " << planes[pair.second]
			<< std::endl;
	}

	for (SimpleEdge_3 &edge : edges)
	{
		edge.iForward = indices[edge.iForward];
		edge.iBackward = indices[edge.iBackward];
	}
	DEBUG_END;
	return planes;
}

void shortenEdges(std::vector<SimpleEdge_3> &edges)
{
	DEBUG_START;
	// FIXME: Make this variable parametrizable
	const double relativeShift = 0.9;
	for (SimpleEdge_3 &edge : edges)
	{
		Vector_3 A = edge.A;
		Vector_3 B = edge.B;
		edge.A = A + (B - A) * relativeShift;
		edge.B = B + (A - B) * relativeShift;
	}
	DEBUG_END;
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

FixedTopology *buildTopology(Polyhedron_3 polyhedron,
		std::vector<SimpleEdge_3> &edges, std::vector<Vector_3> &u,
		std::vector<Vector_3> &U, std::vector<Vector_3> &points,
		std::vector<double> &h, std::vector<double> &H,
		std::map<int, int> &indices)
{
	DEBUG_START;
	std::vector<Plane_3> planes = renumerateFacets(polyhedron, edges,
			indices);
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

	FixedTopology *FT = new FixedTopology();
	FT->tangient.resize(2 * edges.size());
	FT->incident.resize(planes.size());
	FT->influent.resize(planes.size());
	FT->neighbors.resize(2 * edges.size());

	unsigned iTangient = 0;
	for (unsigned i = 0; i < edges.size(); ++i)
	{
		for (const Plane_3 plane : edges[i].tangients)
		{
			FT->tangient[2 * i].insert(iTangient);
			FT->tangient[2 * i + 1].insert(iTangient);
			++iTangient;
			Vector_3 norm = plane.orthogonal_vector();
			double length = sqrt(norm.squared_length());
			norm = norm * (1. / length);
			double value = -plane.d() / length;
			ASSERT(value > 0.);
			u.push_back(norm);
			h.push_back(value);
		}

		FT->incident[edges[i].iForward].insert(2 * i);
		FT->incident[edges[i].iForward].insert(2 * i + 1);
		FT->incident[edges[i].iBackward].insert(2 * i);
		FT->incident[edges[i].iBackward].insert(2 * i + 1);
		points.push_back(edges[i].A);
		points.push_back(edges[i].B);
	}
	ASSERT(iTangient > 0);

	for (unsigned i = 0; i < edges.size(); ++i)
	{
		ASSERT(edges[i].iForward != edges[i].iBackward);
		int iForward = edges[i].iForward;
		int iBackward = edges[i].iBackward;

		for (int j : FT->incident[iForward])
		{
			if (FT->incident[iBackward].find(j)
					== FT->incident[iBackward].end())
				FT->influent[iBackward].insert(j);
			if (unsigned(j / 2) != i)
			{
				FT->neighbors[2 * i].insert(j);
				FT->neighbors[2 * i + 1].insert(j);
			}
		}

		for (int j : FT->incident[iBackward])
		{
			if (FT->incident[iForward].find(j)
					== FT->incident[iForward].end())
				FT->influent[iForward].insert(j);
			if (unsigned(j / 2) != i)
			{
				FT->neighbors[2 * i].insert(j);
				FT->neighbors[2 * i + 1].insert(j);
			}
		}
	}

	std::cout << "FT->tangient:" << std::endl;
	printSetVector(FT->tangient);
	std::cout << "FT->incident:" << std::endl;
	printSetVector(FT->incident);
	std::cout << "FT->influent:" << std::endl;
	printSetVector(FT->influent);
	std::cout << "FT->neighbors:" << std::endl;
	printSetVector(FT->neighbors);

	DEBUG_END;
	return FT;
}

Polyhedron_3 obtainPolyhedron(Polyhedron_3 initialP, std::map<int, int> map,
		IpoptTopologicalCorrector *FTNLP)
{
	DEBUG_START;
	std::vector<Vector_3> directions = FTNLP->getDirections();
	std::vector<double> values = FTNLP->getValues();
	std::vector<Plane_3> planes(initialP.size_of_facets());
	unsigned iFacet = 0;
	for (auto I = initialP.facets_begin(), E = initialP.facets_end();
			I != E; ++I)
	{
		auto it = map.find(iFacet);
		if (it != map.end())
		{
			int i = it->second;
			planes[iFacet] = Plane_3(-directions[i].x(),
					-directions[i].y(), -directions[i].z(),
					values[i]);
		}
		else
		{
			planes[iFacet] = I->plane();
		}
		++iFacet;
	}

	Polyhedron_3 intersection(planes);
	DEBUG_END;
	return intersection;
}

Polyhedron_3 EdgeCorrector::run()
{
	DEBUG_START;
	std::vector<SimpleEdge_3> edges = getEdges(initialP);
	shortenEdges(edges);
	associateEdges(edges, SData);
	std::vector<SimpleEdge_3> mainEdges = extractEdges(edges);
	std::cout << "Number of extracted edges: " << mainEdges.size()
		<< std::endl;
	if (mainEdges.size() == 0)
	{
		DEBUG_END;
		return initialP;
	}

	printColouredExtractedPolyhedron(initialP, mainEdges);

	std::vector<Vector_3> u, U, points;
	std::vector<double> h, H;
	std::map<int, int> map;
	FixedTopology *FT = buildTopology(initialP, mainEdges, u, U, points, h,
			H, map);
	IpoptTopologicalCorrector *FTNLP = new IpoptTopologicalCorrector(
			u, h, U, H, points, FT);
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
	if (app->OptimizeTNLP(FTNLP) != Solve_Succeeded)
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
