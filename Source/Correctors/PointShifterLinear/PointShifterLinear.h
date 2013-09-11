/*
 * PointShifterLinear.h
 *
 *  Created on: 20.06.2013
 *      Author: ilya
 */

#ifndef POINTSHIFTERLINEAR_H_
#define POINTSHIFTERLINEAR_H_

class PointShifterLinear: public PCorrector
{
private:
	/* The ID of shifted vertex */
	int id;

	/* Auxiliary arrays (for calculating movements of facets) */
	double* x;
	double* y;
	double* z;

	/* Auxiliary arrays (for saving initial values) */
	double* xold;
	double* yold;
	double* zold;

	/* Auxiliary arrays (for calculatind movements of vertices) */
	double* A;
	double* B;

	/* Coefficient */
	double K;


	double calculateError();
	double calculateDeform();
	void moveFacets();
	void moveVerticesGlobal();
	void moveVerticesLocal();
public:
	PointShifterLinear();
	PointShifterLinear(shared_ptr<Polyhedron> p);
	PointShifterLinear(Polyhedron* p);
	~PointShifterLinear();

	void runGlobal(int id, Vector3d delta);
	void runLocal(int id, Vector3d delta);
	void runTest(int id, Vector3d delta, int mode,
		int& num_steps, double& norm_sum);
	void runPartial(int id, Vector3d delta, int num);
};

#endif /* POINTSHIFTERLINEAR_H_ */
