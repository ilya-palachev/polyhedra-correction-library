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
				facetsNotAssociated(),
				gradient(NULL),
				gradientPrevious(NULL),
				gradientNorm(0.),
				gradientNormPrevious(0.),
				iMinimizedDimension(0),
				iMinimizationLevel(0),
				prevPlanes(NULL),
				dim(0),
				associator(NULL)
{
	DEBUG_START;
	DEBUG_END;
}

GlobalShadeCorrector::GlobalShadeCorrector(Polyhedron* p,
		ShadeContourData* scd, GSCorrectorParameters* _parameters) :
				PCorrector(p),
				edgeData(),
				contourData(scd),
				parameters(*_parameters),
				facetsNotAssociated(),
				gradient(NULL),
				gradientPrevious(NULL),
				gradientNorm(0.),
				gradientNormPrevious(0.),
				iMinimizedDimension(0),
				iMinimizationLevel(0),
				prevPlanes(NULL),
				dim(0),
				associator()
{
	DEBUG_START;

	/* By default, before beginning the algorithm we set the list of corrected
	 * facets to full list, so the default correction mode is correction of all
	 * facets. */
	for (int iFacet; iFacet < polyhedron->numFacets; ++iFacet)
	{
		facetsCorrected.push_back(iFacet);
	}

	DEBUG_END;
}

GlobalShadeCorrector::~GlobalShadeCorrector()
{
	DEBUG_START;

	if (!facetsNotAssociated.empty())
	{
		DEBUG_PRINT("Deleting facetsNotAssociated");
		facetsNotAssociated.clear();
	}
	if (!facetsCorrected.empty())
	{
		DEBUG_PRINT("Deleting facetsCorrected");
		facetsCorrected.clear();
	}
	if (gradient != NULL)
	{
		DEBUG_PRINT("Deleting gradient");
		delete[] gradient;
		gradient = NULL;
	}
	if (gradientPrevious != NULL)
	{
		DEBUG_PRINT("Deleting gradientPrevious");
		delete[] gradientPrevious;
		gradientPrevious = NULL;
	}
	if (prevPlanes != NULL)
	{
		DEBUG_PRINT("Deleting prevPlanes");
		delete[] prevPlanes;
		prevPlanes = NULL;
	}
	if (associator != NULL)
	{
		DEBUG_PRINT("Deleting associator");
		delete associator;
		associator = NULL;
	}

	DEBUG_END;
}

void GlobalShadeCorrector::setFacetsCorrected(list<int> _facetsCorrected)
{
	DEBUG_START;
	facetsCorrected = _facetsCorrected;
	DEBUG_END;
}

void GlobalShadeCorrector::runCorrection()
{
	DEBUG_START;
	if (parameters.methodName != METHOD_ALL)
	{
		runCorrectionDo();
	}
	else
	{
		/* This branch has been added for testing purposes. We test all
		 * algorithms to compare their performance. */
		Plane* planesInitial = new Plane[polyhedron->numFacets];
		GSCorrectorStatus* status = new
				GSCorrectorStatus[NUMBER_METHOD_CORRECTOR];

		for (int iPlane = 0; iPlane < polyhedron->numFacets; ++iPlane)
		{
			planesInitial[iPlane] = polyhedron->facets[iPlane].plane;
		}

		status[0] = repairAndRun(METHOD_GRADIENT_DESCENT, planesInitial);
		parameters.deltaGradientStep = DEFAULT_MAX_DELTA;
		status[1] = repairAndRun(METHOD_GRADIENT_DESCENT_FAST, planesInitial);
		status[2] = repairAndRun(METHOD_CONJUGATE_GRADIENT, planesInitial);

		/* Begin of printing the report */

		PRINT("# \t|\t method \t\t|\t "
				"exit reason \t\t|\t "
				"value caused exit \t|\t "
				"number of iterations \n");

		for(int iMethod = 0; iMethod < NUMBER_METHOD_CORRECTOR; ++iMethod)
		{
			PRINT("%d \t|\t ", iMethod);
			switch(status[iMethod].parameters.methodName)
			{
			case METHOD_GRADIENT_DESCENT:
				PRINT("gradient descend \t|\t ");
				break;
			case METHOD_GRADIENT_DESCENT_FAST:
				PRINT("fast gradient descend \t|\t ");
				break;
			case METHOD_CONJUGATE_GRADIENT:
				PRINT("conjugate gradient \t|\t");
				break;
			default:
				PRINT("??? \t|\t");
				break;
			}
			switch(status[iMethod].exitReason)
			{
			case GSC_SUCCESS:
				PRINT("success \t|\t ");
				break;
			case GSC_BIG_ERROR_ABSOLUTE:
				PRINT("big abs error \t|\t ");
				break;
			case GSC_BIG_ERROR_RELATIVE:
				PRINT("big rel error \t|\t ");
				break;
			case GSC_SMALL_GRADIENT:
				PRINT("small gradient \t|\t ");
				break;
			case GSC_SMALL_MOVEMENT:
				PRINT("small movement \t|\t ");
				break;
			}
			PRINT("%le \t|\t %d \n", status[iMethod].valueCausedExit,
					status[iMethod].numIterations);
		}

		/* End of printing the report */

		if (planesInitial != NULL)
		{
			delete[] planesInitial;
			planesInitial = NULL;
		}
		if (status != NULL)
		{
			delete[] status;
			status = NULL;
		}
	}
	DEBUG_END;
}

GSCorrectorStatus GlobalShadeCorrector::repairAndRun(MethodCorrector method,
		Plane* planesInitial)
{
	DEBUG_START;
	for (int iPlane = 0; iPlane < polyhedron->numFacets; ++iPlane)
	{
		polyhedron->facets[iPlane].plane = planesInitial[iPlane];
	}
	parameters.methodName = method;
	DEBUG_END;
	return runCorrectionDo();
}

GSCorrectorStatus GlobalShadeCorrector::runCorrectionDo()
{
	DEBUG_START;
	preprocess();
	MAIN_PRINT("Preprocessing done");
	findNotAssociatedFacets();
#ifndef NDEBUG
	DEBUG_PRINT("The following facets have no associations:");
	for (list<int>::iterator iter = facetsNotAssociated.begin();
			iter != facetsNotAssociated.end(); ++iter)
	{
		DEBUG_PRINT("%d", *iter);
	}
#endif
	int numFacetsNotAssociated = facetsNotAssociated.size();
	dim = (facetsCorrected.size() - numFacetsNotAssociated) * 5;

	gradient = new double[dim];
	gradientPrevious = new double[dim];
	prevPlanes = new Plane[polyhedron->numFacets];

	DEBUG_PRINT("memory allocation done");

	for (list<int>::iterator itFacet = facetsCorrected.begin();
			itFacet != facetsCorrected.end(); ++itFacet)
	{
		prevPlanes[*itFacet] = polyhedron->facets[*itFacet].plane;
	}
	DEBUG_PRINT("memory initialization done");

	double error = calculateFunctional();
	double errorInitial = error;
	DEBUG_PRINT("first functional calculation done. error = %e", error);

	int numIterations = 0;
	GSCorrectorStatus status;
	status.parameters = parameters;
	status.exitReason = GSC_SUCCESS;

	while (error > parameters.epsLoopStop)
	{
		MAIN_PRINT(COLOUR_GREEN "Iteration %d : begin\n" COLOUR_NORM,
				numIterations);

#ifndef NDEBUG
		char* fileName = new char[255];
		sprintf(fileName, "./poly-data-out/correction-of-cube/cube%d.txt",
				numIterations);

		polyhedron->my_fprint(fileName);
		delete[] fileName;
#endif

#ifdef DO_EDGES_CHECK
		if (polyhedron->checkEdges(edgeData) > 0)
		{
			DEBUG_PRINT("checkEdges test failed, breaking...");
			break;
		}
#endif

		runCorrectionIteration();

		double movement = 0.;
		for (list<int>::iterator itFacet = facetsCorrected.begin();
				itFacet != facetsCorrected.end(); ++itFacet)
		{
			DEBUG_PRINT(
					"Plane[%d]: (%lf, %lf, %lf, %lf) --> (%lf, %lf, %lf, %lf)",
					*itFacet, prevPlanes[*itFacet].norm.x,
					prevPlanes[*itFacet].norm.y,
					prevPlanes[*itFacet].norm.z,
					prevPlanes[*itFacet].dist,
					polyhedron->facets[*itFacet].plane.norm.x,
					polyhedron->facets[*itFacet].plane.norm.y,
					polyhedron->facets[*itFacet].plane.norm.z,
					polyhedron->facets[*itFacet].plane.dist);


			double oneMovement = qmod(prevPlanes[*itFacet].norm -
					polyhedron->facets[*itFacet].plane.norm);
			oneMovement += (prevPlanes[*itFacet].dist -
					polyhedron->facets[*itFacet].plane.dist) *
					(prevPlanes[*itFacet].dist -
							polyhedron->facets[*itFacet].plane.dist);

			movement += oneMovement;
			prevPlanes[*itFacet] = polyhedron->facets[*itFacet].plane;
		}
		error = calculateFunctional();
		MAIN_PRINT("error = %le", error);
		MAIN_PRINT(COLOUR_GREEN "Iteration %d : End\n" COLOUR_NORM,
				numIterations);
		if (error > MAX_ERROR_ABSOLUTE)
		{
			ERROR_PRINT("Too big absolute value of error.");
			status.exitReason = GSC_BIG_ERROR_ABSOLUTE;
			status.valueCausedExit = error;
			break;
		}
		if (error / errorInitial > MAX_ERROR_RELATIVE)
		{
			ERROR_PRINT("Too big relative value of error.");
			status.exitReason = GSC_BIG_ERROR_RELATIVE;
			status.valueCausedExit = error / errorInitial;
			break;
		}
		if (gradientNorm < EPSILON_MINIMAL_GRADIENT_NORM)
		{
			ERROR_PRINT("Stopping, because gradient is too small");
			status.exitReason = GSC_SMALL_GRADIENT;
			status.valueCausedExit = gradientNorm;
			break;
		}
		if (!(parameters.methodName == METHOD_CONJUGATE_GRADIENT &&
				iMinimizedDimension == 0) &&
				movement < EPSILON_MINIMAL_MOVEMENT)
		{
			ERROR_PRINT("Stopping, because movement is too small");
			status.exitReason = GSC_SMALL_MOVEMENT;
			status.valueCausedExit = movement;
			break;
		}
		++iMinimizedDimension;
		if (iMinimizedDimension == dim)
		{
			iMinimizedDimension = 0;
			++iMinimizationLevel;
		}
		++numIterations;
	}
	status.numIterations = numIterations;
	if (status.exitReason == GSC_SUCCESS)
	{
		status.valueCausedExit = error;
	}
	DEBUG_END;
	return status;
}

void GlobalShadeCorrector::findNotAssociatedFacets()
{
	DEBUG_START;
	for (list<int>::iterator itFacet = facetsCorrected.begin();
			itFacet != facetsCorrected.end(); ++itFacet)
	{
		int* indVertices = polyhedron->facets[*itFacet].indVertices;
		int numVerticesFacet = polyhedron->facets[*itFacet].numVertices;
		int numAssociations = 0;
		for (int iVertex = 0; iVertex < numVerticesFacet; ++iVertex)
		{
			EdgeSetIterator edge =
					edgeData->findEdge(indVertices[iVertex],
					indVertices[iVertex + 1]);
			if (edge != edgeData->edges.end())
			{
				numAssociations += edge->assocList.size();
			}
		}
		if (numAssociations == 0)
		{
			facetsNotAssociated.push_back(*itFacet);
		}
	}
	DEBUG_END;
}

double GlobalShadeCorrector::calculateFunctional()
{
	DEBUG_START;
	double sum = 0;

	EdgeSetIterator edge = edgeData->edges.begin();
	ASSERT(edgeData->numEdges == edgeData->edges.size());

	for (int iEdge = 0; iEdge < edgeData->numEdges; ++iEdge)
	{
//    	DEBUG_PRINT("Processing edge #%d (%d) = [%d, %d] with info f0 = %d, "
//    			"f1 = %d", iEdge, edge->id, edge->v0, edge->v1, edge->f0,
//    			edge->f1);
		int f0 = edge->f0;
		int f1 = edge->f1;

		if (f0 < 0 || f0 >= polyhedron->numFacets)
		{
			ERROR_PRINT("Facet id f0 = %d is out of bounds 0 <= i < %d. It "
					"happened during processing the edge #%d = [%d, %d] "
					"with info f0 = %d, f1 = %d", f0,
					polyhedron->numFacets, iEdge, edge->v0, edge->v1, f0, f1);
			ASSERT(0);
			DEBUG_END;
			return DEFAULT_ERROR_FOR_DOUBLE_FUNCTIONS;
		}

		if (f1 < 0 || f1 >= polyhedron->numFacets)
		{
			ERROR_PRINT("Facet id f1 = %d is out of bounds 0 <= i < %d. It "
					"happened during processing the edge #%d = [%d, %d] "
					"with info f0 = %d, f1 = %d", f1,
					polyhedron->numFacets, iEdge, edge->v0, edge->v1, f0, f1);
			ASSERT(0);
			DEBUG_END;
			return DEFAULT_ERROR_FOR_DOUBLE_FUNCTIONS;
		}

		Plane plane0 = polyhedron->facets[f0].plane;
		Plane plane1 = polyhedron->facets[f1].plane;
		Plane planePrev0 = prevPlanes[f0];
		Plane planePrev1 = prevPlanes[f1];

		int iAssoc = 0;
		for (list<EdgeContourAssociation>::const_iterator itCont =
				edge->assocList.begin();
				itCont != edge->assocList.end(); ++itCont)
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
		++edge;
	}

	DEBUG_END;
	return sum;

}

void GlobalShadeCorrector::shiftCoefficients(double delta)
{
	DEBUG_START;

	list<int>::iterator iterNotAssociated = facetsNotAssociated.begin();
	int countNotAssociated = 0;

#ifndef NDEBUG
	/* For calculating L1, L2 and C norms of gradient */
	double norm_L1 = 0.;
	double norm_L2 = 0.;
	double norm_C = 0.;
#endif /* NDEBUG */

	int iFacetLocal = 0;
	for (list<int>::iterator itFacet = facetsCorrected.begin();
			itFacet != facetsCorrected.end(); ++itFacet, ++iFacetLocal)
	{
		int iFacet = *itFacet;

		if (*iterNotAssociated == iFacet)
		{
			++iterNotAssociated;
			++countNotAssociated;
			continue;
		}
		int iFacetShifted = iFacetLocal - countNotAssociated;
		Plane deltaPlane = Plane(Vector3d(gradient[4 * iFacetShifted],
				gradient[4 * iFacetShifted + 1],
				gradient[4 * iFacetShifted + 2]),
				gradient[4 * iFacetShifted + 3]);
		deltaPlane.norm *= delta;
		deltaPlane.dist *= delta;
		polyhedron->facets[iFacet].plane.norm -= deltaPlane.norm;
		polyhedron->facets[iFacet].plane.dist -= deltaPlane.dist;

#ifndef NDEBUG
		/* Calculate L1, L2 and C norms of gradient */
		double abs_a = fabs(deltaPlane.norm.x);
		double abs_b = fabs(deltaPlane.norm.y);
		double abs_c = fabs(deltaPlane.norm.z);
		double abs_d = fabs(deltaPlane.dist);

		norm_L1 += abs_a + abs_b + abs_c + abs_d;
		norm_L2 += abs_a * abs_a + abs_b * abs_b + abs_c * abs_c +
				abs_d * abs_d;
		norm_C = abs_a > norm_C ? abs_a : norm_C;
		norm_C = abs_b > norm_C ? abs_b : norm_C;
		norm_C = abs_c > norm_C ? abs_c : norm_C;
#endif /* NDEBUG */
	}
#ifndef NDEBUG
	norm_L2 = sqrt(norm_L2);

	/* Print L1, L2 and C norms of gradient*/
	DEBUG_PRINT("NORMS of GRADIENT:");
	DEBUG_PRINT("\tL1 norm :\t%lf", norm_L1);
	DEBUG_PRINT("\tL2 norm :\t%lf", norm_L2);
	DEBUG_PRINT("\tC  norm :\t%lf", norm_C);
#endif /* NDEBUG */


	DEBUG_END;
}

double GlobalShadeCorrector::calculateFunctional(double delta)
{
	DEBUG_START;

	shiftCoefficients(delta);
	double functionalValue = calculateFunctional();
	shiftCoefficients(-delta);

	DEBUG_END;
	return functionalValue;
}

const double GOLDEN_RATIO_RECIPROCAL = 0.5 * (sqrt(5) - 1);
const double INTERVAL_PRECISION = 1e-10;

double GlobalShadeCorrector::findOptimalDelta(double deltaMax)
{
	DEBUG_START;

	double leftBound = 0.;
	double rightBound = deltaMax;
	double leftChecker = rightBound - GOLDEN_RATIO_RECIPROCAL *
			(rightBound - leftBound);
	double rightChecker = leftBound + GOLDEN_RATIO_RECIPROCAL *
			(rightBound - leftBound);
	while (rightBound - leftBound > INTERVAL_PRECISION)
	{
		DEBUG_PRINT("[%lf   (%lf   %lf)   %lf]", leftBound, leftChecker,
				rightChecker, rightBound);
		double leftValue = calculateFunctional(leftChecker);
		double rightValue = calculateFunctional(rightChecker);
		if (leftValue > rightValue)
		{
			DEBUG_PRINT("left = %lf > %lf = right, eliminating left",
					leftValue, rightValue);
			leftBound = leftChecker;
			leftChecker = rightChecker;
			rightChecker = leftBound + GOLDEN_RATIO_RECIPROCAL *
					(rightBound - leftBound);
		}
		else
		{
			DEBUG_PRINT("left = %lf < %lf = right, eliminating right",
					leftValue, rightValue);
			rightBound = rightChecker;
			rightChecker = leftChecker;
			leftChecker = rightBound - GOLDEN_RATIO_RECIPROCAL *
					(rightBound - leftBound);
		}
	}

	DEBUG_END;
	return leftBound;
}

double GlobalShadeCorrector::calculateGradientNorm()
{
	DEBUG_START;

	double norm = 0.;
	for (int iDimension = 0; iDimension < dim; ++iDimension)
	{
		norm += gradient[iDimension] * gradient[iDimension];
	}

	DEBUG_END;
	return norm;
}

void GlobalShadeCorrector::runCorrectionIteration()
{
	DEBUG_START;
	calculateGradient();
	gradientNorm = calculateGradientNorm();

#ifdef GLOBAL_CORRECTION_DERIVATIVE_TESTING
	derivativeTest_all();
#endif

	double deltaOptimal;
	double omega;
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
	case METHOD_CONJUGATE_GRADIENT:
		MAIN_PRINT("iMinimizationLevel = %d, iMinimizedDimension = %d",
				iMinimizationLevel, iMinimizedDimension);
		if (iMinimizedDimension != 0)
		{
			omega = gradientNorm / gradientNormPrevious;
			if (iMinimizedDimension != 1)
			{
				for (int iDimension = 0; iDimension < dim; ++iDimension)
				{
					gradient[iDimension] -= omega *
							gradientPrevious[iDimension];
				}
			}
			deltaOptimal = findOptimalDelta(parameters.deltaGradientStep);
			MAIN_PRINT("deltaOptimal = %lf", deltaOptimal);
			shiftCoefficients(deltaOptimal);
		}
		break;
	case METHOD_ALL:
		ERROR_PRINT("This point is unreachable!");
		break;
	case METHOD_UNKNOWN:
	default:
		ERROR_PRINT("Unknown method name!");
		break;
	}

	/* Saving previous values of gradient: */
	gradientNormPrevious = gradientNorm;
	for (int iDimension = 0; iDimension < dim; ++iDimension)
	{
		gradientPrevious[iDimension] = gradient[iDimension];
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

	list<int>::iterator iterNotAssocicated = facetsNotAssociated.begin();
	int countNotAssociated = 0;

	int iFacetLocal = 0;
	for (list<int>::iterator itFacet = facetsCorrected.begin();
			itFacet != facetsCorrected.end(); ++itFacet, ++iFacetLocal)
	{
		int iFacet = *itFacet;

		if (*iterNotAssocicated == iFacet)
		{
			++iterNotAssocicated;
			++countNotAssociated;
			continue;
		}
		int iFacetShifted = iFacetLocal - countNotAssociated;

		int nv = polyhedron->facets[iFacet].numVertices;
		int *index = polyhedron->facets[iFacet].indVertices;

		Plane planePrevThis = prevPlanes[iFacetLocal];

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

			EdgeSetIterator edge = edgeData->findEdge(v0, v1);

			DEBUG_PRINT("Searching edge: [%d, %d] ; Found edge: [%d, %d]",
					v0, v1, edge->v0, edge->v1);

			if (edge == edgeData->edges.end())
			{
				ERROR_PRINT("Error! edge [%d, %d] cannot be found\n", v0, v1);

				EdgeSetIterator edgeDumped = edgeData->edges.begin();
				for (int iEdge = 0; iEdge < edgeData->numEdges; ++iEdge)
				{
					if ((edgeDumped->v0 == v0 && edgeDumped->v1 == v1) ||
							(edgeDumped->v0 == v1 && edgeDumped->v1 == v0))
					{
						DEBUG_PRINT("It presents: [%d, %d]",
								edgeDumped->v0, edgeDumped->v1);
					}
					++edgeDumped;
				}

				ASSERT(edge != edgeData->edges.end());
				return;
			}

			ASSERT((edge->v0 == v0 && edge->v1 == v1) ||
					(edge->v0 == v1 && edge->v1 == v0));

			int iAssociation = 0;
			for (list<EdgeContourAssociation>::const_iterator itCont =
					edge->assocList.begin();
					itCont != edge->assocList.end(); ++itCont)
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
				ASSERT(!(fabs(denominator) < EPS_MIN_DOUBLE));

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
