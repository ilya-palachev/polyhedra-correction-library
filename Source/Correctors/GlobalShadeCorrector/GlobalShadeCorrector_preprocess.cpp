/*
 * GlobalShadeCorrector.cpp
 *
 *  Created on: 12.05.2013
 *      Author: iliya
 */

#include "PolyhedraCorrectionLibrary.h"

void GlobalShadeCorrector::preprocess()
{
	DEBUG_START;
	preprocessEdges();
	preprocessAssociations();
	DEBUG_END;
}

void GlobalShadeCorrector::preprocessEdges()
{
	DEBUG_START;
	int numEdgesMax = numEdges = 0;
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

	for (int iEdge = 0; iEdge < numEdges; ++iEdge)
	{
		edges[iEdge].id = iEdge;
	}

	for (int i = 0; i < numEdges; ++i)
	{
		edges[i].my_fprint(stdout);
	}
	DEBUG_END;
}

void GlobalShadeCorrector::preprocessAssociations()
{
	DEBUG_START;
	GSAssociator associator;
	associator.preinit();

	for (int iContour = 0; iContour < numContours; ++iContour)
	{
		for (int iFacet = 0; iFacet < polyhedron->numFacets; ++iFacet)
		{
			int numVerticesFacet = polyhedron->facets[iFacet].numVertices;
			int* indVertices = polyhedron->facets[iFacet].indVertices;
			for (int iVertex = 0; iVertex < numVerticesFacet; ++iVertex)
			{
				int iEdge = findEdge(indVertices[iVertex],
						indVertices[iVertex + 1]);
				associator.run(iContour, iFacet, iEdge);
			}
		}
	}
	/* Print found associations : */
	for (int iEdge = 0; iEdge < numEdges; ++iEdge)
	{
		edges[iEdge].my_fprint(stdout);
	}
	DEBUG_END;
}

void GlobalShadeCorrector::addEdge(int numEdgesMax, int v0, int v1, int f0,
		int f1)
{
	if (numEdges >= numEdgesMax)
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
	if (edges[retvalfind].v0 == v0 && edges[retvalfind].v1 == v1)
	{
		return;
	}

// If not, add current edge to array of edges :
	for (int i = numEdges; i > retvalfind; --i)
	{
		edges[i] = edges[i - 1];
	}
	edges[retvalfind].v0 = v0;
	edges[retvalfind].v1 = v1;
	edges[retvalfind].f0 = f0;
	edges[retvalfind].f1 = f1;
	edges[retvalfind].id = numEdges;
	++numEdges;
}

int GlobalShadeCorrector::findEdge(int v0, int v1)
{
	if (v0 > v1)
	{
		int tmp = v0;
		v0 = v1;
		v1 = tmp;
	}

// Binary search :
	int first = 0;		// Первый элемент в массиве

	int last = numEdges;		// Последний элемент в массиве

	while (first < last)
	{
		int mid = (first + last) / 2;
		int v0_mid = edges[mid].v0;
		int v1_mid = edges[mid].v1;

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

