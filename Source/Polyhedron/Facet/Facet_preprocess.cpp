#include "PolyhedraCorrectionLibrary.h"

void Facet::preprocess_free()
{
	DEBUG_START;
	if (numVertices <= 0)
	{
		ERROR_PRINT("\tError. nv = %d\n", numVertices);
		DEBUG_END;
		return;
	}
	if (indVertices == NULL)
	{
		ERROR_PRINT("\tError. index == NULL\n");
		DEBUG_END;
		return;
	}
	for (int i = numVertices + 1; i < 3 * numVertices + 1; ++i)
		indVertices[i] = -1;
	indVertices[numVertices] = indVertices[0];
	rgb[0] = 100;
	rgb[1] = 100;
	rgb[2] = 100;
	DEBUG_END;
}

void Facet::preprocess()
{
	DEBUG_START;
	int i, v0, v1;

	for (i = 0; i < numVertices; ++i)
	{
		v0 = indVertices[i];
		v1 = indVertices[i + 1];
		preprocess_edge(v0, v1, i);
	}
	DEBUG_END;
}

void Facet::preprocess_edge(int v0, int v1, int v0_id)
{
	DEBUG_START;
	int i, pos;

	pos = -1;
	for (i = 0; i < parentPolyhedron->numFacets; ++i)
	{
		if (i == id)
			continue;
		pos = parentPolyhedron->facets[i].preprocess_search_edge(v1, v0);
		if (pos != -1)
		{
			indVertices[v0_id + numVertices + 1] = i;
			indVertices[v0_id + 2 * numVertices + 1] = pos;
			break;
		}
	}
	if (pos == -1)
	{
		ERROR_PRINT("Error. Edge (%d %d) cannot be found anywhere\n", v0, v1);
		ASSERT(pos != -1);
		DEBUG_END;
		return;
	}
	DEBUG_END;
}

int Facet::preprocess_search_edge(int v0, int v1)
{
	DEBUG_START;
	int i;
	for (i = 0; i < numVertices; ++i)
		if (indVertices[i] == v0 && indVertices[i + 1] == v1)
			break;
	if (i == numVertices)
	{
		DEBUG_END;
		return -1;
	}
	else
	{
		DEBUG_END;
		return i < numVertices - 1 ? i + 1 : 0;
	}
}

int Facet::preprocess_search_vertex(int v, int& v_next)
{
	DEBUG_START;
	for (int i = 0; i < numVertices; ++i)
		if (indVertices[i] == v)
		{
			v_next = indVertices[i + 1];
			DEBUG_END;
			return i;
		}
	v_next = -1;
	DEBUG_END;
	return -1;
}

