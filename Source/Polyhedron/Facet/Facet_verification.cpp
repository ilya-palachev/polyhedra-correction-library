/*
 * Facet_verification.cpp
 *
 *  Created on: 20.06.2013
 *      Author: ilya
 */

#include "PolyhedraCorrectionLibrary.h"

void Facet::test_pair_neighbours()
{
	DEBUG_START;
	int i, j;
	for (i = 0; i < numVertices; ++i)
	{
		for (j = 0; j < i; ++j)
		{
			if (indVertices[numVertices + 1 + i]
					== indVertices[numVertices + 1 + j])
			{
				DEBUG_PRINT(
						"WARNING!!! test_pair_neighbours in facet %d. neighbour[%d] = %d and neighbour[%d] = %d",
						id, j, indVertices[numVertices + 1 + j], i,
						indVertices[numVertices + 1 + i]);
			}
		}

	}
	DEBUG_END;
}

/* Verify that current facet contains proper information about incidence
 * structure of the polyhedron.
 *
 * NOTE: It does not check the information contained in neghbor facets, but in
 * current facet only.
 *
 * FIXME: In previous version of this function (which was called
 * test_structure) there was code that changed the information if it's not
 * correct.
 * After refactoring this change has been removed. Thus, the routines that used
 * this function are now buggy (see somewhere in Coalescer).
 * */
bool Facet::verifyIncidenceStructure()
{
	DEBUG_START;
	DEBUG_PRINT("Verifying information about incidence structure contained in "
			"facet #%d");

	for (int iVertex = 0; iVertex < numVertices; ++iVertex)
	{
		int iVertexShared = indVertices[iVertex];
		if (iVertexShared == INT_NOT_INITIALIZED)
		{
			DEBUG_PRINT("Vertex at position %d is not initialized.", iVertex);
			DEBUG_END;
			return true;
		}

		int iFacetNeighbor = indVertices[numVertices + 1 + iVertex];
		if (iFacetNeighbor == INT_NOT_INITIALIZED)
		{
			DEBUG_PRINT("Info about neighbor facet at position %d is not "
					"initialized.", iVertex);
			DEBUG_END;
			return true;
		}

		int iPosition = indVertices[2 * numVertices + 1 + iVertex];
		if (iFacetNeighbor == INT_NOT_INITIALIZED)
		{
			DEBUG_PRINT("Info about shared vertex position at position %d is "
					"not initialized.", iVertex);
			DEBUG_END;
			return true;
		}

		Facet* facetNeighbor = &parentPolyhedron->facets[iFacetNeighbor];

		if (iPosition < 0 || iPosition >= facetNeighbor->numVertices)
		{
			ERROR_PRINT("Info about shared vertex #%d position is out of "
					"bounds in facet #%d (facet #%d says that it's in "
					"position %d)", iVertexShared, iFacetNeighbor, id,
					iPosition);
			ASSERT(0);
			DEBUG_END;
			return false;
		}

		if (facetNeighbor->indVertices[iPosition] != iVertexShared)
		{
			ERROR_PRINT("Info about shared vertex #%d position is wrong. "
					"Facet #%d thinks that it at position %d in facet #%d,"
					"but actually there is a vertex %d at that position.",
					iVertexShared, id, iPosition, iFacetNeighbor,
					facetNeighbor->indVertices[iPosition]);
			ASSERT(0);
			DEBUG_END;
			return false;
		}
	}
	DEBUG_END;
	return true;
}
