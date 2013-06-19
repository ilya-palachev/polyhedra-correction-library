/*
 * Coalescer.h
 *
 *  Created on: 19.06.2013
 *      Author: ilya
 */

#ifndef COALESCER_H_
#define COALESCER_H_

#include "PCorrector.h"

class Coalescer: public PCorrector
{
private:
	Plane plane;
	Facet coalescedFacet;

	void buildIndex(int fid0, int fid1, int& nv);
	void buildIndex(int n, int* fid, int& nv);
	void calculatePlane(int fid0, int fid1);
	void rise(int fid0);
	int riseFind(int fid0);
	double riseFindStep(int fid0, int i);
	void risePoint(int fid0, int imin);
public:
	Coalescer();
	~Coalescer();
	void run(int fid0, int fid1);
	void run(int n, int* fid);
};

#endif /* COALESCER_H_ */
