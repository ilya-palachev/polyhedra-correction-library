/*
 * GSAssociator.cpp
 *
 *  Created on: 12.05.2013
 *      Author: iliya
 */

#include "PolyhedraCorrectionLibrary.h"

const double ASSOCIATOR_MIN_PORTION_REL = 0.1;
const double EPS_SAME_POINTS = 1e-16;
const double EPSILON_EDGE_CONTOUR_VISIBILITY = 1e-6;

static double distVertexEdge(
// this routine calculates distance
		Vector3d A, // from this vector
		Vector3d A1, // to the edge [A1, A2]
		Vector3d A2, Vector3d& A_nearest);

static double weightForAssociations(double x)
{
	DEBUG_START;
	DEBUG_END;
	return x * x;
}

const double MIN_DIST_MIN = 0.1;
const int NO_SIDE_FOUND = -1;

GSAssociator::GSAssociator(GlobalShadeCorrector* corrector) :
				GlobalShadeCorrector(*corrector),
				iContour(INT_NOT_INITIALIZED),
				iFacet(INT_NOT_INITIALIZED),
				iEdge(INT_NOT_INITIALIZED),
				edge(NULL),
				polyhedronTmp(NULL),
				bufDouble(NULL),
				v0_projected(),
				v1_projected(),
				v0_nearest(),
				v1_nearest(),
				iSideDistMin0(INT_NOT_INITIALIZED),
				iSideDistMin1(INT_NOT_INITIALIZED)
{
	DEBUG_START;
	DEBUG_END;
}

GSAssociator::~GSAssociator()
{
	DEBUG_START;
	if (bufDouble)
	{
		delete[] bufDouble;
		bufDouble = NULL;
	}
	DEBUG_END;
}

void GSAssociator::preinit()
{
	DEBUG_START;
	int numSidesMax = 0;
	for (int iContour = 0; iContour < contourData->numContours; ++iContour)
	{
		DEBUG_PRINT("contourData->contours[%d].ns = %d", iContour,
				contourData->contours[iContour].ns);
		int numSidesCurr = contourData->contours[iContour].ns;
		if (numSidesCurr > numSidesMax)
			numSidesMax = numSidesCurr;
	}
	int numVerticesTmp = 2 * numSidesMax + 2;
	int numFacetsTmp = 3;
	polyhedronTmp = new Polyhedron(numVerticesTmp, numFacetsTmp);
	bufDouble = new double[numSidesMax + 1];
	edge = edgeData->edges.begin();
	DEBUG_END;
}

const double EPSILON_MIN_AREA_ABS = 1e-3;
const double EPSILON_MIN_AREA_REL = 0.2;

void GSAssociator::run(int iContourIn, int iFacetIn, int iEdgeIn)
{
	DEBUG_START;

	iContour = iContourIn;
	iFacet = iFacetIn;
	iEdge = iEdgeIn;

	DEBUG_PRINT("processing contour # %d, facet # %d, edge # %d", iContour,
			iFacet, iEdge);

	if (init() != EXIT_SUCCESS)
		return;

	double distMin0;
	iSideDistMin0 = findNearestPoint(v0_projected, v0_nearest, distMin0);

	double distMin1;
	iSideDistMin1 = findNearestPoint(v1_projected, v1_nearest, distMin1);

	double edgeProjectionLength = sqrt(qmod(v0_projected - v1_projected));
	if (distMin0 + distMin1 > edgeProjectionLength)
	{
		DEBUG_PRINT("Edge is too far from contour's border: %lf + %lf > %lf",
				distMin0, distMin1, edgeProjectionLength);
		DEBUG_END;
		return;
	}

	double areaLeft = calculateArea(ORIENTATION_LEFT);
	double areaRight = calculateArea(ORIENTATION_RIGHT);
	double areaTotal = areaLeft + areaRight;

	DEBUG_PRINT("areaLeft = %lf, areaRight = %lf, areaTotal = %lf", areaLeft,
			areaRight, areaTotal);

	if (areaTotal < EPSILON_FOR_DIVISION)
	{
		ERROR_PRINT("Too small shade contour, total area = %le", areaTotal);
		DEBUG_END;
		return;
	}

	bool criteriaLeft = (areaLeft < EPSILON_MIN_AREA_ABS)
			|| (areaLeft / areaTotal < EPSILON_MIN_AREA_REL);

	bool criteriaRight = (areaRight < EPSILON_MIN_AREA_ABS)
			|| (areaRight / areaTotal < EPSILON_MIN_AREA_REL);

	DEBUG_PRINT("criteriaLeft = %d, criteriaRight = %d", criteriaLeft,
			criteriaRight);

	if ((criteriaLeft && !criteriaRight) || (!criteriaLeft && criteriaRight))
	{
		Orientation orientation =
				criteriaLeft ? ORIENTATION_LEFT : ORIENTATION_RIGHT;
		add(orientation);
	}

	++edge;
	DEBUG_END;
}

int GSAssociator::init()
{
	DEBUG_START;
	if (checkVisibility() != EXIT_SUCCESS)
	{
		DEBUG_END;
		return EXIT_FAILURE;
	}

	if (checkAlreadyAdded() != EXIT_SUCCESS)
	{
		DEBUG_END;
		return EXIT_FAILURE;
	}

	projectEdge();

	if (checkExtinction() != EXIT_SUCCESS)
	{
		DEBUG_END;
		return EXIT_FAILURE;
	}

	int numSides = contourData->contours[iContour].ns;
	int numVerticesAdded = 0;
	for (int iSide = 0; iSide < numSides; ++iSide)
	{
		polyhedronTmp->vertices[numVerticesAdded++] =
				contourData->contours[iContour].sides[iSide].A1;
		polyhedronTmp->vertices[numVerticesAdded++] =
				contourData->contours[iContour].sides[iSide].A2;
	}
	polyhedronTmp->numVertices = numVerticesAdded;
	polyhedronTmp->my_fprint(stdout);

	DEBUG_END;
	return EXIT_SUCCESS;
}

int GSAssociator::checkVisibility()
{
	DEBUG_START;
	double visibility = calculateVisibility();
	if (visibility >= EPSILON_EDGE_CONTOUR_VISIBILITY)
	{
		DEBUG_PRINT("Edge is visible on this contour");
		DEBUG_PRINT("\t visibility = %lf", visibility);
		DEBUG_END;
		return EXIT_SUCCESS;
	}
	else
	{
		DEBUG_PRINT("Edge is INVISIBLE on this contour");
		DEBUG_PRINT("\t visibility = %lf", visibility);
		DEBUG_END;
		return EXIT_FAILURE;
	}
}

double GSAssociator::calculateVisibility()
{
	DEBUG_START;

	int f0 = edge->f0;
	int f1 = edge->f1;
	Vector3d norm0 = polyhedron->facets[f0].plane.norm;
	norm0.norm(1.);
	Vector3d norm1 = polyhedron->facets[f1].plane.norm;
	norm1.norm(1.);
	Vector3d directionOfProjection = contourData->contours[iContour].plane.norm;
	directionOfProjection.norm(1.);

	double angle0 = acos(directionOfProjection * norm0);
	double angle1 = acos(directionOfProjection * norm1);

	double beta0 = angle0 - 0.5 * M_PI;
	double beta1 = angle1 - 0.5 * M_PI;

	double sum = beta1 + beta0;
	double sub = beta1 - beta0;

	double visibility;

	if (sum >= 0.)
	{
		if (sub >= 0.)
		{
			visibility = -beta0;
		}
		else
		{
			visibility = -beta1;
		}
	}
	else
	{
		if (sub >= 0.)
		{
			visibility = beta1;
		}
		else
		{
			visibility = beta0;
		}
	}

	DEBUG_END;
	return visibility;
}

int GSAssociator::checkAlreadyAdded()
{
	DEBUG_START;
// Check whether this association has been already added to the list
	int numAlreadyPushedAssocs = edge->assocList.size();
	EdgeContourAssociation lastCont = edge->assocList.back();
	int iContourLastInList = lastCont.indContour;
	DEBUG_PRINT("iContourLastInList = %d, numAlreadyPushedAssocs = %d",
			iContourLastInList, numAlreadyPushedAssocs);
	if (numAlreadyPushedAssocs != 0 && iContourLastInList == iContour)
	{
		DEBUG_PRINT("Edge %d already has association with contour # %d", iEdge,
				iContour);
		DEBUG_END;
		return EXIT_FAILURE;
	}
	DEBUG_END;
	return EXIT_SUCCESS;
}

int GSAssociator::checkExtinction()
{
	DEBUG_START;
	int iVertex0 = edge->v0;
	int iVertex1 = edge->v1;
	if (qmod(v0_projected - v1_projected) < EPS_SAME_POINTS)
	{
		DEBUG_PRINT("Edge # %d (%d, %d) is reduced into point when projecting",
				iEdge, iVertex0, iVertex1);
		DEBUG_PRINT("on the plane of projection of contour # %d", iContour);
		DEBUG_END;
		return EXIT_FAILURE;
	}
	DEBUG_END;
	return EXIT_SUCCESS;
}

void GSAssociator::projectEdge()
{
	DEBUG_START;
	int iVertex0 = edge->v0;
	int iVertex1 = edge->v1;
	Vector3d v0 = polyhedron->vertices[iVertex0];
	Vector3d v1 = polyhedron->vertices[iVertex1];
	Plane planeOfProjection = contourData->contours[iContour].plane;
	DEBUG_PRINT("v0 before projection: (%lf, %lf, %lf)", v0.x, v0.y, v0.z);
	DEBUG_PRINT("v1 before projection: (%lf, %lf, %lf)", v1.x, v1.y, v1.z);
	v0_projected = planeOfProjection.project(v0);
	v1_projected = planeOfProjection.project(v1);
	DEBUG_PRINT("v0 after projection: (%lf, %lf, %lf)", v0_projected.x,
			v0_projected.y, v0_projected.z);
	DEBUG_PRINT("v1 after projection: (%lf, %lf, %lf)", v1_projected.x,
			v1_projected.y, v1_projected.z);
	DEBUG_END;
}

int GSAssociator::findNearestPoint(Vector3d v_projected, Vector3d& v_nearest,
		double& distMin)
{
	DEBUG_START;
	SideOfContour* sides = contourData->contours[iContour].sides;
	int numSides = contourData->contours[iContour].ns;
	Vector3d v_nearestCurr;
	int iSideDistMin;

	for (int iSide = 0; iSide < numSides; ++iSide)
	{
		Vector3d v0 = sides[iSide].A1;
		Vector3d v1 = sides[iSide].A2;
		DEBUG_PRINT("finding dist from (%lf, %lf, %lf) to edge", v_projected.x,
				v_projected.y, v_projected.z);
		DEBUG_PRINT("  [ (%lf, %lf, %lf) (%lf, %lf, %lf)]", v0.x, v0.y, v0.z,
				v1.x, v1.y, v1.z);
		double distCurr = distVertexEdge(v_projected, v0, v1, v_nearestCurr);
		DEBUG_PRINT("dist to side %d  = %lf", iSide, distCurr);
		if (iSide == 0 || distCurr < distMin)
		{
			iSideDistMin = iSide;
			distMin = distCurr;
			v_nearest = v_nearestCurr;
		}
	}
	DEBUG_END;
	return iSideDistMin;
}

static double distVertexEdge(
// this routine calculates distance
		Vector3d A, // from this vector
		Vector3d A1, // to the edge [A1, A2]
		Vector3d A2, Vector3d& A_nearest)
{

	DEBUG_START;
	double dist;
	Vector3d edge = A2 - A1;
	if ((A1 - A) * edge > 0 && (A2 - A) * edge > 0)
	{
		dist = sqrt(qmod(A1 - A));
	}
	else if ((A1 - A) * edge < 0 && (A2 - A) * edge < 0)
	{
		dist = sqrt(qmod(A2 - A));
	}
	else
	{

		Vector3d step = edge * 0.5;
		Vector3d Aproj = A1 + step;
		do
		{
			step *= 0.5;
			double scalarProd = (Aproj - A) * edge;
			if (scalarProd < EPS_COLLINEARITY && scalarProd > -EPS_COLLINEARITY)
			{
				break;
			}
			else if (scalarProd > 0)
			{
				Aproj -= step;
			}
			else if (scalarProd < 0)
			{
				Aproj += step;
			}
		} while (1);

		dist = sqrt(qmod(A - Aproj));
		A_nearest = Aproj;
	}
	DEBUG_END;
	return dist;
}

enum facetPart
{
	FACET_WHOLE, FACET_LEFT, FACET_RIGHT
};

double GSAssociator::calculateArea(Orientation orientation)
{
	DEBUG_START;

	int numVerticesTmp = polyhedronTmp->numVertices;
	const int iNearest0 = numVerticesTmp + 1;
	const int iNearest1 = numVerticesTmp + 2;

	polyhedronTmp->vertices[iNearest0] = v0_nearest;
	polyhedronTmp->vertices[iNearest1] = v1_nearest;

	facetPart fPart;
	int numSides = contourData->contours[iContour].ns;
	int numVerticesFacet = 0;
	int numPairsToBeAdded;
	int iStep;

	int numLeft = (numSides + iSideDistMin1 - iSideDistMin0) % numSides;

	switch (orientation)
	{
	case ORIENTATION_LEFT:
		DEBUG_PRINT("ORIENTATION_LEFT");
		fPart = FACET_LEFT;
		iStep = -1;
		numPairsToBeAdded = numLeft + 1;
		break;
	case ORIENTATION_RIGHT:
		DEBUG_PRINT("ORIENTATION_RIGHT");
		fPart = FACET_RIGHT;
		iStep = 1;
		numPairsToBeAdded = numSides - numLeft + 1;
		break;
	}

	numVerticesFacet = 2 * numPairsToBeAdded;
	DEBUG_PRINT("numVerticesFacet = %d", numVerticesFacet);
	polyhedronTmp->facets[fPart] = Facet(fPart, numVerticesFacet,
			contourData->contours[iContour].plane, polyhedronTmp);

	polyhedronTmp->facets[fPart].set_ind_vertex(0, iNearest0);
	polyhedronTmp->facets[fPart].set_ind_vertex(1, iNearest1);

	int iBegin = iSideDistMin1;
	int iEnd = iSideDistMin0;

	DEBUG_PRINT("iStep = %d", iStep);
	DEBUG_PRINT("iBegin = %d", iBegin);
	DEBUG_PRINT("iEnd = %d", iEnd);
	DEBUG_PRINT("iSideDistMin0 = %d", iSideDistMin0);
	DEBUG_PRINT("iSideDistMin1 = %d", iSideDistMin1);
	DEBUG_PRINT("numPairsToBeAdded = %d", numPairsToBeAdded);

	int iSide = iBegin;

	for (int iPairsAdded = 1; iPairsAdded < numPairsToBeAdded; ++iPairsAdded)
	{
		int iAdded0, iAdded1;
		switch (orientation)
		{
		case ORIENTATION_LEFT:
			iAdded0 = 2 * iSide;
			iAdded1 = 2 * iSide - 1;
			break;
		case ORIENTATION_RIGHT:
			iAdded0 = 2 * iSide + 1;
			iAdded1 = 2 * iSide + 2;
			break;
		}
		iAdded0 %= numVerticesTmp;
		iAdded1 %= numVerticesTmp;

		polyhedronTmp->facets[fPart].set_ind_vertex(2 * iPairsAdded, iAdded0);
		polyhedronTmp->facets[fPart].set_ind_vertex(2 * iPairsAdded + 1,
				iAdded1);
		iSide = (numSides + iSide + iStep) % numSides;
	}
	DEBUG_END;
	return fabs(polyhedronTmp->facets[fPart].area());
}

void GSAssociator::add(Orientation orientation)
{
	DEBUG_START;

	int iStep;
	switch (orientation)
	{
	case ORIENTATION_LEFT:
		iStep = -1;
		DEBUG_PRINT("ORIENTATION_LEFT");
		break;
	case ORIENTATION_RIGHT:
		iStep = 1;
		DEBUG_PRINT("ORIENTATION_RIGHT");
		break;
	}
	int numSides = contourData->contours[iContour].ns;
	SideOfContour* sides = contourData->contours[iContour].sides;

	int iBeginToBeAdded, iEndToBeAdded;
	findBounds(orientation, iBeginToBeAdded, iEndToBeAdded);

	int iVertex0 = edge->v0;
	int iVertex1 = edge->v1;
	Vector3d edgeVector = polyhedron->vertices[iVertex1]
			- polyhedron->vertices[iVertex0];
	double weight = calculateWeight();

	DEBUG_PRINT("Starting addition");
	for (int iSide = iBeginToBeAdded; iSide != iEndToBeAdded;
			iSide = (numSides + iSide + iStep) % numSides)
	{
		Vector3d side = sides[iSide].A2 - sides[iSide].A1;
		bool ifDirectionIsProper = edgeVector * side > 0;
		DEBUG_PRINT("Adding contour # %d to the assoc list of edgeVector %d",
				iContour, iEdge);
		DEBUG_PRINT("\tadding side # %d", iSide);
		// Create new association
		EdgeContourAssociation* assocForCurrentEdge =
				new EdgeContourAssociation(iContour, iSide, ifDirectionIsProper,
						weight);
		// Push it to the list
		edge->assocList.push_back(*assocForCurrentEdge);
	}
	DEBUG_PRINT("Addition done");
	DEBUG_END;

}

void GSAssociator::findBounds(Orientation orientation, int& iResultBegin,
		int& iResultEnd)
{
	DEBUG_START;
	int numSides = contourData->contours[iContour].ns;
	SideOfContour* sides = contourData->contours[iContour].sides;

	int iStep;
	int numSidesProcessed;
	switch (orientation)
	{
	case ORIENTATION_LEFT:
		iStep = -1;
		numSidesProcessed = (numSides + iSideDistMin1 - iSideDistMin0)
				% numSides;
		DEBUG_PRINT("ORIENTATION_LEFT");
		break;
	case ORIENTATION_RIGHT:
		iStep = 1;
		numSidesProcessed = (numSides + iSideDistMin0 - iSideDistMin1)
				% numSides;
		DEBUG_PRINT("ORIENTATION_RIGHT");
		break;
	}

	int iBegin = iSideDistMin1;
	int iEnd = (numSides + iSideDistMin0 + iStep) % numSides;

	double* lengthSide = bufDouble;

	DEBUG_PRINT("Starting calculating length of chain");
	double lengthChain = 0.;
	int iLengthSide = 0;
	for (int iSide = iBegin; iSide != iEnd;
			iSide = (numSides + iSide + iStep) % numSides, ++iLengthSide)
	{
		DEBUG_PRINT("iLengthSide = %d", iLengthSide);
		Vector3d A1 = sides[iSide].A1;
		DEBUG_PRINT("A1 = (%lf, %lf, %lf)", A1.x, A1.y, A1.z);
		Vector3d A2 = sides[iSide].A2;
		DEBUG_PRINT("A2 = (%lf, %lf, %lf)", A2.x, A2.y, A2.z);
		if (iSide == iSideDistMin1)
		{
			switch (orientation)
			{
			case ORIENTATION_LEFT:
				A2 = v1_nearest;
				DEBUG_PRINT("A2 --> v0_nearest = (%lf, %lf, %lf)", A2.x, A2.y,
						A2.z);
				break;
			case ORIENTATION_RIGHT:
				A1 = v1_nearest;
				DEBUG_PRINT("A1 --> v0_nearest = (%lf, %lf, %lf)", A1.x, A1.y,
						A1.z);
				break;
			}
		}
		if (iSide == iSideDistMin0)
		{
			switch (orientation)
			{
			case ORIENTATION_LEFT:
				A1 = v0_nearest;
				DEBUG_PRINT("A1 --> v1_nearest = (%lf, %lf, %lf)", A1.x, A1.y,
						A1.z);
				break;
			case ORIENTATION_RIGHT:
				A2 = v0_nearest;
				DEBUG_PRINT("A2 --> v1_nearest = (%lf, %lf, %lf)", A2.x, A2.y,
						A2.z);
				break;
			}
		}
		double lengthSideCurr = sqrt(qmod(A2 - A1));
		lengthSide[iLengthSide] = lengthSideCurr;
		DEBUG_PRINT("lengthSideCurr = %lf", lengthSideCurr);

		lengthChain += lengthSideCurr;
	}
	DEBUG_PRINT("lengthChain = %lf", lengthChain);

	DEBUG_PRINT("Starting calculating iBeginToBeAdded");
	int iBeginToBeAdded = INT_NOT_INITIALIZED;
	iLengthSide = 0;
	for (int iSide = iBegin;;
			iSide = (numSides + iSide + iStep) % numSides, ++iLengthSide)
	{
		DEBUG_PRINT("iLengthSide = %d", iLengthSide);
		double lengthSideCurr = lengthSide[iLengthSide];
		if (lengthSideCurr > ASSOCIATOR_MIN_PORTION_REL * lengthChain)
		{
			iBeginToBeAdded = iSide;
			break;
		}
	}
	DEBUG_PRINT("iBeginToBeAdded = %d", iBeginToBeAdded);

	DEBUG_PRINT("Starting calculating iEndToBeAdded");
	double lengthVisited = 0.;
	int iEndToBeAdded = INT_NOT_INITIALIZED;
	for (int iSide = iBeginToBeAdded;;
			iSide = (numSides + iSide + iStep) % numSides, ++iLengthSide)
	{
		DEBUG_PRINT("iLengthSide = %d", iLengthSide);
		double lengthSideCurr = lengthSide[iLengthSide];
		lengthVisited += lengthSideCurr;

		if (lengthVisited > (1 - ASSOCIATOR_MIN_PORTION_REL) * lengthChain)
		{
			iEndToBeAdded = iSide;
			break;
		}
	}
	iEndToBeAdded = (numSides + iEndToBeAdded + iStep) % numSides;
	DEBUG_PRINT("iEndToBeAdded = %d", iEndToBeAdded);

	iResultBegin = iBeginToBeAdded;
	iResultEnd = iEndToBeAdded;

	DEBUG_END;
}

double GSAssociator::calculateWeight()
{
	DEBUG_START;
	double visibility = calculateVisibility();
	double weight = weightForAssociations(visibility);
	DEBUG_PRINT("visibility/weight = %lf / %lf", visibility, weight);
	DEBUG_END;
	return weight;
}
