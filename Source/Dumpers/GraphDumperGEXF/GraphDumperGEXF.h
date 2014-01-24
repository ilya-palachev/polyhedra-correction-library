/*
 * Copyright (c) 2009-2014 Ilya Palachev <iliyapalachev@gmail.com>
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
 * along with Polyhedra Correction Library.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GRAPHDUMPERGEXF_H_
#define GRAPHDUMPERGEXF_H_

#include <libgexf.h>

#include "DataContainers/EdgeData/EdgeData.h"
#include "Polyhedron/Facet/Facet.h"

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
