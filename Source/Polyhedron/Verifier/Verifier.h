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

	int countInnerConsections();
	int countInnerConsectionsFacet(int fid, double* A,
			double* b, Vector3d* vertex_old);
	int countInnerConsectionsPair(int fid, int id0, int id1,
			int id2, int id3, double* A, double* b);
	int countOuterConsections();
	int countOuterConsectionsFacet(int fid);
	int countOuterConsectionsEdge(int id0, int id1);
	int countOuterConsectionsPair(int id0, int id1, int fid);

	bool checkOneEdge(Edge* edge);
public:
	Verifier();
	Verifier(Polyhedron* p);
	Verifier(Polyhedron* p, bool _ifPrint);
	~Verifier();
	int countConsections();
	int checkEdges(EdgeData* edgeData);

};

#endif /* VERIFIER_H_ */
