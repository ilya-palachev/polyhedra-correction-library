/*
 * SizeCalculator.h
 *
 *  Created on: 21.06.2013
 *      Author: ilya
 */

#ifndef SIZECALCULATOR_H_
#define SIZECALCULATOR_H_

class SizeCalculator: public PAnalyzer
{
private:

	double consection_x(double y, double z);

public:
	SizeCalculator();
	SizeCalculator(shared_ptr<Polyhedron> p);
	~SizeCalculator();

	double calculate_J11(int N);

	double volume();
	double areaOfSurface();
	double areaOfFacet(int iFacet);
	void J(double& Jxx, double& Jyy, double& Jzz, double& Jxy, double& Jyz,
			double& Jxz);
	void get_center(double& xc, double& yc, double& zc);
	void inertia(double& l0, double& l1, double& l2, Vector3d& v0, Vector3d& v1,
			Vector3d& v2);
};

#endif /* SIZECALCULATOR_H_ */
