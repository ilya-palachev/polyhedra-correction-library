/*
 * GlobalShadeCorrector.h
 *
 *  Created on: 12.05.2013
 *      Author: iliya
 */

#ifndef GLOBALSHADECORRECTOR_H_
#define GLOBALSHADECORRECTOR_H_

#define DO_EDGES_CHECK

enum MethodCorrector
{
	METHOD_UNKNOWN, METHOD_GRADIENT_DESCENT, METHOD_GRADIENT_DESCENT_FAST,
	METHOD_CONJUGATE_GRADIENT, METHOD_ALL
};
/* Number of different methods of the corrector. */
const int NUMBER_METHOD_CORRECTOR = 3;

struct _GSCorrectorParameters
{
	MethodCorrector methodName;
	double epsLoopStop;
	double deltaGradientStep;
};
typedef struct _GSCorrectorParameters GSCorrectorParameters;

enum GSCorrectorExitReason
{
	GSC_SUCCESS, GSC_BIG_ERROR_ABSOLUTE, GSC_BIG_ERROR_RELATIVE,
	GSC_SMALL_GRADIENT, GSC_SMALL_MOVEMENT
};

struct _GSCorrectorStatus
{
	GSCorrectorParameters parameters;
	int numIterations;
	GSCorrectorExitReason exitReason;
	double valueCausedExit;
};
typedef struct _GSCorrectorStatus GSCorrectorStatus;


const double EPS_LOOP_STOP_DEFAULT = 1e-6;
const double DELTA_GRADIENT_STEP_DEFAULT = 1e-4;
const MethodCorrector METHOD_CORRECTOR_DEFAULT = METHOD_UNKNOWN;
const double MAX_ERROR_ABSOLUTE = 10.;
const double MAX_ERROR_RELATIVE = 10.;
const double EPSILON_MINIMAL_GRADIENT_NORM = 1e-16;
const double EPSILON_MINIMAL_MOVEMENT = 1e-32;

const double DEFAULT_MAX_DELTA = 1.;

const double EPSILON_FOR_DIVISION = 1e-16;

class GlobalShadeCorrector: public PCorrector
{
protected:
	EdgeData* edgeData;
	ShadeContourData* contourData;

private:

	/* Basic parameters of algorithm. */
	GSCorrectorParameters parameters;

	/* In all algorithms we process only those facets which have some
	 * associations with contours. This list is used to store
	 * not-associated facets. */
	list<int>* facetsNotAssociated;

	/* Array for storing current values of gradient - used in all gradient
	 * methods. */
	double* gradient;

	/* Array for storing previous values of S_{k}^{j} - used in conjugate
	 * gradient method only. */
	double* gradientPrevious;

	/* Euclid norm of current and previous gradient - used in conjugate
	 * gradient method only. */
	double gradientNorm;
	double gradientNormPrevious;

	/* Number of currently minimized dimension in cojugate gradient method
	 * ("j" variable from Wikipedia article): */
	int iMinimizedDimension;

	/* Level of minimization
	 * ("k" variable from Wikipedia article): */
	int iMinimizationLevel;

	/* Array for storing previous values of planes' coefficients.
	 * This is used in trick when we freeze some rational expressions
	 * to make functional quadratical. */
	Plane* prevPlanes;

	/* Number of dimensions of space where the functional is minimized. */
	int dim;

	void preprocess();
	void preprocessAssociations();

	void findNotAssociatedFacets();

	double calculateFunctional();

	void shiftCoefficients(double delta);
	double calculateFunctional(double delta);
	double findOptimalDelta(double deltaMax);

	void runCorrectionIteration();

	void calculateGradient();
	double calculateGradientNorm();

	void derivativeTest_all();
	void derivativeTest_1();
	double calculateFunctionalDerivative_1(int iFacet, int iCoefficient);

	GSCorrectorStatus repairAndRun(MethodCorrector method,
			Plane* planesInitial);
	GSCorrectorStatus runCorrectionDo();
public:
	GlobalShadeCorrector();
	GlobalShadeCorrector(Polyhedron* p, ShadeContourData* scd,
		GSCorrectorParameters* _parameters);
	virtual ~GlobalShadeCorrector();

	void runCorrection();
};

#endif /* GLOBALSHADECORRECTOR_H_ */
