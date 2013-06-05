/*
 * EdgeConstructor.cpp
 *
 *  Created on: 05.06.2013
 *      Author: iliya
 */

#include "PolyhedraCorrectionLibrary.h"

EdgeConstructor::EdgeConstructor(Polyhedron* p, EdgeData* e) :
				polyhedron(p),
				edgeData(e)
{
}

EdgeConstructor::~EdgeConstructor()
{
}

void EdgeConstructor::run()
{
	DEBUG_START;
	int numEdgesMax = edgeData->numEdges = 0;
	for (int iFacet = 0; iFacet < polyhedron->numFacets; ++iFacet)
	{
		numEdgesMax += polyhedron->facets[iFacet].numVertices;
	}
	numEdgesMax /= 2;
	DEBUG_PRINT("numEdgesMax = %d", numEdgesMax);
	for (int i = 0; i < polyhedron->numFacets; ++i)
	{
		int numVerticesInFacet = polyhedron->facets[i].numVertices;
		int * index = polyhedron->facets[i].indVertices;
		for (int iVertex = 0; iVertex < numVerticesInFacet; ++iVertex)
		{
			addEdge(numEdgesMax, // Number of edges which we are going add finally
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

	for (int i = 0; i < edgeData->numEdges; ++i)
	{
		edgeData->edges[i].my_fprint(stdout);
	}
	DEBUG_END;
}

void EdgeConstructor::addEdge(int numEdgesMax, int v0, int v1, int f0,
		int f1)
{
	if (edgeData->numEdges >= numEdgesMax)
	{
		DEBUG_PRINT("Warning. List is overflow\n");
		return;
	}

	if (v0 > v1)
	{
		int tmp = v0;
		v0 = v1;
		v1 = tmp;
	}

	if (f0 > f1)
	{
		int tmp = f0;
		f0 = f1;
		f1 = tmp;
	}

	int retvalfind = findEdge(v0, v1);
	if (edgeData->edges[retvalfind].v0 == v0 &&
			edgeData->edges[retvalfind].v1 == v1)
	{
		return;
	}

// If not, add current edge to array of edges :
	for (int i = edgeData->numEdges; i > retvalfind; --i)
	{
		edgeData->edges[i] = edgeData->edges[i - 1];
	}
	edgeData->edges[retvalfind].v0 = v0;
	edgeData->edges[retvalfind].v1 = v1;
	edgeData->edges[retvalfind].f0 = f0;
	edgeData->edges[retvalfind].f1 = f1;
	edgeData->edges[retvalfind].id = edgeData->numEdges;
	++edgeData->numEdges;
}

int EdgeConstructor::findEdge(int v0, int v1)
{
	if (v0 > v1)
	{
		int tmp = v0;
		v0 = v1;
		v1 = tmp;
	}

// Binary search :
	int first = 0;		// Первый элемент в массиве

	int last = edgeData->numEdges;		// Последний элемент в массиве

	while (first < last)
	{
		int mid = (first + last) / 2;
		int v0_mid = edgeData->edges[mid].v0;
		int v1_mid = edgeData->edges[mid].v1;

		if (v0 < v0_mid || (v0 == v0_mid && v1 <= v1_mid))
		{
			last = mid;
		}
		else
		{
			first = mid + 1;
		}
	}
	return last;
}

