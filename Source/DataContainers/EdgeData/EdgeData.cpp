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
	DEBUG_START;
	DEBUG_PRINT(COLOUR_RED "Attention!!! edgeData is being deleted now!"
			COLOUR_NORM);
	if (edges != NULL)
	{
		delete[] edges;
		edges = NULL;
	}
	DEBUG_END;
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
		ASSERT(!(v0 < 0 || v1 < 0 || f0 < 0));
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
					"facet value #1 is empty, initializing value #1", v0, v1);
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
	DEBUG_PRINT("Trying to add edge [%d, %d] to the edge list", v0, v1);
	DEBUG_PRINT("\tnumEdges = %d, numEdgesMax = %d", numEdges, numEdgesMax);
	if (v0 < 0 || v1 < 0 || f0 < 0 || f1 < 0)
	{
		ERROR_PRINT("Negative parameter: v0 = %d, v1 = %d, f0 = %d, f1 = %d",
				v0, v1, f0, f1);
		ASSERT(!(v0 < 0 || v1 < 0 || f0 < 0 || f1 < 0));
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

	int iFoundPosition = findEdge(v0, v1);
	DEBUG_PRINT("Binary search returned the value: %d", iFoundPosition);
	if (edges[iFoundPosition].v0 == v0 && edges[iFoundPosition].v1 == v1)
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

	for (int i = numEdges; i > iFoundPosition; --i)
	{
		edges[i] = edges[i - 1];
	}
	edges[iFoundPosition].v0 = v0;
	edges[iFoundPosition].v1 = v1;
	edges[iFoundPosition].f0 = f0;
	edges[iFoundPosition].f1 = f1;
	edges[iFoundPosition].id = numEdges;
	++numEdges;
	DEBUG_PRINT("Edge (%d, %d) has been successfully added to the edge list",
			v0, v1);
	DEBUG_END;
}

int EdgeData::findEdge(int v0, int v1)
{
//	DEBUG_START;
	if (v0 > v1)
	{
		int tmp = v0;
		v0 = v1;
		v1 = tmp;
	}

	int iMin = 0, iMax = numEdges - 1, iMid;

	while (iMax >= iMin)
	{
		iMid = (iMin + iMax) / 2;

		if (edges[iMid].v0 < v0 ||
				(edges[iMid].v0 == v0 && edges[iMid].v1 < v1))
		{
			iMin = iMid + 1;
		}
		else if (edges[iMid].v0 > v0 ||
				(edges[iMid].v0 == v0 && edges[iMid].v1 > v1))
		{
			iMax = iMid - 1;
		}
		else
		{
//			DEBUG_END;
			return iMid;
		}
	}

	/* Actually, the condition of exit from the loop is:
	 * [(iMin + iMax) / 2] + 1 > iMax
	 * or
	 * [(iMin + iMax) / 2] - 1 < iMin
	 *
	 * Both these conditions can be satisfied only if iMin == iMax
	 *
	 * Thus, iMin == iMax == iMid in the last iteration of the loop.
	 * Therefore, we need to return iMin, because we will add
	 * new edge [v0, v1] before it (see function addEdge). */

//	DEBUG_END;
	return iMin;
}

