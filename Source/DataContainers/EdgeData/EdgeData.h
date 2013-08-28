/*
 * EdgeData.h
 *
 *  Created on: 17.05.2013
 *      Author: iliya
 */

#ifndef EDGEDATA_H_
#define EDGEDATA_H_

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
