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

int Facet::test_structure()
{
	DEBUG_START;
	int i, facet, pos, nnv;

	for (i = 0; i < numVertices; ++i)
	{
		facet = indVertices[numVertices + 1 + i];
		pos = parentPolyhedron->facets[facet].find_vertex(indVertices[i]);
		if (pos == -1)
		{
			ERROR_PRINT(
					"=======test_structure: Error. Cannot find vertex %d in facet %d",
					indVertices[i], facet);
			parentPolyhedron->facets[facet].my_fprint_all(stdout);
			this->my_fprint_all(stdout);
			DEBUG_END;
			return 1;
		}
		indVertices[2 * numVertices + 1] = pos;
		nnv = parentPolyhedron->facets[facet].numVertices;
		pos = (pos + nnv - 1) % nnv;
		if (parentPolyhedron->facets[facet].indVertices[nnv + 1 + pos] != id)
		{
			ERROR_PRINT(
					"=======test_structure: Error. Wrong neighbor facet for vertex %d in facet %d",
					indVertices[i], facet);
			parentPolyhedron->facets[facet].my_fprint_all(stdout);
			this->my_fprint_all(stdout);
			DEBUG_END;
			return 1;
		}
		parentPolyhedron->facets[facet].indVertices[2 * nnv + 1 + pos] = i;
	}
	DEBUG_END;
	return 0;
}
