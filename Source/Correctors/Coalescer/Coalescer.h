/*
 * Coalescer.h
 *
 *  Created on: 19.06.2013
 *      Author: ilya
 */

#ifndef COALESCER_H_
#define COALESCER_H_

class Coalescer: public PCorrector
{
private:
	Plane plane;
	Facet coalescedFacet;

	int buildIndex(int fid0, int fid1);
	int buildIndex(int n, int* fid);
	void calculatePlane(int fid0, int fid1);
	void calculatePlane(int n, int* fid);
	void rise(int fid0);
	int riseFind(int fid0);
	double riseFindStep(int fid0, int i);
	void risePoint(int fid0, int imin);
	void deleteVertexInPolyhedron(int v);
	int appendVertex(Vector3d& vec);
	void leastSquaresMethod(int nv, int* vertex_list, Plane* plane);
	void leastSquaresMethodWeighted(int nv, int* vertex_list, Plane* plane);
public:
	Coalescer();
	Coalescer(Polyhedron* p);
	~Coalescer();
	void run(int fid0, int fid1);
	void run(int n, int* fid);
};

#endif /* COALESCER_H_ */
