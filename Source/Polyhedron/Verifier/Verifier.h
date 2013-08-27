/*
 * Verifier.h
 *
 *  Created on: 21.06.2013
 *      Author: ilya
 */

#ifndef VERIFIER_H_
#define VERIFIER_H_

class Verifier
{
private:
	Polyhedron* polyhedron;
	bool ifPrint;
	EdgesWorkingSets* edgesWS;

	int countInnerConsections();
	int countInnerConsectionsFacet(int fid, double* A,
			double* b, Vector3d* vertex_old);
	int countInnerConsectionsPair(int fid, int id0, int id1,
			int id2, int id3, double* A, double* b);
	int countOuterConsections();
	int countOuterConsectionsFacet(int fid);
	int countOuterConsectionsEdge(int id0, int id1);
	int countOuterConsectionsPair(int id0, int id1, int fid);

	/* Check whether the edge is not destructed. Returns "true" if it is not
	 * destructed. */
	bool checkOneEdge(EdgeSetIterator edge, EdgeDataPtr edgeData);

	/* Reduce one edge (for case when the edge is destructed).
	 * We need to verify following structures: VertexInfo,
	 * Facet, EdgeData, Edge for correctness. */
	bool reduceEdge(EdgeSetIterator edge, EdgeDataPtr edgeData);
public:
	Verifier();
	Verifier(Polyhedron* p);
	Verifier(Polyhedron* p, bool _ifPrint);
	~Verifier();

	/* Count the number of self-consections in the polyhedron. */
	int countConsections();

	/* Count the number of destructed edges of the polyhedron. */
	int checkEdges(EdgeDataPtr edgeData);

};

#endif /* VERIFIER_H_ */
