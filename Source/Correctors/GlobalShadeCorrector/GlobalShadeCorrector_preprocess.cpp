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
	edgeConstructor->run();
	preprocessAssociations();
	DEBUG_END;
}

void GlobalShadeCorrector::preprocessAssociations()
{
	DEBUG_START;
	GSAssociator associator(this);
	associator.preinit();

	for (int iContour = 0; iContour < contourData->numContours; ++iContour)
	{
		for (int iFacet = 0; iFacet < polyhedron->numFacets; ++iFacet)
		{
			int numVerticesFacet = polyhedron->facets[iFacet].numVertices;
			int* indVertices = polyhedron->facets[iFacet].indVertices;
			for (int iVertex = 0; iVertex < numVerticesFacet; ++iVertex)
			{
				int iEdge = edgeData->findEdge(indVertices[iVertex],
						indVertices[iVertex + 1]);
				associator.run(iContour, iFacet, iEdge);
			}
		}
	}
	/* Print found associations : */
	for (int iEdge = 0; iEdge < edgeData->numEdges; ++iEdge)
	{
		edgeData->edges[iEdge].my_fprint(stdout);
	}
	DEBUG_END;
}
