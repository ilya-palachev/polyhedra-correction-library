/*
 * EdgeData.cpp
 *
 *  Created on: 17.05.2013
 *      Author: iliya
 */

#include "PolyhedraCorrectionLibrary.h"

EdgeData::EdgeData() :
				edges(NULL),
				numEdges(0)
{
}

EdgeData::EdgeData(int numEdgesMax) :
				edges(new Edge[numEdgesMax]),
				numEdges(0)
{
	for (int iEdge = 0; iEdge < numEdgesMax; ++iEdge)
	{
		edges[iEdge] = Edge(INT_NOT_INITIALIZED, INT_NOT_INITIALIZED,
				INT_NOT_INITIALIZED, INT_NOT_INITIALIZED, INT_NOT_INITIALIZED);
	}
}

EdgeData::~EdgeData()
{
	if (edges != NULL)
	{
		delete[] edges;
		edges = NULL;
	}
}

bool EdgeData::operator ==(const EdgeData& e)
{
	if (numEdges != e.numEdges)
	{
		return false;
	}

	for (int iEdge = 0; iEdge < numEdges; ++iEdge)
	{
		if (edges[iEdge] != e.edges[iEdge])
		{
			return false;
		}
	}

	return true;
}

bool EdgeData::operator !=(const EdgeData& e)
{
	return !(*this == e);
}


void EdgeData::addEdge(int numEdgesMax, int v0, int v1, int f0)
{
	DEBUG_START;
	DEBUG_PRINT("Trying to add edge (%d, %d) to the edge list", v0, v1);
	if (v0 < 0 || v1 < 0 || f0 < 0)
	{
		ERROR_PRINT("Negative parameter: v0 = %d, v1 = %d, f0 = %d",
				v0, v1, f0);
		return;
	}
	if (v0 > v1)
	{
		int tmp = v0;
		v0 = v1;
		v1 = tmp;
	}

	int retvalfind = findEdge(v0, v1);
	if (edges[retvalfind].v0 == v0 && edges[retvalfind].v1 == v1)
	{
		if (edges[retvalfind].f0 == INT_NOT_INITIALIZED
				&& edges[retvalfind].f1 == INT_NOT_INITIALIZED)
		{
			DEBUG_PRINT("Edge (%d, %d) already presents in the list,"
					"facet values are empty, initializing value #0", v0, v1);
			edges[retvalfind].f0 = f0;
			DEBUG_END;
			return;
		}

		if (edges[retvalfind].f0 != INT_NOT_INITIALIZED
				&& edges[retvalfind].f1 == INT_NOT_INITIALIZED)
		{
			DEBUG_PRINT("Edge (%d, %d) already presents in the list,"
					"facet value #1 is empty, initializing value #0", v0, v1);
			if (edges[retvalfind].f0 < f0)
			{
				edges[retvalfind].f1 = f0;
			}
			else
			{
				edges[retvalfind].f1 = edges[retvalfind].f0;
				edges[retvalfind].f0 = f0;
			}
			DEBUG_END;
			return;
		}

		if (edges[retvalfind].f0 == INT_NOT_INITIALIZED
				&& edges[retvalfind].f1 != INT_NOT_INITIALIZED)
		{
			DEBUG_PRINT("Edge (%d, %d) already presents in the list,"
					"facet value #0 is empty, initializing value #0", v0, v1);
			if (edges[retvalfind].f1 < f0)
			{
				edges[retvalfind].f0 = edges[retvalfind].f1;
				edges[retvalfind].f1 = f0;
			}
			else
			{
				edges[retvalfind].f0 = f0;
			}
			DEBUG_END;
			return;
		}

		if (edges[retvalfind].f0 != INT_NOT_INITIALIZED
				&& edges[retvalfind].f1 != INT_NOT_INITIALIZED)
		{
			DEBUG_PRINT("Edge (%d, %d) already presents in the list,"
					"facet values are non-empty, skipping...", v0, v1);
			DEBUG_END;
			return;
		}
	}

	if (numEdges >= numEdgesMax)
	{
		DEBUG_PRINT("Warning. List is overflow\n");
		return;
	}

	for (int i = numEdges; i > retvalfind; --i)
	{
		edges[i] = edges[i - 1];
	}
	edges[retvalfind].v0 = v0;
	edges[retvalfind].v1 = v1;
	edges[retvalfind].f0 = f0;
	edges[retvalfind].f1 = INT_NOT_INITIALIZED;
	edges[retvalfind].id = numEdges;
	++numEdges;
	DEBUG_PRINT("Edge (%d, %d) has been successfully added to the edge list",
			v0, v1);
	DEBUG_END;
}

void EdgeData::addEdge(int numEdgesMax, int v0, int v1, int f0, int f1)
{
	DEBUG_START;
	DEBUG_PRINT("Trying to add edge (%d, %d) to the edge list", v0, v1);
	if (v0 < 0 || v1 < 0 || f0 < 0 || f1 < 0)
	{
		ERROR_PRINT("Negative parameter: v0 = %d, v1 = %d, f0 = %d, f1 = %d",
				v0, v1, f0, f1);
		return;
	}

	if (v0 > v1)
	{
		int tmp = v0;
		v0 = v1;
		v1 = tmp;
	}

	if (f0 > f1)
	{
		int tmp = f0;
		f0 = f1;
		f1 = tmp;
	}

	int retvalfind = findEdge(v0, v1);
	if (edges[retvalfind].v0 == v0 && edges[retvalfind].v1 == v1)
	{
		DEBUG_PRINT("Edge (%d, %d) already presents in the list!", v0, v1);
		DEBUG_END;
		return;
	}

	if (numEdges >= numEdgesMax)
	{
		DEBUG_PRINT("Warning. List is overflow\n");
		return;
	}

	for (int i = numEdges; i > retvalfind; --i)
	{
		edges[i] = edges[i - 1];
	}
	edges[retvalfind].v0 = v0;
	edges[retvalfind].v1 = v1;
	edges[retvalfind].f0 = f0;
	edges[retvalfind].f1 = f1;
	edges[retvalfind].id = numEdges;
	++numEdges;
	DEBUG_PRINT("Edge (%d, %d) has been successfully added to the edge list",
			v0, v1);
	DEBUG_END;
}

int EdgeData::findEdge(int v0, int v1)
{
	if (v0 > v1)
	{
		int tmp = v0;
		v0 = v1;
		v1 = tmp;
	}

// Binary search :
	int first = 0;		// Первый элемент в массиве

	int last = numEdges;		// Последний элемент в массиве

	while (first < last)
	{
		int mid = (first + last) / 2;
		int v0_mid = edges[mid].v0;
		int v1_mid = edges[mid].v1;

		if (v0 < v0_mid || (v0 == v0_mid && v1 <= v1_mid))
		{
			last = mid;
		}
		else
		{
			first = mid + 1;
		}
	}
	return last;
}

