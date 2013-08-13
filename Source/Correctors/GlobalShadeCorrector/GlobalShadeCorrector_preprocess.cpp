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
	EdgeConstructor* edgeConstructor = new EdgeConstructor(polyhedron,
			edgeData);
	edgeConstructor->run(edgeData);
	preprocessAssociations();
	DEBUG_END;
}

void GlobalShadeCorrector::preprocessAssociations()
{
	DEBUG_START;
	associator->preinit();

	for (int iContour = 0; iContour < contourData->numContours; ++iContour)
	{
		for (int iFacet = 0; iFacet < polyhedron->numFacets; ++iFacet)
		{
			int numVerticesFacet = polyhedron->facets[iFacet].numVertices;
			int* indVertices = polyhedron->facets[iFacet].indVertices;
			for (int iVertex = 0; iVertex < numVerticesFacet; ++iVertex)
			{
				EdgeSetIterator edge =
						edgeData->findEdge(indVertices[iVertex],
						indVertices[iVertex + 1]);
				associator->run(iContour, iFacet, edge->id);
			}
		}
	}
#ifndef NDEBUG
	/* Print found associations : */
	for (EdgeSetIterator edge = edgeData->edges.begin();
			edge != edgeData->edges.end(); ++edge)
	{
		edge->my_fprint(stdout);
		double distance = sqrt(qmod(polyhedron->vertices[edge->v0] -
				polyhedron->vertices[edge->v1]));
		DEBUG_PRINT("\t dist (v_%d, v_%d) = %le", edge->v0, edge->v1,
				distance);
	}
#endif /* NDEBUG */
	DEBUG_END;
}
