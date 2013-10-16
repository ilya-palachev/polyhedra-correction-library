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
 * along with Polyhedra Correction Library.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef EDGEDATA_H_
#define EDGEDATA_H_

#include <set>

#include "DataContainers/EdgeData/Edge/Edge.h"

typedef set<Edge, EdgeComparison> EdgeSet;
typedef set<Edge, EdgeComparison>::iterator EdgeSetIterator;

class EdgeData
{
public:
	EdgeSet edges;
	int numEdges;

	EdgeData();
	~EdgeData();

	/* Try to find the edge in the edge data.
	 *
	 * Note:
	 * If v0 > v1, then the edge [v1, v0] is searched for. */
	EdgeSetIterator findEdge(int v0, int v1);

	/* Add edge [v0, v1] with information about incident facets f0 and f1 to
	 * the edge data.
	 *
	 * Note:
	 * If v0 > v1, then the edge [v1, v0] is added.
	 * If f0 > f1, then facets are added in order f1, f0.
	 *
	 * Return value:
	 * A pair, with its member pair::first set to an iterator pointing
	 * to either the newly inserted element or to the equivalent element
	 * already in the set. The pair::second element in the pair is set to true
	 * if a new element was inserted or false if an equivalent element already
	 * existed (see c++ reference).
	 * */
	pair<EdgeSetIterator, bool> addEdge(int v0, int v1, int f0, int f1);

	/* Add edge [v0, v1] with information about one incident facet f0 to the
	 * edge data.
	 *
	 * Note:
	 * If v0 > v1, then the edge [v1, v0] is added.
	 * If edge already exists, then information about facet f0 is added (if the
	 * information about f0 is not contained in the edge).
	 *
	 * Return value:
	 * A pair, with its member pair::first set to an iterator pointing
	 * to either the newly inserted element or to the equivalent element
	 * already in the set. The pair::second element in the pair is set to true
	 * if a new element was inserted or false if an equivalent element already
	 * existed (see c++ reference).
	 * */
	pair<EdgeSetIterator, bool> addEdge(int v0, int v1, int f0);

	/* Add edge with information about 2 incident facets and found
	 * associations.
	 *
	 * Note:
	 * If v0 > v1, then the edge [v1, v0] is added.
	 * If f0 > f1, then facets are added in order f1, f0.
	 *
	 * Return value:
	 * A pair, with its member pair::first set to an iterator pointing
	 * to either the newly inserted element or to the equivalent element
	 * already in the set. The pair::second element in the pair is set to true
	 * if a new element was inserted or false if an equivalent element already
	 * existed (see c++ reference).
	 * */
	pair<EdgeSetIterator, bool> addEdge(Edge& edge);

	bool operator ==(EdgeData& e);
	bool operator !=(EdgeData& e);

};

#endif /* EDGEDATA_H_ */
