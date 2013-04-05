#include "PolyhedraCorrectionLibrary.h"

double Polyhedron::corpol_calculate_functional_derivative_1(
		Plane* prevPlanes,
		int iVariable) {
	if (iVariable < 0 || iVariable >= numFacets * 4) {
		DBGPRINT("Error. iVariable = %d is out of bounds",
				iVariable);
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
	double funcValueRight = corpol_calculate_functional(prevPlanes);
	*changedValue = changedValuePrev - DEFAULT_DERIVATIVE_STEP;
	double funcValueLeft = corpol_calculate_functional(prevPlanes);
	*changedValue = changedValuePrev;

	return 0.5 * DEFAULT_DERIVATIVE_STEP_RECIPROCAL *
			(funcValueRight - funcValueLeft);

}

double Polyhedron::corpol_calculate_functional_derivative_2(
		Plane* prevPlanes,
		int iVariable,
		int jVariable) {
	if (iVariable < 0 || iVariable >= numFacets * 4) {
		DBGPRINT("Error. iVariable = %d is out of bounds",
				iVariable);
		return DEFAULT_ERROR_FOR_DOUBLE_FUNCTIONS;
	}
	if (jVariable < 0 || jVariable >= numFacets * 4) {
		DBGPRINT("Error. jVariable = %d is out of bounds",
				jVariable);
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
	double funcValueRightUp = corpol_calculate_functional(prevPlanes);

	*changedValueI = changedValuePrevI - DEFAULT_DERIVATIVE_STEP;
	*changedValueJ = changedValuePrevJ + DEFAULT_DERIVATIVE_STEP;
	double funcValueLeftUp = corpol_calculate_functional(prevPlanes);

	*changedValueI = changedValuePrevI + DEFAULT_DERIVATIVE_STEP;
	*changedValueJ = changedValuePrevJ - DEFAULT_DERIVATIVE_STEP;
	double funcValueRightDown = corpol_calculate_functional(prevPlanes);

	*changedValueI = changedValuePrevI - DEFAULT_DERIVATIVE_STEP;
	*changedValueJ = changedValuePrevJ - DEFAULT_DERIVATIVE_STEP;
	double funcValueLeftDown = corpol_calculate_functional(prevPlanes);

	*changedValueI = changedValuePrevI;
	*changedValueJ = changedValuePrevJ;

	return 0.25 * DEFAULT_DERIVATIVE_STEP_RECIPROCAL *
			DEFAULT_DERIVATIVE_STEP_RECIPROCAL *
			(funcValueRightUp - funcValueLeftUp -
					funcValueRightDown + funcValueLeftDown);


}
