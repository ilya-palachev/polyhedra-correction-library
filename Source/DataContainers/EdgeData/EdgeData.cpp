/*
 * EdgeData.cpp
 *
 *  Created on: 17.05.2013
 *      Author: iliya
 */

#include "PolyhedraCorrectionLibrary.h"

EdgeData::EdgeData() :
				edges(),
				numEdges(0)
{
}

EdgeData::EdgeData(int numEdgesMax) :
				edges(),
				numEdges(0)
{
	for (int iEdge = 0; iEdge < numEdgesMax; ++iEdge)
	{
		edges.push_back(Edge(INT_NOT_INITIALIZED, INT_NOT_INITIALIZED,
				INT_NOT_INITIALIZED, INT_NOT_INITIALIZED,
				INT_NOT_INITIALIZED));
	}
}

EdgeData::~EdgeData()
{
	DEBUG_START;
	DEBUG_PRINT(COLOUR_RED "Attention!!! edgeData is being deleted now!"
			COLOUR_NORM);
	edges.clear();
	DEBUG_END;
}

bool EdgeData::operator ==(EdgeData& e)
{
	if (numEdges != e.numEdges)
	{
		return false;
	}

	list<Edge>::iterator edgeSelf = edges.begin();
	list<Edge>::iterator edgeOther = e.edges.begin();

	for (int iEdge = 0; iEdge < numEdges; ++iEdge)
	{
		if (*edgeSelf != *edgeOther)
		{
			return false;
		}
		++edgeSelf;
		++edgeOther;
	}

	return true;
}

bool EdgeData::operator !=(EdgeData& e)
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

	list<Edge>::iterator edgeFound = findEdge(v0, v1);
	if (edgeFound->v0 == v0 && edgeFound->v1 == v1)
	{
		if (edgeFound->f0 == INT_NOT_INITIALIZED
				&& edgeFound->f1 == INT_NOT_INITIALIZED)
		{
			DEBUG_PRINT("Edge (%d, %d) already presents in the list,"
					"facet values are empty, initializing value #0", v0, v1);
			edgeFound->f0 = f0;
			DEBUG_END;
			return;
		}

		if (edgeFound->f0 != INT_NOT_INITIALIZED
				&& edgeFound->f1 == INT_NOT_INITIALIZED)
		{
			DEBUG_PRINT("Edge (%d, %d) already presents in the list,"
					"facet value #1 is empty, initializing value #1", v0, v1);
			if (edgeFound->f0 < f0)
			{
				edgeFound->f1 = f0;
			}
			else
			{
				edgeFound->f1 = edgeFound->f0;
				edgeFound->f0 = f0;
			}
			DEBUG_END;
			return;
		}

		if (edgeFound->f0 == INT_NOT_INITIALIZED
				&& edgeFound->f1 != INT_NOT_INITIALIZED)
		{
			DEBUG_PRINT("Edge (%d, %d) already presents in the list,"
					"facet value #0 is empty, initializing value #0", v0, v1);
			if (edgeFound->f1 < f0)
			{
				edgeFound->f0 = edgeFound->f1;
				edgeFound->f1 = f0;
			}
			else
			{
				edgeFound->f0 = f0;
			}
			DEBUG_END;
			return;
		}

		if (edgeFound->f0 != INT_NOT_INITIALIZED
				&& edgeFound->f1 != INT_NOT_INITIALIZED)
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

	edges.insert(edgeFound, Edge(v0, v1, f0, INT_NOT_INITIALIZED));
	++numEdges;
	DEBUG_PRINT("Edge (%d, %d) has been successfully added to the edgeFound list",
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

	list<Edge>::iterator edgeFound = findEdge(v0, v1);
	if (edgeFound->v0 == v0 && edgeFound->v1 == v1)
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

	edges.insert(edgeFound, Edge(v0, v1, f0, f1));
	++numEdges;
	DEBUG_PRINT("Edge (%d, %d) has been successfully added to the edge list",
			v0, v1);
	DEBUG_END;
}

list<Edge>::iterator EdgeData::findEdge(int v0, int v1)
{
	if (v0 > v1)
	{
		int tmp = v0;
		v0 = v1;
		v1 = tmp;
	}

	list<Edge>::iterator itEdge = std::find(edges.begin(), edges.end(),
			Edge(v0, v1));
	return itEdge;
}

