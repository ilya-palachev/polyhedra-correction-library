/*
 * Copyright (c) 2009-2016 Ilya Palachev <iliyapalachev@gmail.com>
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

#include <set>

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "Constants.h"
#include "Polyhedron/Facet/Facet.h"

void Facet::test_pair_neighbours()
{
	DEBUG_START;
	int i, j;
	for (i = 0; i < numVertices; ++i)
	{
		for (j = 0; j < i; ++j)
		{
			if (indVertices[numVertices + 1 + i] ==
				indVertices[numVertices + 1 + j])
			{
				DEBUG_PRINT("WARNING!!! test_pair_neighbours in facet %d. "
							"neighbour[%d] = %d and neighbour[%d] = %d",
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
				"facet #%d",
				id);

	if (auto polyhedron = parentPolyhedron.lock())
	{
		for (int iVertex = 0; iVertex < numVertices; ++iVertex)
		{
			int iVertexShared = indVertices[iVertex];
			if (iVertexShared == INT_NOT_INITIALIZED)
			{
				DEBUG_PRINT("Vertex at position %d is not initialized.",
							iVertex);
				DEBUG_END;
				return true;
			}

			int iFacetNeighbor = indVertices[numVertices + 1 + iVertex];
			if (iFacetNeighbor == INT_NOT_INITIALIZED)
			{
				DEBUG_PRINT("Info about neighbor facet at position %d is not "
							"initialized.",
							iVertex);
				DEBUG_END;
				return true;
			}

			int iPosition = indVertices[2 * numVertices + 1 + iVertex];
			if (iFacetNeighbor == INT_NOT_INITIALIZED)
			{
				DEBUG_PRINT(
					"Info about shared vertex position at position %d is "
					"not initialized.",
					iVertex);
				DEBUG_END;
				return true;
			}

			Facet *facetNeighbor = &polyhedron->facets[iFacetNeighbor];

			if (iPosition < 0 || iPosition >= facetNeighbor->numVertices)
			{
				ERROR_PRINT("Info about shared vertex #%d position is out of "
							"bounds in facet #%d (facet #%d says that it's in "
							"position %d)",
							iVertexShared, iFacetNeighbor, id, iPosition);
				ASSERT(0);
				DEBUG_END;
				return false;
			}

			if (facetNeighbor->indVertices[iPosition] != iVertexShared)
			{
				ERROR_PRINT(
					"Info about shared vertex #%d position is wrong. "
					"Facet #%d thinks that it at position %d in facet #%d,"
					"but actually there is a vertex %d at that position.",
					iVertexShared, id, iPosition, iFacetNeighbor,
					facetNeighbor->indVertices[iPosition]);
				ASSERT(0);
				DEBUG_END;
				return false;
			}
		}
	}
	DEBUG_END;
	return true;
}

bool Facet::verifyUniqueValues(void)
{
	DEBUG_START;

	std::set<int> vertices;
	for (int iVertex = 0; iVertex < numVertices; ++iVertex)
	{
		vertices.insert(indVertices[iVertex]);
	}

	std::set<int> facets;
	for (int iVertex = 0; iVertex < numVertices; ++iVertex)
	{
		facets.insert(indVertices[numVertices + iVertex + 1]);
	}

	DEBUG_END;
	return (vertices.size() == (unsigned)numVertices) &&
		   (facets.size() == (unsigned)numVertices);
}

bool Facet::correctPlane()
{
	DEBUG_START;
	bool result = !equal(plane.norm, Vector3d(0., 0., 0.), EPS_MIN_DOUBLE);
	DEBUG_END;
	return result;
}
