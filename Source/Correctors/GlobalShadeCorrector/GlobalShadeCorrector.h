/*
 * Copyright (c) 2009-2013 Ilya Palachev <iliyapalachev@gmail.com>
 *
 * This file is part of Polyhedra Correction Library.
 *
 * Polyhedra Correction Library is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Polyhedra Correction Library is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Polyhedra Correction Library.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GLOBALSHADECORRECTOR_H_
#define GLOBALSHADECORRECTOR_H_

#include <list>

#include "Correctors/PCorrector/PCorrector.h"
#include "DataContainers/ShadeContourData/ShadeContourData.h"

class Plane;
class GSAssociator;

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
	EdgeDataPtr edgeData;
	ShadeContourDataPtr contourData;

private:

	/* Basic parameters of algorithm. */
	GSCorrectorParameters parameters;

	/* For opportunity of partial correction, we store a list with facet id's
	 * that are to be corrected. */
	list<int> facetsCorrected;

	/* In all algorithms we process only those facets which have some
	 * associations with contours. This list is used to store
	 * not-associated facets. */
	list<int> facetsNotAssociated;

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

	/* Object of type GSAssociator is needed here because it's deletion causes
	 * the deletion of its parent object, i. e. GlobalShadeCorrector. This
	 * deletes the list facetNotAssociated.
	 *
	 * This issue has been found by the use of Valgrind (Memcheck) tool. */
	GSAssociator* associator;

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

	void init();
public:
	GlobalShadeCorrector();
	GlobalShadeCorrector(shared_ptr<Polyhedron> p, ShadeContourDataPtr scd,
		GSCorrectorParameters* _parameters);
	GlobalShadeCorrector(Polyhedron* p, ShadeContourDataPtr scd,
				GSCorrectorParameters* _parameters);
	virtual ~GlobalShadeCorrector();
	void setFacetsCorrected(list<int> _facetsCorrected);

	void runCorrection();

};

#endif /* GLOBALSHADECORRECTOR_H_ */
