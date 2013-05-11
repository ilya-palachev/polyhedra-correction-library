#include "PolyhedraCorrectionLibrary.h"

void Polyhedron::corpol_derivativeTest_all(Plane* prevPlanes, double* gradient,
		list<int>* facetsNotAssociated)
{
	corpol_derivativeTest_1(prevPlanes, gradient, facetsNotAssociated);
}

void Polyhedron::corpol_derivativeTest_1(Plane* prevPlanes, double* gradient,
		list<int>* facetsNotAssociated)
{
	list<int>::iterator iIteratorNotAssociated = facetsNotAssociated->begin();
	int iCountNotAssociated = 0;

	for (int iFacet = 0; iFacet < numFacets; ++iFacet)
	{
		if (iFacet == *iIteratorNotAssociated)
		{
			++iIteratorNotAssociated;
			++iCountNotAssociated;
			continue;
		}
		int iFacetShifted = iFacet - iCountNotAssociated;

		for (int iCoefficient = 0; iCoefficient < 4; ++iCoefficient)
		{
			double valueFromDerTest = corpol_calculate_functional_derivative_1(
					prevPlanes, iFacet, iCoefficient);
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
			if (errorRelative
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
}

double Polyhedron::corpol_calculate_functional_derivative_1(Plane* prevPlanes,
		int iFacet, int iCoefficient)
{
	if (iFacet < 0 || iFacet >= numFacets)
	{
		ERROR_PRINT("Error. iFacet = %d is out of bounds", iFacet);
		return DEFAULT_ERROR_FOR_DOUBLE_FUNCTIONS;
	}

	if (iCoefficient < 0 || iCoefficient >= 4)
	{
		ERROR_PRINT("Error. iCoefficient = %d is out of bounds", iCoefficient);
		return DEFAULT_ERROR_FOR_DOUBLE_FUNCTIONS;
	}

	double* changedValue;
	switch (iCoefficient)
	{
	case 0:
		changedValue = &(facets[iFacet].plane.norm.x);
		break;
	case 1:
		changedValue = &(facets[iFacet].plane.norm.y);
		break;
	case 2:
		changedValue = &(facets[iFacet].plane.norm.z);
		break;
	case 3:
		changedValue = &(facets[iFacet].plane.dist);
		break;
	}
	double changedValuePrev = *changedValue;
	*changedValue = changedValuePrev + DEFAULT_DERIVATIVE_STEP;
	// We multiply functional value with 0.5 because in matrix
	// each element is multiplied by 0.5
	double funcValueRight = 0.5 * corpol_calculate_functional(prevPlanes);
	*changedValue = changedValuePrev - DEFAULT_DERIVATIVE_STEP;
	double funcValueLeft = 0.5 * corpol_calculate_functional(prevPlanes);
	*changedValue = changedValuePrev;

	return 0.5 * DEFAULT_DERIVATIVE_STEP_RECIPROCAL
			* (funcValueRight - funcValueLeft);
}

