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

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "DataConstructors/EdgeConstructor/EdgeConstructor.h"
#include "DataContainers/EdgeData/EdgeData.h"
#include "Polyhedron/Facet/Facet.h"

EdgeConstructor::EdgeConstructor(PolyhedronPtr p) :
				PDataConstructor(p)
{
	DEBUG_START;
	DEBUG_END;
}

EdgeConstructor::~EdgeConstructor()
{
	DEBUG_START;
	DEBUG_END;
}

void EdgeConstructor::run(EdgeDataPtr edgeData)
{
	DEBUG_START;

	EdgeSetIterator edge;

	for (int i = 0; i < polyhedron->numFacets; ++i)
	{
		int numVerticesInFacet = polyhedron->facets[i].numVertices;
		int * index = polyhedron->facets[i].indVertices;
		for (int iVertex = 0; iVertex < numVerticesInFacet; ++iVertex)
		{
			edgeData->addEdge(index[iVertex], // First vertices
					index[iVertex + 1], // Second vertices
					i, // Current facets id
					index[numVerticesInFacet + 1 + iVertex]); // Id of its neighbor
			DEBUG_PRINT("After iteration #%d we have the "
					"following edge data:", (int) edgeData->edges.size());
			edge = edgeData->edges.begin();
			for (int iEdge = 0; iEdge < edgeData->numEdges; ++iEdge)
			{
				DEBUG_PRINT("\t[%d, %d]", edge->v0, edge->v1);
				++edge;
			}
		}
	}

	edge = edgeData->edges.begin();
	for (int iEdge = 0; iEdge < edgeData->numEdges; ++iEdge)
	{
		edge->id = iEdge;
		++edge;
	}

#ifndef NDEBUG
	edge = edgeData->edges.begin();
	for (int iEdge = 0; iEdge < edgeData->numEdges; ++iEdge)
	{
		edge->my_fprint(stdout);
		++edge;
	}
#endif /* NDEBUG */
	DEBUG_END;
}
