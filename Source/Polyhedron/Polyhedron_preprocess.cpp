#include "PolyhedraCorrectionLibrary.h"

void Polyhedron::preprocess_polyhedron()
{
	DEBUG_START;
	int i;
	DEBUG_PRINT("Cleaning facets");
	for (i = 0; i < numFacets; ++i)
	{
		if (facets[i].numVertices < 1)
			continue;
		facets[i].preprocess_free();
	}

	DEBUG_PRINT("Facets preprocessing");
	for (i = 0; i < numFacets; ++i)
	{
		if (facets[i].numVertices < 1)
			continue;
		facets[i].preprocess();
	}

	DEBUG_PRINT("VertexInfos preprocessing");
	vertexInfos = new VertexInfo[numVertices];
	for (i = 0; i < numVertices; ++i)
	{
		vertexInfos[i] = VertexInfo(i, vertices[i], &(*this));
		vertexInfos[i].preprocess();
	}
	DEBUG_END;
}
