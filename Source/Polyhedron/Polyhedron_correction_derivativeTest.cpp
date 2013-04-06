#include "PolyhedraCorrectionLibrary.h"

void Polyhedron::corpol_derivativeTest_all(
		Plane* prevPlanes,
		double* matrix) {
	corpol_derivativeTest_1(prevPlanes, matrix);
	corpol_derivativeTest_2(prevPlanes, matrix);
}

void Polyhedron::corpol_derivativeTest_1(
		Plane* prevPlanes,
		double* matrix) {
	for (int iVariable = 0; iVariable < 4 * numFacets; ++iVariable) {
		double valueFromDerTest = corpol_calculate_functional_derivative_1(
				prevPlanes, iVariable);
		double valueFromMatrix = corpol_derivativeTest_calculateValFromMatrix_1(
				iVariable, matrix);

		DBGPRINT("value from derivative test: %lf, value from matrix: %lf",
				valueFromDerTest, valueFromMatrix);

		int iFacet = iVariable / 4;
		int iCoefficient = iVariable % 4;

		double absValue = fabs(valueFromDerTest);
		if (absValue <= EPSILON_FOR_DIVISION)
			// In this case we cannot compare values
			continue;

		double errorAbsolute = fabs(valueFromDerTest - valueFromMatrix);
		if (errorAbsolute > EPSILON_FOR_WARNING_IN_DERIVATIVE_TESTING_ABSOLUTE) {
			ERROR_PRINT("!!! Too big absolute error: %lf", errorAbsolute);
			ERROR_PRINT(" iFacet = %d, iCoefficient = %d", iFacet, iCoefficient);
		}

		double errorRelative = errorAbsolute / absValue;
		if (errorRelative > EPSILON_FOR_WARNING_IN_DERIVATIVE_TESTING_RELATIVE) {
			ERROR_PRINT("!!! Too big relative error: %lf", errorRelative);
			ERROR_PRINT(" iFacet = %d, iCoefficient = %d", iFacet, iCoefficient);
		}

	}
}

double Polyhedron::corpol_derivativeTest_calculateValFromMatrix_1(
		int iVariable,
		double* matrix) {
	double valueFromMatrix = 0.;
	int iFacet = iVariable / 4;
	int iCoefficient = iVariable % 4;
	for (int jVariable = 0; jVariable < 4 * numFacets; ++jVariable) {
		int jFacet = jVariable / 4;
		int jCoefficient = jVariable % 4;
		double elementOfMatrix = matrix[(5 * iFacet + iCoefficient) * 5 * numFacets
				+ 5 * jFacet + jCoefficient];
		double currentCoefficient;
		switch (jCoefficient) {
		case 0:
			currentCoefficient = facets[jFacet].plane.norm.x;
			break;
		case 1:
			currentCoefficient = facets[jFacet].plane.norm.y;
			break;
		case 2:
			currentCoefficient = facets[jFacet].plane.norm.z;
			break;
		case 3:
			currentCoefficient = facets[jFacet].plane.dist;
			break;
		}
		valueFromMatrix += elementOfMatrix * currentCoefficient;
	}
	return valueFromMatrix;

}

double Polyhedron::corpol_calculate_functional_derivative_1(
		Plane* prevPlanes,
		int iVariable) {
	if (iVariable < 0 || iVariable >= numFacets * 4) {
		DBGPRINT("Error. iVariable = %d is out of bounds", iVariable);
		return DEFAULT_ERROR_FOR_DOUBLE_FUNCTIONS;
	}

	int iFacet = iVariable / 4;
	int iCoefficient = iVariable % 4;

	double* changedValue;
	switch (iCoefficient) {
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

void Polyhedron::corpol_derivativeTest_2(
		Plane* prevPlanes,
		double* matrix) {
	for (int iVariable = 0; iVariable < 4 * numFacets; ++iVariable) {

		for (int jVariable = 0; jVariable < 4 * numFacets; ++jVariable) {

			double valueFromDerTest = corpol_calculate_functional_derivative_2(
					prevPlanes, iVariable, jVariable);
			double valueFromMatrix = corpol_derivativeTest_calculateValFromMatrix_2(
					iVariable, jVariable, matrix);
			DBGPRINT("value from derivative test: %lf, value from matrix: %lf",
					valueFromDerTest, valueFromMatrix);

			int iFacet = iVariable / 4;
			int iCoefficient = iVariable % 4;
			int jFacet = jVariable / 4;
			int jCoefficient = jVariable % 4;

			double absValue = fabs(valueFromDerTest);
			if (absValue <= EPSILON_FOR_DIVISION)
				// In this case we cannot compare values
				continue;

			double errorAbsolute = fabs(valueFromDerTest - valueFromMatrix);
			if (errorAbsolute > EPSILON_FOR_WARNING_IN_DERIVATIVE_TESTING_ABSOLUTE) {
				ERROR_PRINT("!!! Too big absolute error: %lf", errorAbsolute);
				ERROR_PRINT(" iFacet = %d, iCoefficient = %d, "
						"jFacet = %d, jCoefficient = %d",
						iFacet, iCoefficient, jFacet, jCoefficient);
			}

			double errorRelative = errorAbsolute / absValue;
			if (errorRelative > EPSILON_FOR_WARNING_IN_DERIVATIVE_TESTING_RELATIVE) {
				ERROR_PRINT("!!! Too big relative error: %lf", errorRelative);
				ERROR_PRINT(" iFacet = %d, iCoefficient = %d, "
						"jFacet = %d, jCoefficient = %d",
						iFacet, iCoefficient, jFacet, jCoefficient);
			}
		}
	}
}

double Polyhedron::corpol_calculate_functional_derivative_2(
		Plane* prevPlanes,
		int iVariable,
		int jVariable) {
	if (iVariable < 0 || iVariable >= numFacets * 4) {
		DBGPRINT("Error. iVariable = %d is out of bounds", iVariable);
		return DEFAULT_ERROR_FOR_DOUBLE_FUNCTIONS;
	}
	if (jVariable < 0 || jVariable >= numFacets * 4) {
		DBGPRINT("Error. jVariable = %d is out of bounds", jVariable);
		return DEFAULT_ERROR_FOR_DOUBLE_FUNCTIONS;
	}

	int iFacet = iVariable / 4;
	int iCoefficient = iVariable % 4;

	int jFacet = jVariable / 4;
	int jCoefficient = jVariable % 4;

	double* changedValueI;
	switch (iCoefficient) {
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
	switch (jCoefficient) {
	case 0:
		changedValueJ = &(facets[iFacet].plane.norm.x);
		break;
	case 1:
		changedValueJ = &(facets[iFacet].plane.norm.y);
		break;
	case 2:
		changedValueJ = &(facets[iFacet].plane.norm.z);
		break;
	case 3:
		changedValueJ = &(facets[iFacet].plane.dist);
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
	double funcValueRightDown = 0.5 * corpol_calculate_functional(prevPlanes);

	*changedValueI = changedValuePrevI - DEFAULT_DERIVATIVE_STEP;
	*changedValueJ = changedValuePrevJ - DEFAULT_DERIVATIVE_STEP;
	double funcValueLeftDown = 0.5 * corpol_calculate_functional(prevPlanes);

	*changedValueI = changedValuePrevI;
	*changedValueJ = changedValuePrevJ;

	return 0.25 * DEFAULT_DERIVATIVE_STEP_RECIPROCAL
			* DEFAULT_DERIVATIVE_STEP_RECIPROCAL
			* (funcValueRightUp - funcValueLeftUp - funcValueRightDown
					+ funcValueLeftDown);
}

double Polyhedron::corpol_derivativeTest_calculateValFromMatrix_2(
		int iVariable,
		int jVariable,
		double* matrix) {
	int iFacet = iVariable / 4;
	int iCoefficient = iVariable % 4;

	int jFacet = jVariable / 4;
	int jCoefficient = jVariable % 4;

	int dim = 5 * numFacets;

	return matrix[dim * (5 * iFacet + iCoefficient) + 5 * jFacet + jCoefficient];
}

