/*
 * PointShifter.h
 *
 *  Created on: 20.06.2013
 *      Author: ilya
 */

#ifndef POINTSHIFTER_H_
#define POINTSHIFTER_H_

class PointShifter: public PCorrector
{
private:
	int n;
	double* x;
	double* x1;
	double* fx;
	double* A;
	int id;
	int* sum;
	double* tmp0;
	double* tmp1;
	double* tmp2;
	double* tmp3;

	void f();
	void derf();
	void derf2();

public:
	PointShifter();
	PointShifter(Polyhedron* p);
	~PointShifter();

	void run(int id, Vector3d delta);
};

#endif /* POINTSHIFTER_H_ */
