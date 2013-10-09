/*
 * EdgeConstructor.cpp
 *
 *  Created on: 05.06.2013
 *      Author: iliya
 */

#include "PolyhedraCorrectionLibrary.h"

EdgeConstructor::EdgeConstructor(shared_ptr<Polyhedron> p) :
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
