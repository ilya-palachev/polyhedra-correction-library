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

#include "DebugAssert.h"
#include "DebugPrint.h"
#include "Polyhedron/Facet/Facet.h"
#include "Polyhedron/VertexInfo/VertexInfo.h"

void VertexInfo::preprocess()
{
	DEBUG_START;
	int pos_curr = -1;
	int pos_next = -1;
	int v_curr = -1;
	int fid_first = -1;
	int fid_curr = -1;
	int fid_next = -1;

	Facet *facets = NULL;
	int numFacetsTotal = 0;

	if (auto polyhedron = parentPolyhedron.lock())
	{
		facets = polyhedron->facets;
		numFacetsTotal = polyhedron->numFacets;
	}
	else
	{
		ASSERT_PRINT(0, "parentPolyhedron expired!");
		DEBUG_END;
		return;
	}

	DEBUG_PRINT("1. Searching first facet : ");
	for (int i = 0; i < numFacetsTotal; ++i)
	{
		pos_next = facets[i].preprocess_search_vertex(id, v_curr);
		if (pos_next != -1)
		{
			fid_first = fid_next = i;
			break;
		}
	}
	if (pos_next == -1)
	{
		return;
	}

	DEBUG_PRINT("2. Counting the number of facets : ");
	numFacets = 0;
	do
	{
		++numFacets;
		pos_curr = pos_next;
		fid_curr = fid_next;

		DEBUG_PRINT("\t Facet currently visited by preprocessor: %d", fid_curr);

		DEBUG_PRINT("\t Jumping from facet #%d, position %d (vertex #%d)",
					fid_curr, pos_curr, v_curr);

		facets[fid_curr].get_next_facet(pos_curr, pos_next, fid_next, v_curr);

		/* This assertion has been added for debugging purposes, because
		 * sometimes wrong transformations of polyhedron cause the
		 * incorrectness of data inside facet arrays. */
		ASSERT(facets[fid_curr].indVertices[pos_curr] ==
			   facets[fid_next].indVertices[pos_next]);

		DEBUG_PRINT("\t           to facet #%d, position %d (vertex #%d)",
					fid_next, pos_next, v_curr);

#ifndef NDEBUG
		facets[fid_curr].my_fprint_all(stderr);
#endif /* NDEBUG */

		if (pos_next == -1 || fid_next == -1)
		{
			ERROR_PRINT("Error. Cannot find v%d in f%d\n", v_curr, fid_curr);
			DEBUG_END;
			return;
		}

		ASSERT(numFacets <= numFacetsTotal);
		if (numFacets > numFacetsTotal)
		{
			ERROR_PRINT("Endless loop occurred!");
			DEBUG_END;
			return;
		}

	} while (fid_next != fid_first);

	DEBUG_PRINT("Total number of facets is %d\n", numFacets);

	if (indFacets != NULL)
	{
		delete[] indFacets;
	}

	indFacets = new int[3 * numFacets + 1];

	DEBUG_PRINT("3. Building the VECTOR :");
	pos_next = facets[fid_first].preprocess_search_vertex(id, v_curr);
	fid_next = fid_first;
	for (int i = 0; i < numFacets; ++i)
	{
		pos_curr = pos_next;
		fid_curr = fid_next;

		facets[fid_curr].get_next_facet(pos_curr, pos_next, fid_next, v_curr);
		indFacets[i] = fid_curr;
		indFacets[i + numFacets + 1] = v_curr;
		indFacets[i + 2 * numFacets + 1] = pos_curr;
		DEBUG_PRINT("\t%d %d %d\n", fid_curr, v_curr, pos_curr);
	}
	indFacets[numFacets] = indFacets[0];
	DEBUG_END;
}

void VertexInfo::find_and_replace_facet(int from, int to)
{
	DEBUG_START;
	for (int i = 0; i < numFacets + 1; ++i)
		if (indFacets[i] == from)
		{
			indFacets[i] = to;
		}
	DEBUG_END;
}

void VertexInfo::find_and_replace_vertex(int from, int to)
{
	DEBUG_START;
	for (int i = numFacets + 1; i < 2 * numFacets + 1; ++i)
		if (indFacets[i] == from)
		{
			indFacets[i] = to;
		}
	DEBUG_END;
}
