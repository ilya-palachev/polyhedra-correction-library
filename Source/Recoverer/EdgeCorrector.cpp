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
#include "Common.h"
#include "EdgeCorrector.h"
#include "Colouring.h"
#include "DataConstructors/SupportFunctionDataConstructor/SupportFunctionDataConstructor.h"

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

Vector_3 projectOnBasis(Vector_3 e1, Vector_3 e2, Vector_3 a)
{
	DEBUG_START;
	double x1 = a * e1;
	Vector_3 a1 = e1 * x1;
	double x2 = a * e2;
	Vector_3 a2 = e2 * x2;
	Vector_3 projection = a1 + a2;
	DEBUG_END;
	return projection;
}

#define SOME_BIG_DOUBLE 1e10
#define SOME_SMALL_DOUBLE 1e-10

double calculateDiff(SimpleEdge_3 edge, Vector_3 e1, Vector_3 e2, Vector_3 u)
{
	DEBUG_START;
	Vector_3 origin(0., 0., 0.);
	Vector_3 a = projectOnBasis(e1, e2, edge.A - origin);
	Vector_3 b = projectOnBasis(e1, e2, edge.B - origin);
	Vector_3 segment = a - b;
	double square = segment * segment;
	if (square < SOME_SMALL_DOUBLE)
	{
		DEBUG_END;
		return SOME_BIG_DOUBLE;
	}
	double alpha = -(a * segment) / square;
	Vector_3 v = alpha * a + (1. - alpha) * b;
	double length = sqrt(v * v);
	if (length < SOME_SMALL_DOUBLE)
	{
		DEBUG_END;
		return SOME_BIG_DOUBLE;
	}
	v = (1. / length) * v;
	Vector_3 d = v - u;
	DEBUG_END;
	return d * d;
}

void associateEdges(std::vector<SimpleEdge_3> edges,
		SupportFunctionDataPtr data)
{
	DEBUG_START;
	std::vector<Vector_3> directions = data->supportDirections<Vector_3>();
	std::vector<Plane_3> planes = data->supportPlanes();
	unsigned numItems = directions.size();
	double L1 = 0.;
	double L2 = 0.;
	double Linf = 0.;
	for (unsigned i = 0; i < numItems; ++i)
	{
		Vector_3 u = directions[i];
		// FIXME: This should be rewritten on information about
		// contours.
		Vector_3 u_z(0., 0., u.z());
		Vector_3 u_xy(u.x(), u.y(), 0.);

		unsigned jBest = edges.size();
		double diffBest = SOME_BIG_DOUBLE;
		
		for (unsigned j = 0; j < edges.size(); ++j)
		{
			double diff = calculateDiff(edges[j], u_z, u_xy, u);
			if (diff < diffBest)
			{
				diffBest = diff;
				jBest = j;
			}	
		}
		ASSERT(jBest != edges.size() && "Failed to find best edge");
		edges[jBest].tangients.push_back(planes[i]);
		L1 += diffBest;
		L2 += diffBest * diffBest;
		Linf = diffBest > Linf ? diffBest : Linf;
	}
	std::cout << "L1: " << L1 << ", mean: " << L1 / numItems << std::endl;
	std::cout << "L2: " << sqrt(L2) << ", mean: " << L2 / sqrt(numItems)
		<< std::endl;
	std::cout << "Linf: " << Linf << std::endl;

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

Polyhedron_3 EdgeCorrector::run()
{
	DEBUG_START;
	std::vector<SimpleEdge_3> edges = getEdges(initialP);
	associateEdges(edges, SData);
	std::vector<SimpleEdge_3> mainEdges = extractEdges(edges);
	std::cout << "Number of extracted edges: " << mainEdges.size()
		<< std::endl;
	if (mainEdges.size() == 0)
	{
		DEBUG_END;
		return initialP;
	}
	std::set<int> cluster;
	for (const SimpleEdge_3 &edge: mainEdges)
	{
		cluster.insert(edge.iForward);
		cluster.insert(edge.iBackward);
	}
	std::vector<std::set<int>> clusters;
	clusters.push_back(cluster);
	printColouredPolyhedron(initialP, clusters,
			"edge-corrector-extracted-edges.ply");

	Polyhedron_3 correctedP = initialP;
	DEBUG_END;
	return correctedP;
}
