#include "PolyhedraCorrectionLibrary.h"

//#define NDEBUG

static double distVertexEdge(Vector3d A, Vector3d A1, Vector3d A2,
		Vector3d& A_nearest);

static double weightForAssociations(double x)
{
	return x * x;
}

void Polyhedron::corpol_preprocess()
{
	DEBUG_START;
	preprocess_edges();
	corpol_prepFindAssociations();
	DEBUG_END;
}

void Polyhedron::preprocess_edges()
{
	DEBUG_START;
	int numEdgesMax = numEdges = 0;
	for (int iFacet = 0; iFacet < numFacets; ++iFacet)
	{
		numEdgesMax += facets[iFacet].numVertices;
	}
	numEdgesMax /= 2;
	DEBUG_PRINT("numEdgesMax = %d", numEdgesMax);
	for (int i = 0; i < numFacets; ++i)
	{
		int numVerticesInFacet = facets[i].numVertices;
		int * index = facets[i].indVertices;
		for (int iVertex = 0; iVertex < numVerticesInFacet; ++iVertex)
		{
			preprocess_edges_add(numEdgesMax, // Number of edges which we are going add finally
					index[iVertex], // First vertices
					index[iVertex + 1], // Second vertices
					i, // Current facets id
					index[numVerticesInFacet + 1 + iVertex]); // Id of its neighbor
		}
	}

	for (int iEdge = 0; iEdge < numEdges; ++iEdge)
	{
		edges[iEdge].id = iEdge;
	}

	for (int i = 0; i < numEdges; ++i)
	{
		edges[i].my_fprint(stdout);
	}
	DEBUG_END;
}

void Polyhedron::corpol_prepFindAssociations()
{
	DEBUG_START;
	Polyhedron* polyhedronTmp;
	corpol_prepFindAssociations_init(polyhedronTmp);

	for (int iContour = 0; iContour < numContours; ++iContour)
	{
		corpol_prepFindAssiciations_withContour(iContour, polyhedronTmp);
	}

	/* Print found associations : */
	for (int iEdge = 0; iEdge < numEdges; ++iEdge)
	{
		edges[iEdge].my_fprint(stdout);
	}
	DEBUG_END;
}

void Polyhedron::corpol_prepFindAssociations_init(Polyhedron* &polyhedronTmp)
{
	DEBUG_START;
	int numSidesMax = 0;
	for (int iContour = 0; iContour < numContours; ++iContour)
	{
		int numSidesCurr = contours[iContour].ns;
		if (numSidesCurr > numSidesMax)
			numSidesMax = numSidesCurr;
	}
	int numVerticesTmp = 2 * numSidesMax + 2;
	int numFacetsTmp = 3;
	polyhedronTmp = new Polyhedron(numVerticesTmp, numFacetsTmp);
	DEBUG_END;
}

void Polyhedron::corpol_prepFindAssiciations_withContour(int iContour,
		Polyhedron* polyhedronTmp)
{
	DEBUG_START;
	for (int iFacet = 0; iFacet < numFacets; ++iFacet)
	{
		corpol_prepFindAssociations_withContour_forFacet(iContour, iFacet,
				polyhedronTmp);
	}
	DEBUG_END;
}

void Polyhedron::corpol_prepFindAssociations_withContour_forFacet(int iContour,
		int iFacet, Polyhedron* polyhedronTmp)
{
	DEBUG_START;
	int numVertex = facets[iFacet].numVertices;
	int* indVertices = facets[iFacet].indVertices;
	for (int iVertex = 0; iVertex < numVertex; ++iVertex)
	{
		int iEdge = preprocess_edges_find(indVertices[iVertex],
				indVertices[iVertex + 1]);
		corpol_prepAssociator(iContour, iFacet, iEdge, polyhedronTmp);
	}
	DEBUG_END;
}

double Polyhedron::corpol_weightForAssociation(int iContour, int iEdge)
{
	double visibility = corpol_prepAssociator_visibility(iContour, iEdge);
	return weightForAssociations(visibility);
}

const double EPSILON_MIN_AREA_ABS = 1e-3;
const double EPSILON_MIN_AREA_REL = 0.2;

void Polyhedron::corpol_prepAssociator(int iContour, int iFacet, int iEdge,
		Polyhedron* polyhedronTmp)
{
	DEBUG_START;
	DEBUG_PRINT("processing contour # %d, facet # %d, edge # %d", iContour,
			iFacet, iEdge);

	Vector3d v0_projected, v1_projected;
	bool checkInit = corpol_prepAssociator_init(iContour, iFacet, iEdge,
			polyhedronTmp, v0_projected, v1_projected) == EXIT_SUCCESS;
	if (!checkInit)
		return;

	Vector3d v0_nearest, v1_nearest;

	int iSideDistMin0 = corpol_prepAssociator_findNearest(iContour,
			v0_projected, v0_nearest);
	int iSideDistMin1 = corpol_prepAssociator_findNearest(iContour,
			v1_projected, v1_nearest);
	double areaLeft = corpol_prepAssociator_calcArea(iContour, polyhedronTmp,
			iSideDistMin0, iSideDistMin1, v0_nearest, v1_nearest,
			ORIENTATION_LEFT);
	double areaRight = corpol_prepAssociator_calcArea(iContour, polyhedronTmp,
			iSideDistMin0, iSideDistMin1, v0_nearest, v1_nearest,
			ORIENTATION_RIGHT);
	double areaTotal = areaLeft + areaRight;

	bool criteriaLeft = (areaLeft < EPSILON_MIN_AREA_ABS)
			|| (areaLeft / areaTotal < EPSILON_MIN_AREA_REL);

	bool criteriaRight = (areaRight < EPSILON_MIN_AREA_ABS)
			|| (areaRight / areaTotal < EPSILON_MIN_AREA_REL);

	DEBUG_PRINT("areaLeft = %lf, areaRight = %lf, areaTotal = %lf", areaLeft,
			areaRight, areaTotal);
	DEBUG_PRINT("criteriaLeft = %d, criteriaRight = %d", criteriaLeft,
			criteriaRight);

	if ((criteriaLeft && !criteriaRight) || (!criteriaLeft && criteriaRight))
	{
		Orientation orientation =
				criteriaLeft ? ORIENTATION_LEFT : ORIENTATION_RIGHT;
		corpol_prepAssociator_add(iContour, iFacet, iEdge, iSideDistMin0,
				iSideDistMin1, v0_nearest, v1_nearest, orientation);
	}
	DEBUG_END;
}

int Polyhedron::corpol_prepAssociator_init(int iContour, int iFacet, int iEdge,
		Polyhedron* polyhedronTmp, Vector3d& v0_projected,
		Vector3d& v1_projected)
{
	DEBUG_START;
	bool checkVisibility = corpol_prepAssociator_checkVisibility(iContour,
			iFacet, iEdge) == EXIT_SUCCESS;
	if (!checkVisibility)
		return EXIT_FAILURE;

	bool checkAlreadyAdded = corpol_prepAssociator_checkAlreadyAdded(iContour,
			iFacet, iEdge) == EXIT_SUCCESS;
	if (!checkAlreadyAdded)
		return EXIT_FAILURE;

	corpol_prepAssociator_project(iContour, iFacet, iEdge, v0_projected,
			v1_projected);

	bool checkExtinction = corpol_prepAssociator_checkExtinction(iContour,
			iFacet, iEdge, v0_projected, v1_projected) == EXIT_SUCCESS;
	if (!checkExtinction)
		return EXIT_FAILURE;

	int numSides = contours[iContour].ns;
	int numVerticesAdded = 0;
	for (int iSide = 0; iSide < numSides; ++iSide)
	{
		polyhedronTmp->set_vertex(numVerticesAdded++,
				contours[iContour].sides[iSide].A1);
		polyhedronTmp->set_vertex(numVerticesAdded++,
				contours[iContour].sides[iSide].A2);
	}
	polyhedronTmp->numVertices = numVerticesAdded;
	polyhedronTmp->my_fprint(stdout);

	DEBUG_END;
	return EXIT_SUCCESS;
}

double Polyhedron::corpol_prepAssociator_visibility(int iContour, int iEdge)
{
	int f0 = edges[iEdge].f0;
	int f1 = edges[iEdge].f1;
	Vector3d norm0 = facets[f0].plane.norm;
	norm0.norm(1.);
	Vector3d norm1 = facets[f1].plane.norm;
	norm1.norm(1.);
	Vector3d directionOfProjection = contours[iContour].plane.norm;
	directionOfProjection.norm(1.);

	double angle0 = acos(directionOfProjection * norm0);
	double angle1 = acos(directionOfProjection * norm1);

	double beta0 = angle0 - 0.5 * M_PI;
	double beta1 = angle1 - 0.5 * M_PI;

	double sum = beta1 + beta0;
	double sub = beta1 - beta0;

	if (sum >= 0.)
	{
		if (sub >= 0.)
		{
			return -beta0;
		}
		else
		{
			return -beta1;
		}
	}
	else
	{
		if (sub >= 0.)
		{
			return beta1;
		}
		else
		{
			return beta0;
		}
	}
}

int Polyhedron::corpol_prepAssociator_checkVisibility(int iContour, int iFacet,
		int iEdge)
{
	DEBUG_START;
	double visibility = corpol_prepAssociator_visibility(iContour, iEdge);
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

int Polyhedron::corpol_prepAssociator_checkAlreadyAdded(int iContour,
		int iFacet, int iEdge)
{
	DEBUG_START;
// Check whether this association has been already added to the list
	int numAlreadyPushedAssocs = edges[iEdge].assocList.size();
	list<EdgeContourAssociation>::iterator lastCont =
			edges[iEdge].assocList.end();
	--lastCont;
	int iContourLastInList = lastCont->indContour;
	DEBUG_PRINT("iContourLastInList = %d, numAlreadyPushedAssocs = %d",
			iContourLastInList, numAlreadyPushedAssocs);
	if (numAlreadyPushedAssocs != 0 && iContourLastInList == iContour)
	{
		DEBUG_PRINT("Edge %d already has association with contour # %d", iEdge,
				iContour);
		return EXIT_FAILURE;
	}
	DEBUG_END;
	return EXIT_SUCCESS;
}

int Polyhedron::corpol_prepAssociator_checkExtinction(int iContour, int iFacet,
		int iEdge, Vector3d v0_projected, Vector3d v1_projected)
{
	DEBUG_START;
	int iVertex0 = edges[iEdge].v0;
	int iVertex1 = edges[iEdge].v1;
	if (qmod(v0_projected - v1_projected) < EPS_SAME_POINTS)
	{
		DEBUG_PRINT("Edge # %d (%d, %d) is reduced into point when projecting",
				iEdge, iVertex0, iVertex1);
		DEBUG_PRINT("on the plane of projection of contour # %d", iContour);
		return EXIT_FAILURE;
	}
	DEBUG_END;
	return EXIT_SUCCESS;
}

void Polyhedron::corpol_prepAssociator_project(int iContour, int iFacet,
		int iEdge, Vector3d& v0_projected, Vector3d& v1_projected)
{
	DEBUG_START;
	int iVertex0 = edges[iEdge].v0;
	int iVertex1 = edges[iEdge].v1;
	Vector3d v0 = vertices[iVertex0];
	Vector3d v1 = vertices[iVertex1];
	Plane planeOfProjection = contours[iContour].plane;
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

int Polyhedron::corpol_prepAssociator_findNearest(int iContour,
		Vector3d v_projected, Vector3d& v_nearest)
{

	DEBUG_START;
	SideOfContour* sides = contours[iContour].sides;
	int numSides = contours[iContour].ns;
	double distMin;
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
	ASSERT(distMin < 0.1);
	DEBUG_END;
	return iSideDistMin;
}

enum facetPart
{
	FACET_WHOLE, FACET_LEFT, FACET_RIGHT
};

double Polyhedron::corpol_prepAssociator_calcArea(int iContour,
		Polyhedron* polyhedronTmp, int iSideDistMin0, int iSideDistMin1,
		Vector3d v0_nearest, Vector3d v1_nearest, Orientation orientation)
{
	DEBUG_START;

	int numVerticesTmp = polyhedronTmp->numVertices;
	const int iNearest0 = numVerticesTmp + 1;
	const int iNearest1 = numVerticesTmp + 2;

	polyhedronTmp->vertices[iNearest0] = v0_nearest;
	polyhedronTmp->vertices[iNearest1] = v1_nearest;

	facetPart fPart;
	int numSides = contours[iContour].ns;
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
			contours[iContour].plane, polyhedronTmp);

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

void Polyhedron::corpol_prepAssociator_add(int iContour, int iFacet, int iEdge,
		int iSideDistMin0, int iSideDistMin1, Vector3d v0_nearest,
		Vector3d v1_nearest, Orientation orientation)
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

	int numSides = contours[iContour].ns;

	int iBegin = iSideDistMin1;
	int iEnd = (numSides + iSideDistMin0 + iStep) % numSides;

	SideOfContour* sides = contours[iContour].sides;

	DEBUG_PRINT("Starting calculating length of chain");
	double lengthChain = 0.;
	for (int iSide = iBegin; iSide != iEnd;
			iSide = (numSides + iSide + iStep) % numSides)
	{

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
		double lengthSide = sqrt(qmod(A2 - A1));
		DEBUG_PRINT("lengthSide = %lf", lengthSide);

		lengthChain += lengthSide;
	}
	DEBUG_PRINT("lengthChain = %lf", lengthChain);

	DEBUG_PRINT("Starting calculating iBeginToBeAdded");
	int iBeginToBeAdded = INT_NOT_INITIALIZED;
	for (int iSide = iBegin;; iSide = (numSides + iSide + iStep) % numSides)
	{
		Vector3d A1 = sides[iSide].A1;
		Vector3d A2 = sides[iSide].A2;
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
		double lengthSide = sqrt(qmod(A2 - A1));
		DEBUG_PRINT("lengthSide = %lf", lengthSide);

		if (lengthSide > ASSOCIATOR_MIN_PORTION_REL * lengthChain)
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
			iSide = (numSides + iSide + iStep) % numSides)
	{
		Vector3d A1 = sides[iSide].A1;
		Vector3d A2 = sides[iSide].A2;
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
		double lengthSide = sqrt(qmod(A2 - A1));
		DEBUG_PRINT("lengthSide = %lf", lengthSide);
		lengthVisited += lengthSide;

		if (lengthVisited > (1 - ASSOCIATOR_MIN_PORTION_REL) * lengthChain)
		{
			iEndToBeAdded = iSide;
			break;
		}
	}
	iEndToBeAdded = (numSides + iEndToBeAdded + iStep) % numSides;
	DEBUG_PRINT("iEndToBeAdded = %d", iEndToBeAdded);

	int iVertex0 = edges[iEdge].v0;
	int iVertex1 = edges[iEdge].v1;
	Vector3d edge = vertices[iVertex1] - vertices[iVertex0];
	double weight = corpol_weightForAssociation(iContour, iEdge);

	DEBUG_PRINT("Starting addition");
	for (int iSide = iBeginToBeAdded; iSide != iEndToBeAdded;
			iSide = (numSides + iSide + iStep) % numSides)
	{
		Vector3d side = sides[iSide].A2 - sides[iSide].A1;
		bool ifDirectionIsProper = edge * side > 0;
		DEBUG_PRINT("Adding contour # %d to the assoc list of edge %d",
				iContour, iEdge);
		DEBUG_PRINT("\tadding side # %d", iSide);
		// Create new association
		EdgeContourAssociation* assocForCurrentEdge =
				new EdgeContourAssociation(iContour, iSide, ifDirectionIsProper,
						weight);
		// Push it to the list
		edges[iEdge].assocList.push_back(*assocForCurrentEdge);
	}
	DEBUG_PRINT("Addition done");
	DEBUG_END;
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

bool Polyhedron::corpol_edgeIsVisibleOnPlane(Edge& edge,
		Plane planeOfProjection)
{
	DEBUG_START;
	int v0 = edge.v0;
	int v1 = edge.v1;
	int f0 = edge.f0;
	int f1 = edge.f1;
	Plane pi0 = facets[f0].plane;
	Plane pi1 = facets[f1].plane;
	double sign0 = pi0.norm * planeOfProjection.norm;
	double sign1 = pi1.norm * planeOfProjection.norm;

	if ((sign0 > EPS_COLLINEARITY && sign1 > EPS_COLLINEARITY)
			|| (sign0 < EPS_COLLINEARITY && sign1 < EPS_COLLINEARITY))
	{
		DEBUG_PRINT(
				"Edge is invisible: it's covered by facets, sign0 = %le, sign1 = %le",
				sign0, sign1);
		return false;
	}

	bool ifOrthogonalTo1stFacet = fabs(sign0) < EPS_COLLINEARITY;
	bool ifOrthogonalTo2ndFacet = fabs(sign1) < EPS_COLLINEARITY;

	if (!ifOrthogonalTo1stFacet && !ifOrthogonalTo2ndFacet)
	{
// Regular case. Nothing is orthogonal.
		DEBUG_PRINT("\t\tRegular case. Nothing is orthogonal.");
		return true;
	}

	else if (ifOrthogonalTo1stFacet && ifOrthogonalTo2ndFacet)
	{
// When the edge is orthogonal to the plane of projection
		DEBUG_PRINT("\t\tThe edge is orthogonal to the plane of projection");
		return false;
	}

	else if (ifOrthogonalTo1stFacet)
	{
// When only the first facets is orthogonal to the plane of projection
		DEBUG_PRINT(
				"\t\tOnly the first facet is orthogonal to the plane of projection");
		return corpol_collinear_visibility(v0, v1, planeOfProjection, f0);
	}

	else // ifOrthogonalTo2ndFacet
	{
// When only the second facets is orthogonal to the plane of projection
		DEBUG_PRINT(
				"\t\tOnly the second facets is orthogonal to the plane of projection");
		return corpol_collinear_visibility(v0, v1, planeOfProjection, f1);
	}
}

bool Polyhedron::corpol_collinear_visibility(int v0processed, int v1processed,
		Plane planeOfProjection, int ifacet)
{
	DEBUG_START;
	int nv = facets[ifacet].numVertices;
	int* index = facets[ifacet].indVertices;
	Vector3d nu = planeOfProjection.norm;

// 1. Find the closest vertices to the plane of projection.
	int ivertexMax = -1;
	double scalarMax = 0.;
	for (int ivertex = 0; ivertex < nv; ++ivertex)
	{
		int v0 = index[ivertex];

		double scalar = nu * vertices[v0];
		if (ivertexMax == -1 || scalarMax < scalar)
		{
			ivertexMax = ivertex;
			scalarMax = scalar;
		}
	}

	int v0Max = index[ivertexMax];
	int v1Max = index[ivertexMax + 1];
	Vector3d vector0Max = planeOfProjection.project(vertices[v0Max]);
	Vector3d vector1Max = planeOfProjection.project(vertices[v1Max]);
	Vector3d mainEdge = vector1Max - vector0Max;

// 1.1. If mainEdge is orthogonal to the plane of projection,
// then we take its another edge which includes vertex iverteMax

	if (qmod(mainEdge % nu) < EPS_COLLINEARITY)
	{
		DEBUG_PRINT(
				"mainEdge (%d, %d) is orthogonal to the plane of projection, ",
				v0Max, v1Max);
		DEBUG_PRINT("so we are taking another edge including ivertexMax (%d)",
				ivertexMax);
		v0Max = index[(ivertexMax - 1 + nv) % nv];
		v1Max = index[ivertexMax];
		vector0Max = planeOfProjection.project(vertices[v0Max]);
		vector1Max = planeOfProjection.project(vertices[v1Max]);
		mainEdge = vector1Max - vector0Max;
	}

// 2. Go around the facets, beginning from the closest vertices.
	for (int ivertex = 0; ivertex < nv; ++ivertex)
	{
		int v0 = index[ivertex];
		int v1 = index[ivertex + 1];
		Vector3d curEdge = vertices[v1] - vertices[v0];

		if ((v0 == v0processed && v1 == v1processed)
				|| (v0 == v1processed && v1 == v0processed))
		{
			// 3. In case of non-positive scalar product
			// -- eliminate the edge from the buffer
			DEBUG_PRINT("main edge = (%d, %d) ; proc edge = (%d, %d)", v0Max,
					v1Max, v0, v1);
			return curEdge * mainEdge > 0;
		}
	}
	DEBUG_PRINT("Error. Edge (%d, %d) cannot be found in facets %d",
			v0processed, v1processed, ifacet);
	DEBUG_END;
	return false;
}

#define NDEBUG

void Polyhedron::preprocess_edges_add(int numEdgesMax, int v0, int v1, int f0,
		int f1)
{
#ifndef NDEBUG
	printf("\n\n----------------\nTrying to add edge (%d, %d) \n", v0, v1);

	for (int i = 0; i < numEdges; ++i)
	printf("\tedges[%d] = (%d, %d)\n", i, edges[i].v0, edges[i].v1);
#endif 
	if (numEdges >= numEdgesMax)
	{
#ifndef NDEBUG
		printf("Warning. List is overflow\n");
#endif
		return;
	}

	if (v0 > v1)
	{
		int tmp = v0;
		v0 = v1;
		v1 = tmp;
	}

	if (f0 > f1)
	{
		int tmp = f0;
		f0 = f1;
		f1 = tmp;
	}

	int retvalfind = preprocess_edges_find(v0, v1);
#ifndef NDEBUG
	printf("retvalfind = %d\n", retvalfind);
#endif
	if (edges[retvalfind].v0 == v0 && edges[retvalfind].v1 == v1)
	{
#ifndef NDEBUG
		printf("Edge (%d, %d) already presents at position %d ! \n", v0, v1, retvalfind);
#endif
		return;
	}

// If not, add current edge to array of edges :
	for (int i = numEdges; i > retvalfind; --i)
	{
		edges[i] = edges[i - 1];
	}
#ifndef NDEBUG
	printf("Adding edge (%d, %d) at position #%d\n", v0, v1, retvalfind);
#endif
	edges[retvalfind].v0 = v0;
	edges[retvalfind].v1 = v1;
	edges[retvalfind].f0 = f0;
	edges[retvalfind].f1 = f1;
	edges[retvalfind].id = numEdges;
	++numEdges;
}

int Polyhedron::preprocess_edges_find(int v0, int v1)
{
#ifndef NDEBUG
	printf("Trying to find edge (%d, %d) \n", v0, v1);
#endif

	if (v0 > v1)
	{
		int tmp = v0;
		v0 = v1;
		v1 = tmp;
	}

// Binary search :
	int first = 0;		// Первый элемент в массиве

	int last = numEdges;		// Последний элемент в массиве

#ifndef NDEBUG
	printf("\tfirst = %d, last = %d \n", first, last);
#endif

	while (first < last)
	{
		int mid = (first + last) / 2;
		int v0_mid = edges[mid].v0;
		int v1_mid = edges[mid].v1;

		if (v0 < v0_mid || (v0 == v0_mid && v1 <= v1_mid))
		{
			last = mid;
		}
		else
		{
			first = mid + 1;
		}
#ifndef NDEBUG
		printf("\tfirst = %d, last = %d, mid = %d \n", first, last, mid);
#endif
	}

#ifndef NDEBUG
	printf("Return value of \"find\" = %d \n", last);
#endif
	return last;
}

#undef NDEBUG
