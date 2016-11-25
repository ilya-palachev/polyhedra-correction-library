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

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
typedef CGAL::Simple_cartesian<double> SCKernel;

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "EdgeCorrector.h"
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
	for (auto I = P.halfedges_begin(), E = P.halfedges_end(); I != E; ++I)
	{
		if (visited[I->id])
			continue;
		SimpleEdge_3 edge;
		edge.A = I->vertex()->point();
		edge.B = I->opposite()->vertex()->point();
		edge.iForward = I->facet()->id;
		edge.iBackward = I->opposite()->facet()->id;
		edges.push_back(edge);
		visited[I->id] = visited[I->opposite()->id] = true;
	}
	std::cout << "getEdges: number of edges: " << edges.size() << std::endl;
	ASSERT(edges.size() * 2 == P.size_of_halfedges());

	DEBUG_END;
	return edges;
}

Polyhedron_3 EdgeCorrector::run()
{
	DEBUG_START;
	std::vector<SimpleEdge_3> edges = getEdges(initialP);
	Polyhedron_3 correctedP = initialP;
	DEBUG_END;
	return correctedP;
}
