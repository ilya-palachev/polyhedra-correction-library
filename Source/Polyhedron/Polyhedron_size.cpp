#include "PolyhedraCorrectionLibrary.h"

double Polyhedron::calculate_J11(int N)
{
	SizeCalculator* sizeCalculator = new SizeCalculator(this);
	double ret = sizeCalculator->calculate_J11(N);
	delete sizeCalculator;
	return ret;
}

double Polyhedron::volume()
{
	SizeCalculator* sizeCalculator = new SizeCalculator(this);
	double ret = sizeCalculator->volume();
	delete sizeCalculator;
	return ret;
}

double Polyhedron::areaOfSurface()
{
	SizeCalculator* sizeCalculator = new SizeCalculator(this);
	double ret = sizeCalculator->areaOfSurface();
	delete sizeCalculator;
	return ret;
}

double Polyhedron::areaOfFacet(int iFacet)
{
	SizeCalculator* sizeCalculator = new SizeCalculator(this);
	double ret = sizeCalculator->areaOfFacet(iFacet);
	delete sizeCalculator;
	return ret;
}

void Polyhedron::J(double& Jxx, double& Jyy, double& Jzz, double& Jxy, double& Jyz,
			double& Jxz)
{
	SizeCalculator* sizeCalculator = new SizeCalculator(this);
	sizeCalculator->J(Jxx, Jyy, Jzz, Jxy, Jyz, Jxz);
	delete sizeCalculator;
}

void Polyhedron::get_center(double& xc, double& yc, double& zc)
{
	SizeCalculator* sizeCalculator = new SizeCalculator(this);
	sizeCalculator->get_center(xc, yc, zc);
	delete sizeCalculator;
}

void Polyhedron::inertia(double& l0, double& l1, double& l2, Vector3d& v0, Vector3d& v1,
			Vector3d& v2)
{
	SizeCalculator* sizeCalculator = new SizeCalculator(this);
	sizeCalculator->inertia(l0, l1, l2, v0, v1, v2);
	delete sizeCalculator;
}
