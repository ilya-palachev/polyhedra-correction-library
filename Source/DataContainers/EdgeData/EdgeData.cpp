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
}

EdgeData::~EdgeData()
{
	if (edges != NULL)
	{
		delete[] edges;
		edges = NULL;
	}
}

void EdgeData::addEdge(int numEdgesMax, int v0, int v1, int f0, int f1)
{
	DEBUG_START;
	DEBUG_PRINT("Trying to add edge (%d, %d) to the edge list", v0, v1);

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

