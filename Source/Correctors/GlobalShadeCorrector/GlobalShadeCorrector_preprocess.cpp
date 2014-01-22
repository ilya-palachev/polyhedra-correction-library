/*
 * Copyright (c) 2009-2013 Ilya Palachev <iliyapalachev@gmail.com>
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

#include <cmath>

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "Correctors/GlobalShadeCorrector/GlobalShadeCorrector.h"
#include "Correctors/GlobalShadeCorrector/GSAssociator/GSAssociator.h"
#include "DataContainers/EdgeData/EdgeData.h"
#include "DataConstructors/EdgeConstructor/EdgeConstructor.h"
#include "Polyhedron/Facet/Facet.h"

void GlobalShadeCorrector::preprocess()
{
	DEBUG_START;
	edgeData.reset(new EdgeData());
	shared_ptr<EdgeConstructor> edgeConstructor(new
			EdgeConstructor(polyhedron));
	edgeConstructor->run(edgeData);
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
	
	DEBUG_PRINT("Search of assocations is ended.");
	
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
