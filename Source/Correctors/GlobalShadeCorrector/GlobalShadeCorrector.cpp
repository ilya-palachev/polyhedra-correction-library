/*
 * GlobalShadeCorrector.cpp
 *
 *  Created on: 12.05.2013
 *      Author: iliya
 */

#include "PolyhedraCorrectionLibrary.h"

GlobalShadeCorrector::GlobalShadeCorrector() :
				PCorrector(),
				edgeData(NULL),
				contourData(NULL),
				parameters(
				{ METHOD_CORRECTOR_DEFAULT, EPS_LOOP_STOP_DEFAULT,
	DELTA_GRADIENT_STEP_DEFAULT}),
				facetsNotAssociated(NULL),
				gradient(NULL),
				prevPlanes(NULL),
				dim(0)
{
}

GlobalShadeCorrector::GlobalShadeCorrector(Polyhedron* p,
		ShadeContourData* scd, GSCorrectorParameters* _parameters) :
				PCorrector(p),
				edgeData(NULL),
				contourData(scd),
				parameters(*_parameters),
				facetsNotAssociated(new list<int>),
				gradient(NULL),
				prevPlanes(NULL),
				dim(0)
{
}

GlobalShadeCorrector::~GlobalShadeCorrector()
{
}

void GlobalShadeCorrector::runCorrection()
{
	DEBUG_START;
	preprocess();
	findNotAssociatedFacets();
#ifndef NDEBUG
	DEBUG_PRINT("The following facets have no associations:");
	for (list<int>::iterator iter = facetsNotAssociated->begin();
			iter != facetsNotAssociated->end(); ++iter)
	{
		DEBUG_PRINT("%d", *iter);
	}
#endif
	int numFacetsNotAssociated = facetsNotAssociated->size();
	dim = (polyhedron->numFacets - numFacetsNotAssociated) * 5;

	gradient = new double[dim];
	prevPlanes = new Plane[polyhedron->numFacets];

	DEBUG_PRINT("memory allocation done");

	for (int i = 0; i < polyhedron->numFacets; ++i)
	{
		prevPlanes[i] = polyhedron->facets[i].plane;
	}
	DEBUG_PRINT("memory initialization done");

	double error = calculateFunctional();
	double errorInitial = error;
	DEBUG_PRINT("first functional calculation done. error = %e", error);

	int numIterations = 0;
	while (error > parameters.epsLoopStop)
	{
		DEBUG_PRINT(COLOUR_GREEN "Iteration %d : begin\n", numIterations);

		char* fileName = new char[255];
		sprintf(fileName, "./poly-data-out/correction-of-cube/cube%d.txt",
				numIterations);

		polyhedron->my_fprint(fileName);
		delete[] fileName;

		runCorrectionIteration();

		for (int i = 0; i < polyhedron->numFacets; ++i)
		{
			DEBUG_PRINT(
					"PLane[%d]: (%lf, %lf, %lf, %lf) --> (%lf, %lf, %lf, %lf)",
					i, prevPlanes[i].norm.x, prevPlanes[i].norm.y,
					prevPlanes[i].norm.z, prevPlanes[i].dist,
					polyhedron->facets[i].plane.norm.x,
					polyhedron->facets[i].plane.norm.y,
					polyhedron->facets[i].plane.norm.z,
					polyhedron->facets[i].plane.dist);
			prevPlanes[i] = polyhedron->facets[i].plane;
		}
		error = calculateFunctional();
		MAIN_PRINT("error = %le", error);
		MAIN_PRINT(COLOUR_GREEN "Iteration %d : End\n", numIterations);
		if (error > MAX_ERROR_ABSOLUTE)
		{
			ERROR_PRINT("Too big absolute value of error.");
			break;
		}
		if (error / errorInitial > MAX_ERROR_RELATIVE)
		{
			ERROR_PRINT("Too big relative value of error.");
			break;
		}
		++numIterations;
	}
	DEBUG_END;
}

void GlobalShadeCorrector::findNotAssociatedFacets()
{
	for (int iFacet = 0; iFacet < polyhedron->numFacets; ++iFacet)
	{
		int* indVertices = polyhedron->facets[iFacet].indVertices;
		int numVerticesFacet = polyhedron->facets[iFacet].numVertices;
		int numAssociations = 0;
		for (int iVertex = 0; iVertex < numVerticesFacet; ++iVertex)
		{
			int edgeid = edgeData->findEdge(indVertices[iVertex],
					indVertices[iVertex + 1]);
			numAssociations += edgeData->edges[edgeid].assocList.size();
		}
		if (numAssociations == 0)
		{
			facetsNotAssociated->push_back(iFacet);
		}
	}
}

double GlobalShadeCorrector::calculateFunctional()
{
	double sum = 0;

	for (int iEdge = 0; iEdge < edgeData->numEdges; ++iEdge)
	{
//    	DEBUG_PRINT("%s: processing edge #%d\n", __func__, i);
		int f0 = edgeData->edges[iEdge].f0;
		int f1 = edgeData->edges[iEdge].f1;
		Plane plane0 = polyhedron->facets[f0].plane;
		Plane plane1 = polyhedron->facets[f1].plane;
		Plane planePrev0 = prevPlanes[f0];
		Plane planePrev1 = prevPlanes[f1];

		int iAssoc = 0;
		for (list<EdgeContourAssociation>::iterator itCont =
				edgeData->edges[iEdge].assocList.begin();
				itCont != edgeData->edges[iEdge].assocList.end(); ++itCont)
		{
//        	DEBUG_PRINT("\t%s: processing contour #%d\n", __func__, j);
			int curContour = itCont->indContour;
			int curNearestSide = itCont->indNearestSide;
			SideOfContour * sides = contourData->contours[curContour].sides;
			Plane planeOfProjection = contourData->contours[curContour].plane;
			double weight = itCont->weight;

			double enumerator = -planePrev1.norm * planeOfProjection.norm;
			double denominator = (planePrev0.norm - planePrev1.norm)
					* planeOfProjection.norm;
//			DEBUG_PRINT("iEdge = %d (%d, %d), iContour = %d | enu = %lf, den = %lf",
//					iEdge, edges[iEdge].v0, edges[iEdge].v1, itCont->indContour, enumerator, denominator);

//			ASSERT(fabs(denominator) > 1e-10);
			double gamma_ij = enumerator / denominator;

			double a_ij = gamma_ij * plane0.norm.x
					+ (1 - gamma_ij) * plane1.norm.x;
			double b_ij = gamma_ij * plane0.norm.y
					+ (1 - gamma_ij) * plane1.norm.y;
			double c_ij = gamma_ij * plane0.norm.z
					+ (1 - gamma_ij) * plane1.norm.z;
			double d_ij = gamma_ij * plane0.dist + (1 - gamma_ij) * plane1.dist;

			Vector3d A_ij0 = sides[curNearestSide].A1;
			Vector3d A_ij1 = sides[curNearestSide].A2;

			double summand0 = a_ij * A_ij0.x + b_ij * A_ij0.y + c_ij * A_ij0.z
					+ d_ij;

			summand0 *= summand0;
			summand0 *= weight;

			double summand1 = a_ij * A_ij1.x + b_ij * A_ij1.y + c_ij * A_ij1.z
					+ d_ij;

			summand1 *= summand1;
			summand1 *= weight;

			sum += summand0 + summand1;
		}
	}
	return sum;

}

void GlobalShadeCorrector::shiftCoefficients(double delta)
{
	list<int>::iterator iterNotAssociated = facetsNotAssociated->begin();
	int countNotAssociated = 0;
	for (int iFacet = 0; iFacet < polyhedron->numFacets; ++iFacet)
	{
		if (*iterNotAssociated == iFacet)
		{
			++iterNotAssociated;
			++countNotAssociated;
			continue;
		}
		int iFacetShifted = iFacet - countNotAssociated;
		Plane deltaPlane = Plane(Vector3d(gradient[4 * iFacetShifted],
				gradient[4 * iFacetShifted + 1],
				gradient[4 * iFacetShifted + 2]),
				gradient[4 * iFacetShifted + 3]);
		deltaPlane.norm *= parameters.deltaGradientStep;
		deltaPlane.dist *= parameters.deltaGradientStep;
		polyhedron->facets[iFacet].plane.norm -= deltaPlane.norm;
		polyhedron->facets[iFacet].plane.dist -= deltaPlane.dist;
	}
}

double GlobalShadeCorrector::calculateFunctional(double delta)
{
	shiftCoefficients(delta);
	double functionalValue = calculateFunctional();
	shiftCoefficients(-delta);
	return functionalValue;
}

const double GOLDEN_RATIO_RECIPROCAL = 0.5 * (sqrt(5) - 1);

enum GoldenRatioMethodState
{
	ELIMINATING_LEFT_BOUND, ELIMINATING_RIGHT_BOUND
};

double GlobalShadeCorrector::findOptimalDelta(double deltaMax)
{
	double leftBound = 0.;
	double rightBound = deltaMax;
	double leftChecker = rightBound - GOLDEN_RATIO_RECIPROCAL *
			(rightBound - leftBound);
	double rightChecker = leftBound + GOLDEN_RATIO_RECIPROCAL *
			(rightBound - leftBound);
	while (rightBound > leftBound)
	{
		double leftValue = calculateFunctional(leftChecker);
		double rightValue = calculateFunctional(rightChecker);
		if (leftValue > rightValue)
		{
			leftBound = leftChecker;
			leftChecker = rightChecker;
			rightChecker = leftBound + GOLDEN_RATIO_RECIPROCAL *
					(rightBound - leftBound);
		}
		else
		{
			rightBound = rightChecker;
			rightChecker = leftChecker;
			leftChecker = rightBound - GOLDEN_RATIO_RECIPROCAL *
					(rightBound - leftBound);
		}
	}
	return leftBound;
}

void GlobalShadeCorrector::runCorrectionIteration()
{
	DEBUG_START;
	calculateGradient();
#ifdef GLOBAL_CORRECTION_DERIVATIVE_TESTING
	derivativeTest_all();
#endif
	double deltaOptimal;
	switch (parameters.methodName)
	{
	case METHOD_GRADIENT_DESCENT:
		shiftCoefficients(parameters.deltaGradientStep);
		break;
	case METHOD_GRADIENT_DESCENT_FAST:
		deltaOptimal = findOptimalDelta(parameters.deltaGradientStep);
		MAIN_PRINT("deltaOptimal = %lf", deltaOptimal);
		shiftCoefficients(deltaOptimal);
		break;
	case METHOD_UNKNOWN:
	default:
		ERROR_PRINT("Unknown method name!");
		break;
	}
	DEBUG_END;
}

void GlobalShadeCorrector::calculateGradient()
{
	DEBUG_START;
	for (int i = 0; i < dim; ++i)
	{
		gradient[i] = 0.;
	}

	list<int>::iterator iterNotAssicated = facetsNotAssociated->begin();
	int countNotAssociated = 0;

	for (int iFacet = 0; iFacet < polyhedron->numFacets; ++iFacet)
	{
		if (*iterNotAssicated == iFacet)
		{
			++iterNotAssicated;
			++countNotAssociated;
			continue;
		}
		int iFacetShifted = iFacet - countNotAssociated;

		int nv = polyhedron->facets[iFacet].numVertices;
		int *index = polyhedron->facets[iFacet].indVertices;

		Plane planePrevThis = prevPlanes[iFacet];

		int i_ak = 4 * iFacetShifted;
		int i_bk = i_ak + 1;
		int i_ck = i_ak + 2;
		int i_dk = i_ak + 3;

		double ak = planePrevThis.norm.x;
		double bk = planePrevThis.norm.y;
		double ck = planePrevThis.norm.z;
		double dk = planePrevThis.dist;

		for (int iEdge = 0; iEdge < nv; ++iEdge)
		{
			int v0 = index[iEdge];
			int v1 = index[iEdge + 1];
			int iFacetNeighbour = index[nv + 1 + iEdge];

			Plane planePrevNeighbour = prevPlanes[iFacetNeighbour];
			double an = planePrevNeighbour.norm.x;
			double bn = planePrevNeighbour.norm.y;
			double cn = planePrevNeighbour.norm.z;
			double dn = planePrevNeighbour.dist;

			int edgeid = edgeData->findEdge(v0, v1);

			if (edgeid == -1)
			{
				ERROR_PRINT("Error! edge (%d, %d) cannot be found\n", v0, v1);
				return;
			}
			int iAssociation = 0;
			for (list<EdgeContourAssociation>::iterator itCont =
					edgeData->edges[edgeid].assocList.begin();
					itCont != edgeData->edges[edgeid].assocList.end(); ++itCont)
			{
				int curContour = itCont->indContour;
				int curNearestSide = itCont->indNearestSide;
				double weight = itCont->weight;
				SideOfContour * sides = contourData->contours[curContour].sides;
				Plane planeOfProjection = contourData->contours[curContour]
						.plane;

				double enumerator = -planePrevNeighbour.norm
						* planeOfProjection.norm;
				double denominator = ((planePrevThis.norm
						- planePrevNeighbour.norm) * planeOfProjection.norm);

				double gamma_ij = enumerator / denominator;
				ASSERT(!isnan(gamma_ij));

				Vector3d A_ij1 = sides[curNearestSide].A1;
				Vector3d A_ij2 = sides[curNearestSide].A2;

				double x0 = A_ij1.x;
				double y0 = A_ij1.y;
				double z0 = A_ij1.z;
				double x1 = A_ij2.x;
				double y1 = A_ij2.y;
				double z1 = A_ij2.z;

				double xx = x0 * x0 + x1 * x1;
				double xy = x0 * y0 + x1 * y1;
				double xz = x0 * z0 + x1 * z1;
				double x = x0 + x1;
				double yy = y0 * y0 + y1 * y1;
				double yz = y0 * z0 + y1 * z1;
				double y = y0 + y1;
				double zz = z0 * z0 + z1 * z1;
				double z = z0 + z1;

				double gamma1 = gamma_ij * gamma_ij * weight;
				double gamma2 = gamma_ij * (1 - gamma_ij) * weight;

				double coeff_ak_ak = gamma1 * xx;
				double coeff_ak_bk = gamma1 * xy;
				double coeff_ak_ck = gamma1 * xz;
				double coeff_ak_dk = gamma1 * x;

				gradient[i_ak] += ak * coeff_ak_ak + bk * coeff_ak_bk
						+ ck * coeff_ak_ck + dk * coeff_ak_dk;

				double coeff_bk_ak = gamma1 * xy;
				double coeff_bk_bk = gamma1 * yy;
				double coeff_bk_ck = gamma1 * yz;
				double coeff_bk_dk = gamma1 * y;

				gradient[i_bk] += ak * coeff_bk_ak + bk * coeff_bk_bk
						+ ck * coeff_bk_ck + dk * coeff_bk_dk;

				double coeff_ck_ak = gamma1 * xz;
				double coeff_ck_bk = gamma1 * yz;
				double coeff_ck_ck = gamma1 * zz;
				double coeff_ck_dk = gamma1 * z;

				gradient[i_ck] += ak * coeff_ck_ak + bk * coeff_ck_bk
						+ ck * coeff_ck_ck + dk * coeff_ck_dk;

				double coeff_dk_ak = gamma1 * x;
				double coeff_dk_bk = gamma1 * y;
				double coeff_dk_ck = gamma1 * z;
				double coeff_dk_dk = gamma1 * 2;

				gradient[i_dk] += ak * coeff_dk_ak + bk * coeff_dk_bk
						+ ck * coeff_dk_ck + dk * coeff_dk_dk;

				double coeff_ak_an = gamma2 * xx;
				double coeff_ak_bn = gamma2 * xy;
				double coeff_ak_cn = gamma2 * xz;
				double coeff_ak_dn = gamma2 * x;

				gradient[i_ak] += an * coeff_ak_an + bn * coeff_ak_bn
						+ cn * coeff_ak_cn + dn * coeff_ak_dn;

				double coeff_bk_an = gamma2 * xy;
				double coeff_bk_bn = gamma2 * yy;
				double coeff_bk_cn = gamma2 * yz;
				double coeff_bk_dn = gamma2 * y;

				gradient[i_bk] += an * coeff_bk_an + bn * coeff_bk_bn
						+ cn * coeff_bk_cn + dn * coeff_bk_dn;

				double coeff_ck_an = gamma2 * xz;
				double coeff_ck_bn = gamma2 * yz;
				double coeff_ck_cn = gamma2 * zz;
				double coeff_ck_dn = gamma2 * z;

				gradient[i_ck] += an * coeff_ck_an + bn * coeff_ck_bn
						+ cn * coeff_ck_cn + dn * coeff_ck_dn;

				double coeff_dk_an = gamma2 * x;
				double coeff_dk_bn = gamma2 * y;
				double coeff_dk_cn = gamma2 * z;
				double coeff_dk_dn = gamma2 * 2;

				gradient[i_dk] += an * coeff_dk_an + bn * coeff_dk_bn
						+ cn * coeff_dk_cn + dn * coeff_dk_dn;
			}
		}
	}
	DEBUG_END;
}
