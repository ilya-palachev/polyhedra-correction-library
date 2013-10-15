/* 
 * Copyright (c) 2009-2013 Ilya Palachev <iliyapalachev@gmail.com>
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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "PolyhedraCorrectionLibrary.h"

EdgeData::EdgeData() :
				edges(),
				numEdges(0)
{
	DEBUG_START;
	DEBUG_END;
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
	DEBUG_START;
	if (numEdges != e.numEdges)
	{
		DEBUG_END;
		return false;
	}

	EdgeSetIterator edgeSelf = edges.begin();
	EdgeSetIterator edgeOther = e.edges.begin();

	for (int iEdge = 0; iEdge < numEdges; ++iEdge)
	{
		if (*edgeSelf != *edgeOther)
		{
			DEBUG_END;
			return false;
		}
		++edgeSelf;
		++edgeOther;
	}

	DEBUG_END;
	return true;
}

bool EdgeData::operator !=(EdgeData& e)
{
	DEBUG_START;
	DEBUG_END;
	return !(*this == e);
}


pair<EdgeSetIterator, bool> EdgeData::addEdge(int v0, int v1, int f0)
{
	DEBUG_START;
	DEBUG_PRINT("Trying to add edge [%d, %d] to the edge list", v0, v1);
	pair<EdgeSetIterator, bool> returnValue;

	if (v0 < 0 || v1 < 0 || f0 < 0)
	{
		ERROR_PRINT("Negative parameter: v0 = %d, v1 = %d, f0 = %d",
				v0, v1, f0);
		ASSERT(!(v0 < 0 || v1 < 0 || f0 < 0));
		returnValue = pair<EdgeSetIterator, bool> (edges.end(), false);
		DEBUG_END;
		return returnValue;
	}
	if (v0 > v1)
	{
		int tmp = v0;
		v0 = v1;
		v1 = tmp;
	}

	EdgeSetIterator edgeFound = findEdge(v0, v1);
	if (edgeFound != edges.end())
	{
		if (edgeFound->f0 == INT_NOT_INITIALIZED
				&& edgeFound->f1 == INT_NOT_INITIALIZED)
		{
			DEBUG_PRINT("Edge [%d, %d] already presents in the list,"
					"facet values are empty, initializing value #0", v0, v1);
			edgeFound->f0 = f0;
		}
		else if (edgeFound->f0 != INT_NOT_INITIALIZED
				&& edgeFound->f1 == INT_NOT_INITIALIZED)
		{
			DEBUG_PRINT("Edge [%d, %d] already presents in the list,"
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
		}
		else if (edgeFound->f0 == INT_NOT_INITIALIZED
				&& edgeFound->f1 != INT_NOT_INITIALIZED)
		{
			DEBUG_PRINT("Edge [%d, %d] already presents in the list,"
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
		}
		else if (edgeFound->f0 != INT_NOT_INITIALIZED
				&& edgeFound->f1 != INT_NOT_INITIALIZED)
		{
			DEBUG_PRINT("Edge [%d, %d] already presents in the list,"
					"facet values are non-empty, skipping...", v0, v1);
		}
		returnValue = pair<EdgeSetIterator, bool> (edgeFound, false);
	}
	else
	{
		returnValue = edges.insert(Edge(v0, v1, f0, INT_NOT_INITIALIZED));

		/* If we have added new edge to the set, set its id to the number of edges
		 * in the edge data. */
		if (returnValue.second)
		{
			returnValue.first->id = numEdges;
		}
	}
	numEdges = edges.size();

	DEBUG_PRINT("Edge [%d, %d] has been successfully added to the edgeFound list",
			v0, v1);
	DEBUG_END;
	return returnValue;
}

pair<EdgeSetIterator, bool> EdgeData::addEdge(int v0, int v1, int f0, int f1)
{
	DEBUG_START;
	DEBUG_PRINT("Trying to add edge [%d, %d] to the edge list", v0, v1);
	pair<EdgeSetIterator, bool> returnValue;

	DEBUG_PRINT("\tnumEdges = %d", numEdges);
	if (v0 < 0 || v1 < 0 || f0 < 0 || f1 < 0)
	{
		ERROR_PRINT("Negative parameter: v0 = %d, v1 = %d, f0 = %d, f1 = %d",
				v0, v1, f0, f1);
		ASSERT(!(v0 < 0 || v1 < 0 || f0 < 0 || f1 < 0));
		returnValue = pair<EdgeSetIterator, bool> (edges.end(), false);
		return returnValue;
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

	returnValue = edges.insert(Edge(v0, v1, f0, f1));
	DEBUG_PRINT("Edge [%d, %d] has been successfully added to "
					"the edge list", v0, v1);
	numEdges = edges.size();

	/* If we have added new edge to the set, set its id to the number of edges
	 * in the edge data. */
	if (returnValue.second)
	{
		returnValue.first->id = numEdges;
	}

	DEBUG_END;
	return returnValue;
}

pair<EdgeSetIterator, bool> EdgeData::addEdge(Edge& edge)
{
	DEBUG_START;
	pair<EdgeSetIterator, bool> returnValue = addEdge(edge.v0, edge.v1,
			edge.f0, edge.f1);

	/* Append associations to the edge in set. */
	EdgeSetIterator edgeInSet = returnValue.first;
	edgeInSet->assocList.insert(edgeInSet->assocList.end(),
			edge.assocList.begin(), edge.assocList.end());

	return returnValue;
	DEBUG_END;
}

EdgeSetIterator EdgeData::findEdge(int v0, int v1)
{
	DEBUG_START;
	if (v0 > v1)
	{
		int tmp = v0;
		v0 = v1;
		v1 = tmp;
	}

	Edge* edgeTmp = new Edge(v0, v1);
	EdgeSetIterator itEdge = edges.find(*edgeTmp);
	DEBUG_PRINT("Found edge: [%d, %d]", itEdge->v0, itEdge->v1);
	delete edgeTmp;

	DEBUG_END;
	return itEdge;
}

