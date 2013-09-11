/*
 * PointShifterWeighted.h
 *
 *  Created on: 20.06.2013
 *      Author: ilya
 */

#ifndef POINTSHIFTERWEIGHTED_H_
#define POINTSHIFTERWEIGHTED_H_

class PointShifterWeighted: public PCorrector
{
private:
	int n;
	double* x;
	double* x1;
	double* fx;
	double* A;
	int id;
	bool* khi;
	double* tmp0;
	double* tmp1;
	double* tmp2;
	double* tmp3;
	double K;

	void calculateFunctional();
	void calculateFuncitonalDerivative();

	void init();
public:
	PointShifterWeighted();
	PointShifterWeighted(shared_ptr<Polyhedron> p);
	PointShifterWeighted(Polyhedron* p);
	~PointShifterWeighted();

	void run(int id, Vector3d delta);
};

#endif /* POINTSHIFTERWEIGHTED_H_ */
