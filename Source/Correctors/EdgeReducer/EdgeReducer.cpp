/*
 * EdgeReducer.cpp
 *
 *  Created on: Aug 22, 2013
 *      Author: ilya
 */

#include "PolyhedraCorrectionLibrary.h"

EdgeReducer::EdgeReducer() :
		PCorrector()
{
	DEBUG_START;
	DEBUG_END;
}

EdgeReducer::EdgeReducer(Polyhedron* p) :
		PCorrector(p)
{
	DEBUG_START;
	DEBUG_END;
}

EdgeReducer::~EdgeReducer()
{
	DEBUG_START;
	DEBUG_END;
}

bool EdgeReducer::run(EdgeSetIterator _edge, EdgeDataPtr _edgeData)
{
	DEBUG_START;

	edge = _edge;
	edgeData = _edgeData;

	init();

	/* Stage 1. */
	if (!updateFacets())
	{
		DEBUG_END;
		return false;
	}

	/* Stage 2. */
	if (!rePreprocessFacets())
	{
		DEBUG_END;
		return false;
	}

	/* Stage 3. */
	if (!updateEdges())
	{
		DEBUG_END;
		return false;
	}

	/* Stage 4. */
	if (!updateVertexInfos())
	{
		DEBUG_END;
		return false;
	}

	DEBUG_END;
	return true;
}

/* Initialize commonly used members. */
void EdgeReducer::init()
{
	iVertexReduced = edge->v1;
	iVertexStayed = edge->v0;
	DEBUG_PRINT("Reduced vertex: %d", iVertexReduced);
	DEBUG_PRINT(" Stayed vertex: %d", iVertexStayed);

	vertexInfoReduced = &polyhedron->vertexInfos[iVertexReduced];
	vertexInfoStayed  = &polyhedron->vertexInfos[iVertexStayed];

	vertexInfoReduced->my_fprint_all(stderr);
}

/* Stage 1. Update data structures "Facet" for those facets that contain
 * "removed" vertex. */
bool EdgeReducer::updateFacets()
{
	DEBUG_START;
	bool ifCase1aHappened = false;
	bool ifCase1bHappened = false;

	DEBUG_PRINT("Stage 1. Updating structures \"Facet\".");
	for (int iFacet = 0; iFacet < vertexInfoReduced->numFacets; ++iFacet)
	{
		int iFacetCurrent = vertexInfoReduced->indFacets[iFacet];
		DEBUG_PRINT("\tUpdating facet #%d.", iFacetCurrent);

		facetsUpdated.insert(iFacetCurrent);

		int iPositionReduced =
				vertexInfoReduced->indFacets[2 * vertexInfoReduced->numFacets +
		                                    iFacet + 1];
		Facet* facetCurr = &polyhedron->facets[iFacetCurrent];

		DEBUG_PRINT("\t before:");
		facetCurr->my_fprint_all(stderr);

		DEBUG_PRINT("facetCurr->indVertices[iPositionReduced = %d] = %d",
				iPositionReduced, facetCurr->indVertices[iPositionReduced]);
		DEBUG_PRINT("iVertexReduced = %d", iVertexReduced);
		ASSERT(facetCurr->indVertices[iPositionReduced] == iVertexReduced);

		int iPositionPrev = (facetCurr->numVertices + iPositionReduced - 1) %
				facetCurr->numVertices;
		int iPositionNext = (facetCurr->numVertices + iPositionReduced + 1) %
				facetCurr->numVertices;

		/* 2 cases: */

		/* 1). In case when "stayed" vertex LAYS in the current facet, delete
		 * "removed" vertex from facet. */

		/* 1a). The "stayed" vertex is EARLIER in the list of vertices than
		 * the "reduced" vertex. */
		if (facetCurr->indVertices[iPositionPrev] == iVertexStayed)
		{
			ifCase1aHappened = true;
			DEBUG_PRINT("Case 1a: \"stayed\" vertex lays in facet, "
					"it's earlier than reduced one");
			/* Let v2 be "stayed" vertex, v3 - "reduced" vertex
			 * Let numVertices == 6.
			 *
			 * In this case iPositionReduced == 3, iPositionPrev == 2
			 *
			 * Before:
			 * 0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18
			 * --------------------------------------------------------
			 * v0 v1 v2 v3 v4 v5 v0 f0 f1 f2 f3 f4 f5 p0 p1 p2 p3 p4 p5
			 *
			 *                        |
			 *                        |
			 *                        v
			 *
			 * After:
			 * 0  1  2     3  4  5  6  7     8  9  10 11 12    13 14 15
			 * --------------------------------------------------------
			 * v0 v1 v2    v4 v5 v0 f0 f1    f3 f4 f5 p0 p1    p3 p4 p5
			 *
			 *                   ^
			 *                   |
			 *                   |
			 *             need to be rewritten
			 *             if v0 is "reduced"
			 * */

			for (int i = iPositionReduced;
					i < facetCurr->numVertices + iPositionPrev; ++i)
			{
				facetCurr->indVertices[i] = facetCurr->indVertices[i + 1];
			}

			for (int i = facetCurr->numVertices + iPositionPrev;
					i < 2 * facetCurr->numVertices + iPositionPrev - 1; ++i)
			{
				facetCurr->indVertices[i] = facetCurr->indVertices[i + 2];
			}

			for (int i = 2 * facetCurr->numVertices + iPositionPrev - 1;
					i < 3 * facetCurr->numVertices - 2; ++i)
			{
				facetCurr->indVertices[i] = facetCurr->indVertices[i + 3];
			}

			--facetCurr->numVertices;
		}

		/* 1b). The "stayed" vertex is LATER in the list of vertices than
		 * the "reduced" vertex. */
		else if (facetCurr->indVertices[iPositionNext] == iVertexStayed)
		{
			ifCase1bHappened = true;
			DEBUG_PRINT("Case 1b: \"stayed\" vertex lays in facet, "
					"it's later than reduced one");
			/* Let v3 be "stayed" vertex, v2 - "reduced" vertex
			 * Let numVertices == 6.
			 *
			 * In this case iPositionReduced == 2, iPositionNext == 3
			 *
			 * Before:
			 * 0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18
			 * --------------------------------------------------------
			 * v0 v1 v2 v3 v4 v5 v0 f0 f1 f2 f3 f4 f5 p0 p1 p2 p3 p4 p5
			 *
			 *                        |
			 *                        |
			 *                        v
			 *
			 * After:
			 * 0  1     2  3  4  5  6  7     8  9  10 11 12    13 14 15
			 * --------------------------------------------------------
			 * v0 v1    v3 v4 v5 v0 f0 f1    f3 f4 f5 p0 p1    p3 p4 p5
			 *
			 *                   ^
			 *                   |
			 *                   |
			 *             need to be rewritten
			 *             if v0 is "reduced"
			 * */

			for (int i = iPositionReduced;
					i < facetCurr->numVertices + iPositionReduced; ++i)
			{
				facetCurr->indVertices[i] = facetCurr->indVertices[i + 1];
			}

			for (int i = facetCurr->numVertices + iPositionReduced;
					i < 2 * facetCurr->numVertices + iPositionReduced - 1; ++i)
			{
				facetCurr->indVertices[i] = facetCurr->indVertices[i + 2];
			}

			for (int i = 2 * facetCurr->numVertices + iPositionReduced - 1;
					i < 3 * facetCurr->numVertices - 2; ++i)
			{
				facetCurr->indVertices[i] = facetCurr->indVertices[i + 3];
			}

			--facetCurr->numVertices;
		}

		/* 2). In case when "stayed" vertex DOES NOT LAY in the current facet,
		 * replace "reduced" vertex with "stayed" vertex. */
		else
		{
			DEBUG_PRINT("Case 1a: \"stayed\" vertex does not lay in facet");
			facetCurr->indVertices[iPositionReduced] = iVertexStayed;
		}

		/* Cycling vertex needs to be rewritten after the updating. */
		facetCurr->indVertices[facetCurr->numVertices] =
				facetCurr->indVertices[0];

		DEBUG_PRINT("\t after:");
		facetCurr->my_fprint_all(stderr);

		/* If after the updating the number of vertices incident to current
		 * facet is less than 3, we reduce the degenerated facet by
		 * transmitting the information about neighbors contained in this
		 * facet. */
		if (facetCurr->numVertices < 3)
		{
			DEBUG_PRINT("Facet #%d is degenerated => will be reduced. ");
			cutDegeneratedFacet(iFacetCurrent);
		}
	}

	/* For debugging purposes we have added the checking of the fact that
	 * cases 1a and 1b have really happened. */

	if (!ifCase1aHappened)
	{
		ERROR_PRINT("case 1a never happened");
	}

	if (!ifCase1bHappened)
	{
		ERROR_PRINT("case 1b never happened");
	}

	ASSERT(ifCase1aHappened && ifCase1bHappened);
	if (!ifCase1aHappened || !ifCase1bHappened)
	{
		DEBUG_END;
		return false;
	}

	DEBUG_END;
	return true;
}

/* The routine for cutting the degenerated facet. */
void EdgeReducer::cutDegeneratedFacet(int iFacet)
{
	DEBUG_START;
	Facet* facet = &polyhedron->facets[iFacet];

	/* Assume that we remove vertices one-by-one. This will provide this
	 * statement. */
	ASSERT(facet->numVertices == 2);

	int iVertex0 = facet->indVertices[0];
	int iVertex1 = facet->indVertices[1];
	int iFacet0 = facet->indVertices[3];
	int iFacet1 = facet->indVertices[4];
	int iPosition0 = facet->indVertices[5];
	int iPosition1 = facet->indVertices[6];

	Facet* facet0 = &polyhedron->facets[iFacet0];
	Facet* facet1 = &polyhedron->facets[iFacet1];

	/* Transmit the information about "facet1" from "facet" to "facet0". */
	int iPositionChanged0 = (facet0->numVertices + iPosition0 - 1)
			% facet0->numVertices;
	facet0->indVertices[facet0->numVertices + 1 + iPositionChanged0] = iFacet1;
	facet0->indVertices[2 * facet0->numVertices + 1 + iPositionChanged0] =
			iPosition1;

	/* Transmit the information about "facet0" from "facet" to "facet1". */
	int iPositionChanged1 = (facet1->numVertices + iPosition1 - 1)
			% facet1->numVertices;
	facet1->indVertices[facet1->numVertices + 1 + iPositionChanged1] = iFacet1;
	facet1->indVertices[2 * facet1->numVertices + 1 + iPositionChanged1] =
			iPosition0;

	/* Clear current facet. */
	facet->clear();

	DEBUG_END;
}


/* Stage 2. We need to re-preprocess updated facets and their neighbors, since the
 * positions of vertices change because of the removement of the reduced
 * vertex. */
bool EdgeReducer::rePreprocessFacets()
{
	DEBUG_START;

	DEBUG_PRINT("Stage 2. Re-preprocess all neighbors of updated facets");

	set<int> facetsPreprocessed;
	/* 1). Add set "facetsUpdated" to set "facetsPreprocessed". */
	facetsPreprocessed.insert(facetsUpdated.begin(), facetsUpdated.end());

	/* 2). Add neighbors of facets inside set "facetsUpdated" to set
	 * "facetsPreprocessed". */
	for (set<int>::iterator itFacetUpdated = facetsUpdated.begin();
			itFacetUpdated != facetsUpdated.end(); ++itFacetUpdated)
	{
		Facet* facetUpdated = &polyhedron->facets[*itFacetUpdated];
		for (int i = 0; i < facetUpdated->numVertices; ++i)
		{
			facetsPreprocessed.insert(
					facetUpdated->indVertices[facetUpdated->numVertices
					                          + 1 + i]);
		}
	}

	/* 3). Re-process all facets collected to the set "facetsPreprocessed". */
	for (set<int>::iterator itFacet = facetsPreprocessed.begin();
			itFacet != facetsPreprocessed.end(); ++itFacet)
	{
		DEBUG_PRINT("*itFacet = %d", *itFacet);
		DEBUG_PRINT("polyhedron->facets[%d].id = %d", *itFacet,
				polyhedron->facets[*itFacet].id);
		ASSERT(polyhedron->facets[*itFacet].id == *itFacet);

		polyhedron->facets[*itFacet].preprocess();
	}


#ifndef NDEBUG
	DEBUG_PRINT("-------------- After stage 1 we have the following");
	DEBUG_PRINT("               configuration of facets:");

	GraphDumperGEXF* graphDumper = new GraphDumperGEXF();

	for (set<int>::iterator itFacet = facetsPreprocessed.begin();
			itFacet != facetsPreprocessed.end(); ++itFacet)
	{
		Facet* facetCurr = &polyhedron->facets[*itFacet];
		DEBUG_PRINT("Dumping facet #%d", facetCurr->id);
		facetCurr->my_fprint_all(stderr);

		graphDumper->collect(facetCurr);
	}
	graphDumper->dump("poly-data-out/facets_dump.gexf");

	facetsPreprocessed.clear();

	DEBUG_PRINT("-------------- end of dumping facets");
#endif /* NDEBUG */

	return true;
	DEBUG_END;
}


/* Stage 3. Update data structures "Edge" for those facets that contain
 * "removed" vertex. */
bool EdgeReducer::updateEdges()
{

	DEBUG_PRINT("Stage 3. Updating structures \"Edge\".");
	for (int iFacet = 0; iFacet < vertexInfoReduced->numFacets; ++iFacet)
	{
		int iVertexNeighbour =
				vertexInfoReduced->indFacets[vertexInfoReduced->numFacets +
				                             iFacet + 1];
		DEBUG_PRINT("\tUpdating edge [%d, %d]", iVertexReduced,
				iVertexNeighbour);
		EdgeSetIterator edgeUpdated = edgeData->findEdge(iVertexReduced,
							iVertexNeighbour);
		if (edgeUpdated == edgeData->edges.end())
		{
			continue;
		}
		if (iVertexNeighbour != iVertexStayed)
		{
			if (edgeUpdated->v0 == iVertexReduced &&
					edgeUpdated->v1 == iVertexNeighbour)
			{
				edgeUpdated->v0 = iVertexStayed;
			}
			else if (edgeUpdated->v0 == iVertexNeighbour &&
					edgeUpdated->v1 == iVertexReduced)
			{
				edgeUpdated->v1 = iVertexStayed;
			}
			else
			{
				ERROR_PRINT("Failed to find edge [%d, %d] in edge data.",
						iVertexReduced, iVertexNeighbour);
				DEBUG_PRINT("   Found edge: [%d, %d]",
						edgeUpdated->v0,
						edgeUpdated->v1);
				DEBUG_PRINT("Previous edge: [%d, %d]",
						edgeUpdated->v0,
						edgeUpdated->v1);
				DEBUG_PRINT("    Next edge: [%d, %d]",
						edgeUpdated->v0,
						edgeUpdated->v1);

				DEBUG_PRINT("Printing edge data:");
				EdgeSetIterator edgePrinted = edgeData->edges.begin();
				for (int iEdge = 0; iEdge < edgeData->numEdges; ++iEdge)
				{
					DEBUG_PRINT("Edge #%d: [%d, %d]", iEdge,
							edgePrinted->v0, edgePrinted->v1);
					++edgePrinted;
				}
				DEBUG_END;
				return false;
			}

			/* If succeeded to find the edge, add edge with proper values and
			 * erase the old one. */
			edgeData->edges.insert(Edge(*edgeUpdated));
			edgeData->edges.erase(edgeUpdated);
		}
		else
		{
			DEBUG_PRINT("\tThis edge must be deleted at all.");
			edgeData->edges.erase(edgeUpdated);
			--edgeData->numEdges;
		}
	}

#ifndef NDEBUG
	/* Verify that the reduced edge has been actually removed from
	 * edge set. */
	EdgeSetIterator edgeRemoved = edgeData->findEdge(iVertexReduced,
								iVertexStayed);
	if (edgeRemoved != edgeData->edges.end())
	{
		ERROR_PRINT("The edge [%d, %d] has not been removed from the "
				"set!");
		ASSERT(0);
		DEBUG_END;
		return false;
	}
#endif

	DEBUG_END;
	return true;
}


/* Stage 4. Rebuild data structure "VertexInfo" for all vertices
 * that lay in facets which contained the "reduced" vertex
 * before the reduction happened. */
bool EdgeReducer::updateVertexInfos()
{
	DEBUG_START;

	DEBUG_PRINT("Stage 4. Updating structures \"VertexInfo\".");
	for (int iFacet = 0; iFacet < vertexInfoReduced->numFacets; ++iFacet)
	{
		int iFacetCurr = vertexInfoReduced->indFacets[iFacet];
		Facet* facetCurr = &polyhedron->facets[iFacetCurr];
		for (int iVertex = 0; iVertex < facetCurr->numVertices; ++iVertex)
		{
			int iVertexCurrent = facetCurr->indVertices[iVertex];
			DEBUG_PRINT("\tUpdating vertexInfo #%d", iVertexCurrent);
			DEBUG_PRINT("\tBefore:");
			polyhedron->vertexInfos[iVertexCurrent].my_fprint_all(stdout);

			polyhedron->vertexInfos[iVertexCurrent].preprocess();

			DEBUG_PRINT("\tAfter:");
			polyhedron->vertexInfos[iVertexCurrent].my_fprint_all(stdout);

#ifndef NDEBUG
			for (int iFacet = 0; iFacet < polyhedron->numFacets; ++iFacet)
			{
				int iPositionFound =
						polyhedron->facets[iFacet].find_vertex(iVertexCurrent);
				if (iPositionFound != -1)
				{
					DEBUG_PRINT("Vertex #%d has been found in facet #%d at "
							"position %d", iVertexCurrent, iFacet,
							iPositionFound);
				}
			}
#endif /* NDEBUG */

			/* Sometimes, when the facet has been degenerated, the vertex
			 * incident to it can degenerate, too. In this case we must remove
			 * it from all incident facets.
			 *
			 * In old terminology it has been called "hanging vertices". */
			if (polyhedron->vertexInfos[iVertexCurrent].numFacets < 3)
			{
				DEBUG_PRINT("Too small number of incident facets was found for"
						" vertex #%d => it will be reduced.", iVertexCurrent);
				cutDegeneratedVertex(iVertexCurrent);
				DEBUG_END;
				return false;
			}
		}
	}

	DEBUG_END;
	return true;
}

/* The routine for cutting degenerated vertices. */
void EdgeReducer::cutDegeneratedVertex(int iVertex)
{
	DEBUG_START;
	DEBUG_PRINT("Removing vetrexInfo #%d", iVertex);
	VertexInfo* vertexInfo = &polyhedron->vertexInfos[iVertex];

	/* Assume that the vertex is incident to exactly 2 facets. */
	ASSERT(vertexInfo->numFacets == 2);

	int iFacet0 = vertexInfo->indFacets[0];
	int iFacet1 = vertexInfo->indFacets[1];
	int iVertex0 = vertexInfo->indFacets[3];
	int iVertex1 = vertexInfo->indFacets[4];
	int iPosition0 = vertexInfo->indFacets[5];
	int iPosition1 = vertexInfo->indFacets[6];

	Facet* facet0 = &polyhedron->facets[iFacet0];
	Facet* facet1 = &polyhedron->facets[iFacet1];

	facet0->my_fprint_all(stderr);
	facet1->my_fprint_all(stderr);

	/* 1). Remove cut vertex out from both facets. */
	DEBUG_PRINT("iPosition0 = %d", iPosition0);
	DEBUG_PRINT("iVertex0 = %d", iVertex0);
	DEBUG_PRINT("facet0->indVertices[%d] = %d", iPosition0,
			facet0->indVertices[iPosition0]);
	DEBUG_PRINT("iPosition1 = %d", iPosition1);
	DEBUG_PRINT("iVertex1 = %d", iVertex1);
	DEBUG_PRINT("facet1->indVertices[%d] = %d", iPosition1,
			facet1->indVertices[iPosition1]);

	ASSERT(facet0->indVertices[iPosition0] == iVertex);
	facet0->remove(iPosition0);

	ASSERT(facet1->indVertices[iPosition1] == iVertex);
	facet1->remove(iPosition1);

	/* 2). Add all the neighbors of facets to the list. */
	set<int> facetsPreprocessed;
	facetsPreprocessed.insert(iFacet0);
	facetsPreprocessed.insert(iFacet1);
	for (int i = 0; i < facet0->numVertices; ++i)
	{
		facetsPreprocessed.insert(
				facet0->indVertices[facet0->numVertices
				                    + 1 + i]);
	}
	for (int i = 0; i < facet1->numVertices; ++i)
	{
		facetsPreprocessed.insert(
				facet0->indVertices[facet1->numVertices
				                    + 1 + i]);
	}

	/* 3). Re-preprocess all the facets in the list. */
	for (set<int>::iterator itFacet = facetsPreprocessed.begin();
			itFacet != facetsPreprocessed.end(); ++itFacet)
	{
		DEBUG_PRINT("Re-preprocessing facet #%d", *itFacet);

		DEBUG_PRINT("*itFacet = %d", *itFacet);
		DEBUG_PRINT("polyhedron->facets[%d].id = %d", *itFacet,
				polyhedron->facets[*itFacet].id);
		ASSERT(polyhedron->facets[*itFacet].id == *itFacet);

		polyhedron->facets[*itFacet].my_fprint_all(stderr);

		polyhedron->facets[*itFacet].preprocess();
	}

	DEBUG_END;
}
