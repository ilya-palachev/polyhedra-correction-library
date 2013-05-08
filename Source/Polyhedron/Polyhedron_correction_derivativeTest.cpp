#include "PolyhedraCorrectionLibrary.h"

void Polyhedron::corpol_derivativeTest_all(Plane* prevPlanes, double* matrix,
		list<int>* facetsNotAssociated)
{
	corpol_derivativeTest_1(prevPlanes, matrix, facetsNotAssociated);

	/* We have disabled 2nd derivative testing, since it never
	 * produces any useful output, but only errors.
	 * This is caused with the fact that there is not enough
	 * machine precision to calculate exact 2nd derivative
	 * numerically
	 */
//	corpol_derivativeTest_2(prevPlanes, matrix, facetsNotAssociated);
}

void Polyhedron::corpol_derivativeTest_1(Plane* prevPlanes, double* matrix,
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
			double valueFromMatrix =
					corpol_derivativeTest_calculateValFromMatrix_1(
							iFacetShifted, iCoefficient, matrix,
							facetsNotAssociated);

			bool makeAssertion = false;

			DEBUG_PRINT(
					"value from derivative test: %le, value from matrix: %le",
					valueFromDerTest, valueFromMatrix);

			double errorAbsolute = fabs(valueFromDerTest - valueFromMatrix);
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

double Polyhedron::corpol_derivativeTest_calculateValFromMatrix_1(int iFacet,
		int iCoefficient, double* matrix, list<int>* facetsNotAssociated)
{
	list<int>::iterator jIteratorNotAssociated = facetsNotAssociated->begin();
	int jCountNotAssociated = 0;
	int numFacetsNotAssociated = facetsNotAssociated->size();
	int dim = 5 * (numFacets - numFacetsNotAssociated);

	double valueFromMatrix = 0.;
	for (int jFacet = 0; jFacet < numFacets; ++jFacet)
	{
		if (jFacet == *jIteratorNotAssociated)
		{
			++jIteratorNotAssociated;
			++jCountNotAssociated;
			continue;
		}
		int jFacetShifted = jFacet - jCountNotAssociated;

		for (int jCoefficient = 0; jCoefficient < 4; ++jCoefficient)
		{
			double elementOfMatrix = matrix[(5 * iFacet + iCoefficient) * dim
					+ 5 * jFacetShifted + jCoefficient];
			double currentCoefficient;
			switch (jCoefficient)
			{
			case 0:
				currentCoefficient = facets[jFacetShifted].plane.norm.x;
				break;
			case 1:
				currentCoefficient = facets[jFacetShifted].plane.norm.y;
				break;
			case 2:
				currentCoefficient = facets[jFacetShifted].plane.norm.z;
				break;
			case 3:
				currentCoefficient = facets[jFacetShifted].plane.dist;
				break;
			}
			valueFromMatrix += elementOfMatrix * currentCoefficient;
		}
	}
	return valueFromMatrix;

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

void Polyhedron::corpol_derivativeTest_2(Plane* prevPlanes, double* matrix,
		list<int>* facetsNotAssociated)
{
	int numFacetsNotAssociated = facetsNotAssociated->size();
	int dim = 5 * (numFacets - numFacetsNotAssociated);

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
			list<int>::iterator jIteratorNotAssociated =
					facetsNotAssociated->begin();
			int jCountNotAssociated = 0;

			for (int jFacet = 0; jFacet < numFacets; ++jFacet)
			{
				if (jFacet == *jIteratorNotAssociated)
				{
					++jIteratorNotAssociated;
					++jCountNotAssociated;
					continue;
				}
				int jFacetShifted = jFacet - jCountNotAssociated;

				for (int jCoefficient = 0; jCoefficient < 4; ++jCoefficient)
				{

					double valueFromDerTest =
							corpol_calculate_functional_derivative_2(prevPlanes,
									iFacet, iCoefficient, jFacet, jCoefficient);
					double valueFromMatrix =
							corpol_derivativeTest_calculateValFromMatrix_2(
									iFacetShifted, iCoefficient, jFacetShifted,
									jCoefficient, matrix, facetsNotAssociated);

					DEBUG_PRINT(
							"value from derivative test: %le, value from matrix: %le",
							valueFromDerTest, valueFromMatrix);
					DEBUG_PRINT(
							" iFacet = %d, iCoefficient = %d, jFacet = %d, jCoefficient = %d",
							iFacet, iCoefficient, jFacet, jCoefficient);

					double errorAbsolute = fabs(
							valueFromDerTest - valueFromMatrix);
					if (errorAbsolute
							> EPSILON_FOR_WARNING_IN_DERIVATIVE_TESTING_ABSOLUTE)
					{
						ERROR_PRINT("!!! Too big absolute error: %le",
								errorAbsolute);
						ERROR_PRINT(
								" iFacet = %d, iCoefficient = %d, jFacet = %d, jCoefficient = %d",
								iFacet, iCoefficient, jFacet, jCoefficient);
					}

					double absValue = fabs(valueFromDerTest);
					if (absValue <= EPSILON_FOR_DIVISION)
						// In this case we cannot compare values
						continue;

					double errorRelative = errorAbsolute / absValue;
					if (errorRelative
							> EPSILON_FOR_WARNING_IN_DERIVATIVE_TESTING_RELATIVE)
					{
						ERROR_PRINT("!!! Too big relative error: %lf",
								errorRelative);
						ERROR_PRINT(
								" iFacet = %d, iCoefficient = %d, jFacet = %d, jCoefficient = %d",
								iFacet, iCoefficient, jFacet, jCoefficient);
					}
				}
			}
		}
	}
}

double Polyhedron::corpol_calculate_functional_derivative_2(Plane* prevPlanes,
		int iFacet, int iCoefficient, int jFacet, int jCoefficient)
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
	if (jFacet < 0 || jFacet >= numFacets)
	{
		ERROR_PRINT("Error. jFacet = %d is out of bounds", jFacet);
		return DEFAULT_ERROR_FOR_DOUBLE_FUNCTIONS;
	}

	if (jCoefficient < 0 || jCoefficient >= 4)
	{
		ERROR_PRINT("Error. jCoefficient = %d is out of bounds", jCoefficient);
		return DEFAULT_ERROR_FOR_DOUBLE_FUNCTIONS;
	}

	if (iFacet != jFacet || iCoefficient != jCoefficient)
	{
		double* changedValueI;
		switch (iCoefficient)
		{
		case 0:
			changedValueI = &(facets[iFacet].plane.norm.x);
			break;
		case 1:
			changedValueI = &(facets[iFacet].plane.norm.y);
			break;
		case 2:
			changedValueI = &(facets[iFacet].plane.norm.z);
			break;
		case 3:
			changedValueI = &(facets[iFacet].plane.dist);
			break;
		}

		double* changedValueJ;
		switch (jCoefficient)
		{
		case 0:
			changedValueJ = &(facets[jFacet].plane.norm.x);
			break;
		case 1:
			changedValueJ = &(facets[jFacet].plane.norm.y);
			break;
		case 2:
			changedValueJ = &(facets[jFacet].plane.norm.z);
			break;
		case 3:
			changedValueJ = &(facets[jFacet].plane.dist);
			break;
		}
		double changedValuePrevI = *changedValueI;
		double changedValuePrevJ = *changedValueJ;

		*changedValueI = changedValuePrevI + DEFAULT_DERIVATIVE_STEP;
		*changedValueJ = changedValuePrevJ + DEFAULT_DERIVATIVE_STEP;
		double funcValueRightUp = 0.5 * corpol_calculate_functional(prevPlanes);

		*changedValueI = changedValuePrevI - DEFAULT_DERIVATIVE_STEP;
		*changedValueJ = changedValuePrevJ + DEFAULT_DERIVATIVE_STEP;
		double funcValueLeftUp = 0.5 * corpol_calculate_functional(prevPlanes);

		*changedValueI = changedValuePrevI + DEFAULT_DERIVATIVE_STEP;
		*changedValueJ = changedValuePrevJ - DEFAULT_DERIVATIVE_STEP;
		double funcValueRightDown = 0.5
				* corpol_calculate_functional(prevPlanes);

		*changedValueI = changedValuePrevI - DEFAULT_DERIVATIVE_STEP;
		*changedValueJ = changedValuePrevJ - DEFAULT_DERIVATIVE_STEP;
		double funcValueLeftDown = 0.5
				* corpol_calculate_functional(prevPlanes);

		*changedValueI = changedValuePrevI;
		*changedValueJ = changedValuePrevJ;

		return 0.25 * DEFAULT_DERIVATIVE_STEP_RECIPROCAL
				* DEFAULT_DERIVATIVE_STEP_RECIPROCAL
				* (funcValueRightUp - funcValueLeftUp - funcValueRightDown
						+ funcValueLeftDown);
	}
	else
	{
		// iVariable == jVariable

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
		double funcValueRight = 0.5 * corpol_calculate_functional(prevPlanes);

		*changedValue = changedValuePrev - DEFAULT_DERIVATIVE_STEP;
		double funcValueLeft = 0.5 * corpol_calculate_functional(prevPlanes);

		*changedValue = changedValuePrev;
		double funcValueCenter = 0.5 * corpol_calculate_functional(prevPlanes);

		return DEFAULT_DERIVATIVE_STEP_RECIPROCAL
				* DEFAULT_DERIVATIVE_STEP_RECIPROCAL
				* (funcValueRight - 2. * funcValueCenter + funcValueLeft);
	}
}

double Polyhedron::corpol_derivativeTest_calculateValFromMatrix_2(int iFacet,
		int iCoefficient, int jFacet, int jCoefficient, double* matrix,
		list<int>* facetsNotAssociated)
{
	int numFacetsNotAssociated = facetsNotAssociated->size();
	int dim = 5 * (numFacets - numFacetsNotAssociated);

	return matrix[dim * (5 * iFacet + iCoefficient) + 5 * jFacet + jCoefficient];
}

