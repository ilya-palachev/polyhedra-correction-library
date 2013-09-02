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
	edgeData.reset(new EdgeData());
	EdgeConstructor* edgeConstructor = new EdgeConstructor(polyhedron);
	edgeConstructor->run(edgeData);
	delete edgeConstructor;
	preprocessAssociations();
	DEBUG_END;
}

void GlobalShadeCorrector::preprocessAssociations()
{
	DEBUG_START;
	associator = new GSAssociator(this);
	associator->preinit();

	for (int iContour = 0; iContour < contourData->numContours; ++iContour)
	{
		for (list<int>::iterator itFacet = facetsCorrected.begin();
				itFacet != facetsCorrected.end(); ++itFacet)
		{
			int iFacet = *itFacet;
			int numVerticesFacet = polyhedron->facets[iFacet].numVertices;
			int* indVertices = polyhedron->facets[iFacet].indVertices;
			for (int iVertex = 0; iVertex < numVerticesFacet; ++iVertex)
			{
				EdgeSetIterator edge =
						edgeData->findEdge(indVertices[iVertex],
						indVertices[iVertex + 1]);
				associator->run(iContour, iFacet, edge);
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
