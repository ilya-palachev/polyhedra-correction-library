/*
 * GlobalShadeCorrector.h
 *
 *  Created on: 12.05.2013
 *      Author: iliya
 */

#ifndef GLOBALSHADECORRECTOR_H_
#define GLOBALSHADECORRECTOR_H_

/*
 *
 */
struct _GSCorrectorParameters
{
	double epsLoopStop;
	double deltaGradientStep;
};

typedef struct _GSCorrectorParameters GSCorrectorParameters;

const double EPS_LOOP_STOP_DEFAULT = 1e-6;
const double DELTA_GRADIENT_STEP_DEFAULT = 1e-4;

class GlobalShadeCorrector: public PCorrector
{
protected:
	EdgeData* edgeData;
	ShadeContourData* contourData;

private:
	GSCorrectorParameters parameters;

	list<int>* facetsNotAssociated;

	double* gradient;
	Plane* prevPlanes;

	int dim;

	void preprocess();
	void preprocessAssociations();

	void findNotAssociatedFacets();

	double calculateFunctional();

	void shiftCoefficients(double delta);
	double calculateFunctional(double delta);

	void runCorrectionIteration();

	void calculateGradient();

	void derivativeTest_all();
	void derivativeTest_1();
	double calculateFunctionalDerivative_1(int iFacet, int iCoefficient);

public:
	GlobalShadeCorrector();
	GlobalShadeCorrector(Polyhedron* p, ShadeContourData* scd,
		GSCorrectorParameters* _parameters);
	GlobalShadeCorrector(Polyhedron* input);
	virtual ~GlobalShadeCorrector();

	void runCorrection();
};

#endif /* GLOBALSHADECORRECTOR_H_ */
