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
		}
	}

	for (int iEdge = 0; iEdge < edgeData->numEdges; ++iEdge)
	{
		edgeData->edges[iEdge].id = iEdge;
	}

#ifndef NDEBUG
	for (int i = 0; i < edgeData->numEdges; ++i)
	{
		edgeData->edges[i].my_fprint(stdout);
	}
#endif /* NDEBUG */
	DEBUG_END;
}
