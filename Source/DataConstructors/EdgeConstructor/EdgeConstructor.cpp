/*
 * EdgeConstructor.cpp
 *
 *  Created on: 05.06.2013
 *      Author: iliya
 */

#include "PolyhedraCorrectionLibrary.h"

EdgeConstructor::EdgeConstructor(Polyhedron* p, EdgeData* e) :
				PDataConstructor(p)
{
}

EdgeConstructor::~EdgeConstructor()
{
}

void EdgeConstructor::run(EdgeData* &edgeData)
{
	DEBUG_START;
	int numEdgesMax = 0;
	for (int iFacet = 0; iFacet < polyhedron->numFacets; ++iFacet)
	{
		numEdgesMax += polyhedron->facets[iFacet].numVertices;
	}
	numEdgesMax /= 2;
	edgeData = new EdgeData(numEdgesMax);
	DEBUG_PRINT("numEdgesMax = %d", numEdgesMax);

	list<Edge>::iterator edge;

	for (int i = 0; i < polyhedron->numFacets; ++i)
	{
		int numVerticesInFacet = polyhedron->facets[i].numVertices;
		int * index = polyhedron->facets[i].indVertices;
		for (int iVertex = 0; iVertex < numVerticesInFacet; ++iVertex)
		{
			edgeData->addEdge(numEdgesMax, // Number of edges which we are going add finally
					index[iVertex], // First vertices
					index[iVertex + 1], // Second vertices
					i, // Current facets id
					index[numVerticesInFacet + 1 + iVertex]); // Id of its neighbor
			DEBUG_PRINT("After iteration #%d we have the "
					"following edge data:");
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
