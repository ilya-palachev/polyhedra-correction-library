/*
 * GraphDumperGEXF.cpp
 *
 *  Created on: Aug 19, 2013
 *      Author: ilya
 */

#include "PolyhedraCorrectionLibrary.h"

GraphDumperGEXF::GraphDumperGEXF() :
		gexf(new libgexf::GEXF()),
		graph(&gexf->getUndirectedGraph())
{
	DEBUG_START;
	DEBUG_END;
}

GraphDumperGEXF::~GraphDumperGEXF()
{
	DEBUG_START;
	if (gexf != NULL)
	{
		delete gexf;
		gexf = NULL;
	}
	DEBUG_END;
}


void GraphDumperGEXF::addEdge(Edge* edge)
{
	DEBUG_START;

	/* Assumption. edge->v0 < edge->v1.
	 * This is provided by the structure of edge data. */

	ASSERT(edge->v0 < edge->v1);

	string v0 = to_string(edge->v0);
	string v1 = to_string(edge->v1);
	string id = to_string(graph->getNodeCount());

	if (!graph->containsEdge(v0, v1))
	{
		if (!graph->containsNode(v0))
		{
			graph->addNode(v0);
		}

		if (!graph->containsNode(v1))
		{
			graph->addNode(v1);
		}

		graph->addEdge(id, v0, v1);
	}

	DEBUG_END;
}

void GraphDumperGEXF::collect(Facet* facet)
{
	DEBUG_START;

	for (int iVertex = 0; iVertex < facet->numVertices; ++iVertex)
	{
		int v0 = facet->indVertices[iVertex];
		int v1 = facet->indVertices[iVertex + 1];

		if (v0 > v1)
		{
			int tmp = v0;
			v0 = v1;
			v1 = tmp;
		}

		Edge* edge = new Edge(v0, v1);
		addEdge(edge);
		delete edge;
	}

	DEBUG_END;
}
