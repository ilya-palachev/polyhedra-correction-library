/*
 * GlobalShadeCorrector_derivateTest.cpp
 *
 *  Created on: 16.05.2013
 *      Author: iliya
 */

#include "PolyhedraCorrectionLibrary.h"

const double EPSILON_FOR_WARNING_IN_DERIVATIVE_TESTING_RELATIVE = 0.5;
const double EPSILON_FOR_WARNING_IN_DERIVATIVE_TESTING_ABSOLUTE = 1e-2;
const double MINIMAL_TRUSTED_ERROR = 1e-6;
const double DEFAULT_DERIVATIVE_STEP = 1e-8;
const double DEFAULT_DERIVATIVE_STEP_RECIPROCAL = 1e+8;

void GlobalShadeCorrector::derivativeTest_all()
{
	DEBUG_START;
	derivativeTest_1();
	DEBUG_END;
}

void GlobalShadeCorrector::derivativeTest_1()
{
	DEBUG_START;
	list<int>::iterator iIteratorNotAssociated = facetsNotAssociated.begin();
	int iCountNotAssociated = 0;

	int iFacetLocal = 0;
	for (list<int>::iterator itFacet = facetsCorrected.begin();
			itFacet != facetsCorrected.end(); ++itFacet, ++iFacetLocal)
	{
		int iFacet = *itFacet;

		if (iFacet == *iIteratorNotAssociated)
		{
			++iIteratorNotAssociated;
			++iCountNotAssociated;
			continue;
		}
		int iFacetShifted = iFacetLocal - iCountNotAssociated;

		for (int iCoefficient = 0; iCoefficient < 4; ++iCoefficient)
		{
			double valueFromDerTest = calculateFunctionalDerivative_1(iFacet,
					iCoefficient);
			double valueFromGradient =
					gradient[4 * iFacetShifted + iCoefficient];

			bool makeAssertion = false;

			DEBUG_PRINT(
					"value from derivative test: %le, value from gradient: %le",
					valueFromDerTest, valueFromGradient);

			double errorAbsolute = fabs(valueFromDerTest - valueFromGradient);
			if (errorAbsolute
					> EPSILON_FOR_WARNING_IN_DERIVATIVE_TESTING_ABSOLUTE)
			{
				ERROR_PRINT("!!! Too big absolute error: %le", errorAbsolute);
				ERROR_PRINT(" iFacet = %d, iCoefficient = %d", iFacet,
						iCoefficient);
				makeAssertion = true;
			}

			double absValue = fabs(valueFromDerTest);
			if (absValue <= EPSILON_FOR_DIVISION)
				// In this case we cannot compare values
				continue;

			double errorRelative = errorAbsolute / absValue;
			if (errorAbsolute >= MINIMAL_TRUSTED_ERROR
					&& errorRelative
					> EPSILON_FOR_WARNING_IN_DERIVATIVE_TESTING_RELATIVE)
			{
				ERROR_PRINT("!!! Too big relative error: %lf", errorRelative);
				ERROR_PRINT(" iFacet = %d, iCoefficient = %d", iFacet,
						iCoefficient);
				makeAssertion = true;
			}
			ASSERT(!makeAssertion);
		}
	}
	DEBUG_END;
}

double GlobalShadeCorrector::calculateFunctionalDerivative_1(int iFacet,
		int iCoefficient)
{
	DEBUG_START;
	if (iFacet < 0 || iFacet >= polyhedron->numFacets)
	{
		ERROR_PRINT("Error. iFacet = %d is out of bounds", iFacet);
		DEBUG_END;
		return DEFAULT_ERROR_FOR_DOUBLE_FUNCTIONS;
	}

	if (iCoefficient < 0 || iCoefficient >= 4)
	{
		ERROR_PRINT("Error. iCoefficient = %d is out of bounds", iCoefficient);
		DEBUG_END;
		return DEFAULT_ERROR_FOR_DOUBLE_FUNCTIONS;
	}

	double* changedValue;
	switch (iCoefficient)
	{
	case 0:
		changedValue = &(polyhedron->facets[iFacet].plane.norm.x);
		break;
	case 1:
		changedValue = &(polyhedron->facets[iFacet].plane.norm.y);
		break;
	case 2:
		changedValue = &(polyhedron->facets[iFacet].plane.norm.z);
		break;
	case 3:
		changedValue = &(polyhedron->facets[iFacet].plane.dist);
		break;
	}
	double changedValuePrev = *changedValue;
	*changedValue = changedValuePrev + DEFAULT_DERIVATIVE_STEP;
	// We multiply functional value with 0.5 because in matrix
	// each element is multiplied by 0.5
	double funcValueRight = 0.5 * calculateFunctional();
	*changedValue = changedValuePrev - DEFAULT_DERIVATIVE_STEP;
	double funcValueLeft = 0.5 * calculateFunctional();
	*changedValue = changedValuePrev;

	DEBUG_END;
	return 0.5 * DEFAULT_DERIVATIVE_STEP_RECIPROCAL
			* (funcValueRight - funcValueLeft);
}
