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

	int test_inner_consections();
	int test_inner_consections_facet(int fid, double* A,
			double* b, Vector3d* vertex_old);
	int test_inner_consections_pair(int fid, int id0, int id1,
			int id2, int id3, double* A, double* b);
	int test_outer_consections();
	int test_outer_consections_facet(int fid);
	int test_outer_consections_edge(int id0, int id1);
	int test_outer_consections_pair(int id0, int id1, int fid);
public:
	Verifier();
	Verifier(Polyhedron* p, bool _ifPrint);
	~Verifier();
	int test_consections();

};

#endif /* VERIFIER_H_ */
