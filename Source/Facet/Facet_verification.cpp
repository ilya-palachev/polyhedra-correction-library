/*
 * Facet_verification.cpp
 *
 *  Created on: 20.06.2013
 *      Author: ilya
 */

#include "PolyhedraCorrectionLibrary.h"

void Facet::test_pair_neighbours()
{
	int i, j;
	for (i = 0; i < numVertices; ++i)
	{
		for (j = 0; j < i; ++j)
		{
			if (indVertices[numVertices + 1 + i]
					== indVertices[numVertices + 1 + j])
			{
				printf(
						"WARNING!!! test_pair_neighbours in facet %d. neighbour[%d] = %d and neighbour[%d] = %d\n\n",
						id, j, indVertices[numVertices + 1 + j], i,
						indVertices[numVertices + 1 + i]);
			}
		}

	}
}

int Facet::test_structure()
{

	int i, facet, pos, nnv;

	for (i = 0; i < numVertices; ++i)
	{
		facet = indVertices[numVertices + 1 + i];
		pos = parentPolyhedron->facets[facet].find_vertex(indVertices[i]);
		if (pos == -1)
		{
			printf(
					"=======test_structure: Error. Cannot find vertex %d in facet %d\n",
					indVertices[i], facet);
			parentPolyhedron->facets[facet].my_fprint_all(stdout);
			this->my_fprint_all(stdout);
			return 1;
		}
		indVertices[2 * numVertices + 1] = pos;
		nnv = parentPolyhedron->facets[facet].numVertices;
		pos = (pos + nnv - 1) % nnv;
		if (parentPolyhedron->facets[facet].indVertices[nnv + 1 + pos] != id)
		{
			printf(
					"=======test_structure: Error. Wrong neighbor facet for vertex %d in facet %d\n",
					indVertices[i], facet);
			parentPolyhedron->facets[facet].my_fprint_all(stdout);
			this->my_fprint_all(stdout);
			return 1;
		}
		parentPolyhedron->facets[facet].indVertices[2 * nnv + 1 + pos] = i;
	}
	return 0;
}



