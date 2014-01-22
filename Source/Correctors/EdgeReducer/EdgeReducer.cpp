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

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "Correctors/EdgeReducer/EdgeReducer.h"
#include "Polyhedron/Facet/Facet.h"
#include "Polyhedron/VertexInfo/VertexInfo.h"
#include "Dumpers/GraphDumperGEXF/GraphDumperGEXF.h"

EdgeReducer::EdgeReducer() :
		PCorrector()
{
	DEBUG_START;
	DEBUG_END;
}

EdgeReducer::EdgeReducer(shared_ptr<Polyhedron> p) :
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

bool EdgeReducer::run(EdgeSetIterator _edge, EdgeDataPtr _edgeData,
		EdgesWorkingSets& _edgesWS)
{
	DEBUG_START;

	edge = _edge;
	edgeData = _edgeData;
	edgesWS = _edgesWS;

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

	/* Verification of edge data. */
	if (!verifyEdgeData())
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



	}

	/* If after the updating the number of vertices incident to current
	 * facet is less than 3, we reduce the degenerated facet by
	 * transmitting the information about neighbors contained in this
	 * facet. */
	for (int iFacet = 0; iFacet < vertexInfoReduced->numFacets; ++iFacet)
	{
		int iFacetCurrent = vertexInfoReduced->indFacets[iFacet];
		Facet* facetCurr = &polyhedron->facets[iFacetCurrent];

		if (facetCurr->numVertices < 3)
		{
			DEBUG_PRINT("Facet #%d is degenerated => will be reduced. ",
					iFacetCurrent);
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
	DEBUG_PRINT("Cutting facet #%d", iFacet);
	Facet* facet = &polyhedron->facets[iFacet];
	facet->my_fprint_all(stderr);

	/* Assume that we remove vertices one-by-one. This will provide this
	 * statement. */
	ASSERT(facet->numVertices == 2);

	/* Workaround: we need to re-prepocess current facet here, since the
	 * information about inicidence srtucture can be obsoloete at this time. */
	facet->preprocess();

	int iVertex0 = facet->indVertices[0];
	int iVertex1 = facet->indVertices[1];
	int iFacet0 = facet->indVertices[3];
	int iFacet1 = facet->indVertices[4];
	int iPosition0 = facet->indVertices[5];
	int iPosition1 = facet->indVertices[6];
	
	DEBUG_PRINT("iFacet0 = %d, iFacet1 = %d", iFacet0, iFacet1);
	ASSERT(iFacet0 != iFacet1);

	Facet* facet0 = &polyhedron->facets[iFacet0];
	Facet* facet1 = &polyhedron->facets[iFacet1];

	if (facet0->indVertices[iPosition0] != iVertex0)
	{
		ERROR_PRINT("facet0->indVertices[%d] = %d != %d", iPosition0,
				facet0->indVertices[iPosition0], iVertex0);
		facet0->my_fprint_all(stderr);
		ASSERT(0);
		DEBUG_END;
		return;
	}

	if (facet1->indVertices[iPosition1] != iVertex1)
	{
		ERROR_PRINT("facet1->indVertices[%d] = %d != %d", iPosition1,
				facet1->indVertices[iPosition1], iVertex1);
		facet1->my_fprint_all(stderr);
		ASSERT(0);
		DEBUG_END;
		return;
	}

	/* Transmit the information about "facet1" from "facet" to "facet0". */
	int iPositionChanged0 = (facet0->numVertices + iPosition0 - 1)
			% facet0->numVertices;

	DEBUG_PRINT("facets[%d]->indVertices[%d] = %d -> %d (new value)", iFacet0,
			facet0->numVertices + 1 + iPositionChanged0,
			facet0->indVertices[facet0->numVertices + 1 + iPositionChanged0],
			iFacet1);
	facet0->indVertices[facet0->numVertices + 1 + iPositionChanged0] = iFacet1;

	DEBUG_PRINT("facets[%d]->indVertices[%d] = %d -> %d (new value)", iFacet0,
			2 * facet0->numVertices + 1 + iPositionChanged0,
			facet0->indVertices[2 * facet0->numVertices + 1 +
			                    iPositionChanged0],
			iPosition1);
	facet0->indVertices[2 * facet0->numVertices + 1 + iPositionChanged0] =
			iPosition1;

	/* Transmit the information about "facet0" from "facet" to "facet1". */
	int iPositionChanged1 = (facet1->numVertices + iPosition1 - 1)
			% facet1->numVertices;

	DEBUG_PRINT("facets[%d]->indVertices[%d] = %d -> %d (new value)", iFacet1,
			facet1->numVertices + 1 + iPositionChanged1,
			facet1->indVertices[facet1->numVertices + 1 + iPositionChanged1],
			iFacet0);
	facet1->indVertices[facet1->numVertices + 1 + iPositionChanged1] = iFacet0;

	DEBUG_PRINT("facets[%d]->indVertices[%d] = %d -> %d (new value)", iFacet1,
			2 * facet1->numVertices + 1 + iPositionChanged1,
			facet1->indVertices[2 * facet1->numVertices + 1 +
			                    iPositionChanged1],
			                    iPosition0);
	facet1->indVertices[2 * facet1->numVertices + 1 + iPositionChanged1] =
			iPosition0;

	/* Clear current facet. */
	facet->clear();

	/* Update information about incident facets in edge
	 * [iVertex0, iVertex1]. */
	EdgeSetIterator edgeUpdated = edgeData->findEdge(iVertex0, iVertex1);

	if (edgeUpdated == edgeData->edges.end())
	{
		ERROR_PRINT("Failed to find edge [%d, %d] in edge data.", iVertex0,
				iVertex1);
		ASSERT(0);
		DEBUG_END;
		return;
	}

	DEBUG_PRINT("Changing edge [%d, %d] facets from f0 = %d, f1 = %d",
			edgeUpdated->v0, edgeUpdated->v1, edgeUpdated->f0, edgeUpdated->f1);
	
	if (iFacet0 < iFacet1)
	{
		edgeUpdated->f0 = iFacet0;
		edgeUpdated->f1 = iFacet1;
	}
	else
	{
		edgeUpdated->f0 = iFacet1;
		edgeUpdated->f1 = iFacet0;
	}

	DEBUG_PRINT("Changing edge [%d, %d] facets to   f0 = %d, f1 = %d",
			edgeUpdated->v0, edgeUpdated->v1, edgeUpdated->f0, edgeUpdated->f1);

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

		/* Verify the information about incidence structure after removals. */
		ASSERT(facetCurr->verifyIncidenceStructure());

		graphDumper->collect(facetCurr);
	}
	graphDumper->dump("poly-data-out/facets_dump.gexf");

	DEBUG_PRINT("-------------- end of dumping facets");
#endif /* NDEBUG */

	facetsPreprocessed.clear();
	
	DEBUG_END;
	return true;
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
			Edge edgeNew(*edgeUpdated);
			if (edgeNew.v0 == iVertexReduced &&
					edgeNew.v1 == iVertexNeighbour)
			{
				edgeNew.v0 = iVertexStayed;
			}
			else if (edgeNew.v0 == iVertexNeighbour &&
					edgeNew.v1 == iVertexReduced)
			{
				edgeNew.v1 = iVertexStayed;
			}
			else
			{
				ERROR_PRINT("Failed to find edge [%d, %d] in edge data.",
						iVertexReduced, iVertexNeighbour);

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
			edgesWS.edgesEdited.insert(pair<int, int> (edgeNew.v0,
								edgeNew.v1));

			DEBUG_PRINT("Erasing edge [%d, %d] and inserting edge [%d, %d]",
					edgeUpdated->v0, edgeUpdated->v1, edgeNew.v0, edgeNew.v1);
			edgeData->edges.erase(edgeUpdated);
			pair<EdgeSetIterator, bool> addResult = edgeData->addEdge(edgeNew);
			EdgeSetIterator edgeAdded = addResult.first;

			/* These 2 assertions are added to check the validity of
			 * information about incident facets. */
			if (edgeAdded->f0 < 0 || edgeAdded->f0 >= polyhedron->numFacets)
			{
				ERROR_PRINT("Facet id f0 = %d is out of bounds 0 <= i < %d. It "
						"happened during processing the edge #%d = [%d, %d] "
						"with info f0 = %d, f1 = %d",
						edgeAdded->f0, polyhedron->numFacets, edgeAdded->id,
						edgeAdded->v0, edgeAdded->v1, edgeAdded->f0,
						edgeAdded->f1);
				ASSERT(0);
				DEBUG_END;
				return false;
			}

			if (edgeAdded->f1 < 0 || edgeAdded->f1 >= polyhedron->numFacets)
			{
				ERROR_PRINT("Facet id f1 = %d is out of bounds 0 <= i < %d. It "
						"happened during processing the edge #%d = [%d, %d] "
						"with info f0 = %d, f1 = %d",
						edgeAdded->f1, polyhedron->numFacets, edgeAdded->id,
						edgeAdded->v0, edgeAdded->v1, edgeAdded->f0,
						edgeAdded->f1);
				ASSERT(0);
				DEBUG_END;
				return false;
			}


			ASSERT(edgeData->findEdge(edgeNew.v0, edgeNew.v1) !=
					edgeData->edges.end());
		}
		else
		{
			DEBUG_PRINT("\tThis edge must be deleted at all.");
			edgesWS.edgesErased.insert(pair<int, int> (edgeUpdated->v0,
					edgeUpdated->v1));
			edgeData->edges.erase(edgeUpdated);
			--edgeData->numEdges;
		}
	}

	/* Ensure that the reduced edge has been actually removed from
	 * the edge set. */
	EdgeSetIterator edgeRemoved = edgeData->findEdge(iVertexReduced,
								iVertexStayed);
	if (edgeRemoved != edgeData->edges.end())
	{
		DEBUG_PRINT("The edge [%d, %d] has not been removed from the "
				"set!", edgeRemoved->v0, edgeRemoved->v1);
		edgesWS.edgesErased.insert(pair<int, int> (edgeRemoved->v0,
				edgeRemoved->v1));
		edgeData->edges.erase(edgeRemoved);
		--edgeData->numEdges;
	}

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

	/* 1). Create a queue and push indices of all facets incident to reduced vertex
	 * into it. */
	queue<int> facetsQueue;
	for (int iFacet = 0; iFacet < vertexInfoReduced->numFacets; ++iFacet)
	{
		int iFacetCurr = vertexInfoReduced->indFacets[iFacet];
		facetsQueue.push(iFacetCurr);
	}

	/* 2). While queue is not empty, process each facet in it.
	 * New indices can be added in facets by routine "cutDegeneratedVertex"
	 * in case when less than 3 vertices are left in some facet after removal
	 * after calling routine "Facet::clear". */
	while (!facetsQueue.empty())
	{
		int iFacetCurr = facetsQueue.front();
		facetsQueue.pop();
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
				cutDegeneratedVertex(iVertexCurrent, facetsQueue);
			}
		}
	}

	DEBUG_END;
	return true;
}

/* The routine for cutting degenerated vertices. */
void EdgeReducer::cutDegeneratedVertex(int iVertex, queue<int>& facetsQueue)
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

	ASSERT(facet0->verifyIncidenceStructure());
	ASSERT(facet1->verifyIncidenceStructure());

	ASSERT(facet0->indVertices[iPosition0] == iVertex);
	facet0->remove(iPosition0);
	if (facet0->numVertices < 3)
	{
		ASSERT(facet0->numVertices == 2);
		facetsQueue.push(facet0->indVertices[3]);
		facetsQueue.push(facet0->indVertices[4]);
		cutDegeneratedFacet(facet0->id);
	}

	ASSERT(facet1->indVertices[iPosition1] == iVertex);
	facet1->remove(iPosition1);
	if (facet1->numVertices < 3)
	{
		ASSERT(facet1->numVertices == 2);
		facetsQueue.push(facet1->indVertices[3]);
		facetsQueue.push(facet1->indVertices[4]);
		cutDegeneratedFacet(facet1->id);
	}

	/* 2). Add all the neighbors of facets to the list. */
	set<int> facetsPreprocessed;
	facetsPreprocessed.insert(iFacet0);
	facetsPreprocessed.insert(iFacet1);

	/* Since we have deleted a vertex from these facets,
	 * we need to update information about positions of vertices contained
	 * in vertexInfo's for all incident vertices of "facet0" and "facet1". */
	facetsQueue.push(iFacet0);
	facetsQueue.push(iFacet1);

	for (int i = 0; i < facet0->numVertices; ++i)
	{
		facetsPreprocessed.insert(
				facet0->indVertices[facet0->numVertices
				                    + 1 + i]);
	}
	for (int i = 0; i < facet1->numVertices; ++i)
	{
		facetsPreprocessed.insert(
				facet1->indVertices[facet1->numVertices
				                    + 1 + i]);
	}

	/* 3). Re-preprocess all the facets in the list. */
	for (set<int>::iterator itFacet = facetsPreprocessed.begin();
			itFacet != facetsPreprocessed.end(); ++itFacet)
	{
		DEBUG_PRINT("Re-preprocessing facet #%d", *itFacet);
		polyhedron->facets[*itFacet].my_fprint_all(stderr);
		polyhedron->facets[*itFacet].preprocess();
	}

	/* Verify incidence structure after removal. */
	for (set<int>::iterator itFacet = facetsPreprocessed.begin();
			itFacet != facetsPreprocessed.end(); ++itFacet)
	{
		ASSERT(polyhedron->facets[*itFacet].verifyIncidenceStructure());
	}

	/* 4). Remove 2 corresponding edges from the edge set and add one proper
	 * edge. */
	EdgeSetIterator edge0 = edgeData->findEdge(iVertex, iVertex0);
	ASSERT(edge0 != edgeData->edges.end());

	EdgeSetIterator edge1 = edgeData->findEdge(iVertex, iVertex1);
	ASSERT(edge1 != edgeData->edges.end());

	pair<EdgeSetIterator, bool> returnValue =
			edgeData->addEdge(iVertex0, iVertex1, iFacet0, iFacet1);
	EdgeSetIterator edgeNew = returnValue.first;
	ASSERT(edgeNew != edgeData->edges.end());
	edgesWS.edgesAdded.insert(pair<int, int> (edgeNew->v0, edgeNew->v1));

	/* These 2 assertions are added to check the validity of
	 * information about incident facets. */
	if (edgeNew->f0 < 0 || edgeNew->f0 >= polyhedron->numFacets)
	{
		ERROR_PRINT("Facet id f0 = %d is out of bounds 0 <= i < %d. It "
				"happened during processing the edge #%d = [%d, %d] "
				"with info f0 = %d, f1 = %d",
				edgeNew->f0, polyhedron->numFacets, edgeNew->id,
				edgeNew->v0, edgeNew->v1, edgeNew->f0, edgeNew->f1);
		ASSERT(0);
		DEBUG_END;
		return;
	}

	if (edgeNew->f1 < 0 || edgeNew->f1 >= polyhedron->numFacets)
	{
		ERROR_PRINT("Facet id f1 = %d is out of bounds 0 <= i < %d. It "
				"happened during processing the edge #%d = [%d, %d] "
				"with info f0 = %d, f1 = %d",
				edgeNew->f1, polyhedron->numFacets, edgeNew->id,
				edgeNew->v0, edgeNew->v1, edgeNew->f0, edgeNew->f1);
		ASSERT(0);
		DEBUG_END;
		return;
	}

	/*
	 * Information about associations is accumulated from removed edges to new
	 * one.
	 *
	 * FIXME: In case when edge "edge" already exists in the set, we need to
	 * merge the lists of associations, because otherwise we will get some
	 * associations more than one time.
	 */
	DEBUG_VARIABLE int numAssociationsBefore = edgeNew->assocList.size();
	
	edgeNew->assocList.insert(edgeNew->assocList.end(),
			edge0->assocList.begin(), edge0->assocList.end());
	edgeNew->assocList.insert(edgeNew->assocList.end(),
			edge1->assocList.begin(), edge1->assocList.end());
	ASSERT((unsigned) edgeNew->assocList.size() == numAssociationsBefore +
			edge0->assocList.size() + edge1->assocList.size());

	edgesWS.edgesErased.insert(pair<int, int> (edge0->v0, edge0->v1));
	edgeData->edges.erase(edge0);

	edgesWS.edgesErased.insert(pair<int, int> (edge1->v0, edge1->v1));
	edgeData->edges.erase(edge1);

	--edgeData->numEdges;

	DEBUG_END;
}

bool EdgeReducer::verifyEdgeData()
{
	DEBUG_START;
	for (int iFacet = 0; iFacet < polyhedron->numFacets; ++iFacet)
	{
		Facet* facet = &polyhedron->facets[iFacet];
		for (int iVertex = 0; iVertex < facet->numVertices; ++iVertex)
		{
			int v0 = facet->indVertices[iVertex];
			int v1 = facet->indVertices[iVertex + 1];
			EdgeSetIterator edgeFound = edgeData->findEdge(v0, v1);
			if (edgeFound == edgeData->edges.end())
			{
				ERROR_PRINT("Failed to find edge [%d, %d] in edge data",
						v0, v1);
				facet->my_fprint_all(stderr);
				ASSERT(0);
				return false;
			}
		}
	}
	DEBUG_END;
	return true;
}
