/*
 * GraphDumperGEXF.h
 *
 *  Created on: Aug 19, 2013
 *      Author: ilya
 */

#ifndef GRAPHDUMPERGEXF_H_
#define GRAPHDUMPERGEXF_H_

/* Class for dumping graph data obtained from facet/edge strcuture to file.
 * It should be used for debugging purposes. We need to see what
 * has been done by the algorithm with current facet.
 *
 * The dumper is based on LibGEXF - open library for GEXF format.
 * See http://www.gexf.net/lib/ for details.
 *
 * This format has been chosen because it's used by Gephi visualizer.
 * In future we will need to have an alternative dumper.
 * */

class GraphDumperGEXF
{
private:
	/* General gexf object. */
	libgexf::GEXF *gexf;

	/* We store all data in undirected graph. */
	libgexf::UndirectedGraph *graph;

	/* Test whether the edge has already been added to the graph and add it if
	 * not. It's assumed that edge->v0 < edge->v1 .*/
	void addEdge(Edge* edge);

public:

	/* Constructor that initializes gexf and created undirected graph for
	 * storing edges. */
	GraphDumperGEXF();

	/* Destructor that finalizes gexf. */
	~GraphDumperGEXF();

	/* Traverse through all the edges inside the facet and add all them to the
	 * graph. */
	void collect(Facet* facet);

	/* Dump already collected graph data to the file. */
	void dump(const char* fileName);
};

#endif /* GRAPHDUMPERGEXF_H_ */
