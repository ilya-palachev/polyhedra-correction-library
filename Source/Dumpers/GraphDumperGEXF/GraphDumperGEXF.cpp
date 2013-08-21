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
			DEBUG_PRINT("Adding node %s to the gexf graph.", edge->v0);
			graph->addNode(v0);
		}

		if (!graph->containsNode(v1))
		{
			DEBUG_PRINT("Adding node %s to the gexf graph.", edge->v1);
			graph->addNode(v1);
		}

		DEBUG_PRINT("Adding edge [%s, %s] to the gexf graph.", edge->v0,
				edge->v1);
		graph->addEdge(id, v0, v1);
	}
	else
	{
		DEBUG_PRINT("Edge [%s, %s] already presents in to the gexf graph.",
				edge->v0, edge->v1);
	}

	DEBUG_END;
}

void GraphDumperGEXF::collect(Facet* facet)
{
	DEBUG_START;
	DEBUG_PRINT("Collecting facet #%d to gexf graph.", facet->id);

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

		DEBUG_PRINT("Trying to add edge [%d, %d] to the gexf graph.", v0, v1);
		Edge* edge = new Edge(v0, v1);
		addEdge(edge);
		delete edge;
	}

	DEBUG_END;
}

void GraphDumperGEXF::dump(char* fileName)
{
	DEBUG_START;

	libgexf::MetaData& meta = gexf->getMetaData();
	meta.setDescription("Graph generated by GraphDumperGEXF.");

    libgexf::FileWriter *writer = new libgexf::FileWriter();
    writer->init(fileName, gexf);
    writer->write();
    delete writer;

    DEBUG_END;
}
