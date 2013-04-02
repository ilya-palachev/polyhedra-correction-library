#include "PolyhedraCorrectionLibrary.h"

double Polyhedron::corpol_calculate_functional_derivative(
		Plane* prevPlanes,
		int iVariable) {
	if (iVariable < 0 || iVariable >= numFacets * 4) {
		DBGPRINT("Error. iVariable = %d is out of bounds",
				iVariable);
		return DEFAULT_ERROR_FOR_DOUBLE_FUNCTIONS;
	}

	int iFacet = iVariable / 4;
	int iCoefficient = iVariable % 4;

	double& changedValue;
	switch (iCoefficient) {
	case 0:
		changedValue = facets[iFacet].plane.norm.x;
		break;
	case 1:
		changedValue = facets[iFacet].plane.norm.y;
		break;
	case 2:
		changedValue = facets[iFacet].plane.norm.z;
		break;
	case 3:
		changedValue = facets[iFacet].plane.dist;
		break;
	}
	double changedValuePrev = changedValue;
	changedValue = changedValuePrev + DEFAULT_DERIVATIVE_STEP;
	double funcValueRight = corpol_calculate_functional(prevPlanes);
	changedValue = changedValuePrev - DEFAULT_DERIVATIVE_STEP;
	double funcValueLeft = corpol_calculate_functional(prevPlanes);
	changedValue = changedValuePrev;

	return 0.5 * DEFAULT_DERIVATIVE_STEP_RECIPROCAL *
			(funcValueRight - funcValueLeft);

}

