#include "PolyhedraCorrectionLibrary.h"

void VertexInfo::preprocess()
{
	DEBUG_START;
	int i;
	int pos_curr;
	int pos_next;
	int v_curr;
	int fid_first;
	int fid_curr;
	int fid_next;

    DEBUG_PRINT("1. Searching first facet : ");
	for (i = 0; i < parentPolyhedron->numFacets; ++i)
	{
		pos_next = parentPolyhedron->facets[i].preprocess_search_vertex(id,
				v_curr);
		if (pos_next != -1)
			break;
	}
	if (pos_next == -1)
	{
		return;
	}
	fid_first = fid_next = i;

    DEBUG_PRINT("2. Counting the number of facets : ");
	numFacets = 0;
	do
	{
		++numFacets;
		pos_curr = pos_next;
		fid_curr = fid_next;

		DEBUG_PRINT("\t Facet currently visited be preprocessor: %d",
				fid_curr);

		DEBUG_PRINT("\t Jumping from facet #%d, position %d (vertex #%d)",
				fid_curr, pos_curr, v_curr);

		parentPolyhedron->facets[fid_curr].get_next_facet(pos_curr, pos_next,
				fid_next, v_curr);

		DEBUG_PRINT("\t           to facet #%d, position %d (vertex #%d)",
				fid_next, pos_next, v_curr);
		parentPolyhedron->facets[fid_curr].my_fprint_all(stderr);

		if (pos_next == -1 || fid_next == -1)
		{
			ERROR_PRINT("Error. Cannot find v%d in f%d\n",
					v_curr, fid_curr);
			DEBUG_END;
			return;
		}

		ASSERT(numFacets <= parentPolyhedron->numFacets);
		if (numFacets > parentPolyhedron->numFacets)
		{
			ERROR_PRINT("Endless loop occurred!");
			DEBUG_END;
			return;
		}

	} while (fid_next != fid_first);

    DEBUG_PRINT("Total number of facets is %d\n", numFacets);
	indFacets = new int[3 * numFacets + 1];

    DEBUG_PRINT("3. Building the VECTOR :");
	pos_next = parentPolyhedron->facets[fid_first].preprocess_search_vertex(id,
			v_curr);
	fid_next = fid_first;
	for (i = 0; i < numFacets; ++i)
	{
		pos_curr = pos_next;
		fid_curr = fid_next;

		parentPolyhedron->facets[fid_curr].get_next_facet(pos_curr, pos_next,
				fid_next, v_curr);
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

