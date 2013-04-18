#include "PolyhedraCorrectionLibrary.h"

//#define NDEBUG

static double distVertexEdge(
		Vector3d A,
		Vector3d A1,
		Vector3d A2);

static double weightForAssociations(
		double x) {
	return x * x;
}

void Polyhedron::corpol_preprocess() {
	DBG_START;
	preprocess_edges();
	corpol_prepFindAssociations();
	DBG_END;
}

void Polyhedron::preprocess_edges() {
	DBG_START;
	int numEdgesMax = numEdges = 0;
	for (int iFacet = 0; iFacet < numFacets; ++iFacet) {
		numEdgesMax += facets[iFacet].numVertices;
	}
	numEdgesMax /= 2;
	DBGPRINT("numEdgesMax = %d", numEdgesMax);
	for (int i = 0; i < numFacets; ++i) {
		int numVerticesInFacet = facets[i].numVertices;
		int * index = facets[i].indVertices;
		for (int iVertex = 0; iVertex < numVerticesInFacet; ++iVertex) {
			preprocess_edges_add(numEdgesMax, // Number of edges which we are going add finally
					index[iVertex], // First vertices
					index[iVertex + 1], // Second vertices
					i, // Current facets id
					index[numVerticesInFacet + 1 + iVertex]); // Id of its neighbor
		}
	}

	for (int iEdge = 0; iEdge < numEdges; ++iEdge) {
		edges[iEdge].id = iEdge;
	}

	for (int i = 0; i < numEdges; ++i) {
		edges[i].my_fprint(stdout);
	}
	DBG_END;
}

void Polyhedron::corpol_prepFindAssociations() {
	DBG_START;
	for (int iContour = 0; iContour < numContours; ++iContour) {
		corpol_prepFindAssiciations_withContour(iContour);
	}

	/* Print found associations : */
	for (int iEdge = 0; iEdge < numEdges; ++iEdge) {
		edges[iEdge].my_fprint(stdout);
	}
	DBG_END;
}

void Polyhedron::corpol_prepFindAssiciations_withContour(
		int iContour) {
	DBG_START;
	for (int iFacet = 0; iFacet < numFacets; ++iFacet) {
		corpol_prepFindAssociations_withContour_forFacet(iContour, iFacet);
	}
	DBG_END;
}

void Polyhedron::corpol_prepFindAssociations_withContour_forFacet(
		int iContour,
		int iFacet) {
	DBG_START;
	int numVertex = facets[iFacet].numVertices;
	int* indVertices = facets[iFacet].indVertices;
	for (int iVertex = 0; iVertex < numVertex; ++iVertex) {
		int iEdge = preprocess_edges_find(indVertices[iVertex],
				indVertices[iVertex + 1]);
		corpol_prepAssociator(iContour, iFacet, iEdge);
	}
	DBG_END;
}

double Polyhedron::corpol_weightForAssociation(
		int iFacet,
		int iContour) {
	Vector3d normalToFacet = facets[iFacet].plane.norm;
	Vector3d directionOfProjection = contours[iContour].plane.norm;
	normalToFacet.norm(1.);
	directionOfProjection.norm(1.);
	return weightForAssociations(normalToFacet * directionOfProjection);
}

double Polyhedron::corpol_visibilityForAssociation(
		int iContour,
		int iEdge) {
	int f0 = edges[iEdge].f0;
	int f1 = edges[iEdge].f1;
	Vector3d norm0 = facets[f0].plane.norm;
	norm0.norm(1.);
	Vector3d norm1 = facets[f1].plane.norm;
	norm1.norm(1.);
	Vector3d directionOfProjection = contours[iContour].plane.norm;
	directionOfProjection.norm(1.);
	double x = norm0 * directionOfProjection;
	DBGPRINT("x = %lf", x);
	double y = norm1 * directionOfProjection;
	DBGPRINT("y = %lf", y);
	double sum = y + x;
	double sub = y - x;
	if (sum >= 0) {
		if (sub >= 0) {
			return -x;
		} else {
			return -y;
		}
	} else {
		if (sub >= 0) {
			return y;
		} else {
			return x;
		}
	}
}

void Polyhedron::corpol_prepAssociator(
		int iContour,
		int iFacet,
		int iEdge) {
	DBG_START;
	DBGPRINT("processing contour # %d, facet # %d, edge # %d", iContour, iFacet,
			iEdge);

	bool checkVisibility = corpol_prepAssociator_checkVisibility(iContour, iFacet,
			iEdge) == EXIT_SUCCESS;
	if (!checkVisibility)
		return;

	bool checkAlreadyAdded = corpol_prepAssociator_checkAlreadyAdded(iContour,
			iFacet, iEdge) == EXIT_SUCCESS;
	if (!checkAlreadyAdded)
		return;

	Vector3d v0_projected, v1_projected;
	corpol_prepAssociator_project(iContour, iFacet, iEdge, v0_projected,
			v1_projected);

	bool checkExtinction = corpol_prepAssociator_checkExtinction(iContour, iFacet,
			iEdge, v0_projected, v1_projected) == EXIT_SUCCESS;
	if (!checkExtinction)
		return;

	int iSideDistMin = corpol_prepAssociator_findNearest(iContour, iFacet, iEdge,
			v0_projected, v1_projected);
	corpol_prepAssociator_add(iContour, iFacet, iEdge, iSideDistMin);

	DBG_END;
}

int Polyhedron::corpol_prepAssociator_checkVisibility(
		int iContour,
		int iFacet,
		int iEdge) {
	double visibilityNumber = corpol_visibilityForAssociation(iContour, iEdge);
	if (visibilityNumber < EPSILON_EDGE_CONTOUR_VISIBILITY) {
		DBGPRINT("Edge is invisible on this contour (visibility %lf)!",
				visibilityNumber);
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int Polyhedron::corpol_prepAssociator_checkAlreadyAdded(
		int iContour,
		int iFacet,
		int iEdge) {
	// Check whether this association has been already added to the list
	int numAlreadyPushedAssocs = edges[iEdge].assocList.size();
	list<EdgeContourAssociation>::iterator lastCont =
			edges[iEdge].assocList.end();
	--lastCont;
	int iContourLastInList = lastCont->indContour;
	DBGPRINT("iContourLastInList = %d, numAlreadyPushedAssocs = %d",
			iContourLastInList, numAlreadyPushedAssocs);
	if (numAlreadyPushedAssocs != 0 && iContourLastInList == iContour) {
		DBGPRINT("Edge %d already has association with contour # %d", iEdge,
				iContour);
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int Polyhedron::corpol_prepAssociator_checkExtinction(
		int iContour,
		int iFacet,
		int iEdge,
		Vector3d v0_projected,
		Vector3d v1_projected) {
	int iVertex0 = edges[iEdge].v0;
	int iVertex1 = edges[iEdge].v1;
	if (qmod(v0_projected - v1_projected) < EPS_SAME_POINTS) {
		DBGPRINT(
				"Edge # %d (%d, %d) is reduced into point when projecting" "on the plane of projection of contour # %d",
				iEdge, iVertex0, iVertex1, iContour);
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

void Polyhedron::corpol_prepAssociator_project(
		int iContour,
		int iFacet,
		int iEdge,
		Vector3d& v0_projected,
		Vector3d& v1_projected) {
	int iVertex0 = edges[iEdge].v0;
	int iVertex1 = edges[iEdge].v1;
	Vector3d v0 = vertices[iVertex0];
	Vector3d v1 = vertices[iVertex1];
	Plane planeOfProjection = contours[iContour].plane;
	v0_projected = planeOfProjection.project(v0);
	v1_projected = planeOfProjection.project(v1);
}

int Polyhedron::corpol_prepAssociator_findNearest(
		int iContour,
		int iFacet,
		int iEdge,
		Vector3d v0_projected,
		Vector3d v1_projected) {
	int iVertex0 = edges[iEdge].v0;
	int iVertex1 = edges[iEdge].v1;
	SideOfContour* sides = contours[iContour].sides;
	int numSides = contours[iContour].ns;
	int iSideDistMin;
	double distMin;
	DBGPRINT("We are processing the following contour:");
	//contours[iContour].my_fprint(stdout);

	for (int iSide = 0; iSide < numSides; ++iSide) {
		DBGPRINT(
				"processing contour # %d, facet # %d, edge # %d (%d, %d), " "side of contour # %d",
				iContour, iFacet, iEdge, iVertex0, iVertex1, iSide);
		Vector3d v0 = sides[iSide].A1;
		Vector3d v1 = sides[iSide].A2;
		double distCurr1 = distVertexEdge(v0_projected, v0, v1);
		double distCurr2 = distVertexEdge(v1_projected, v0, v1);
		double distCurr = distCurr1 + distCurr2;
		DBGPRINT("distCurr = %lf", distCurr);
		DBGPRINT("---");
		if (iSide == 0 || distMin > distCurr) {
			iSideDistMin = iSide;
			distMin = distCurr;
		}
	}
	DBGPRINT("distMin = %lf", distMin);
	ASSERT(distMin < 0.1);
	return iSideDistMin;
}

void Polyhedron::corpol_prepAssociator_add(
		int iContour,
		int iFacet,
		int iEdge,
		int iSideDistMin) {
	int iVertex0 = edges[iEdge].v0;
	int iVertex1 = edges[iEdge].v1;
	SideOfContour* sides = contours[iContour].sides;
	Vector3d side = sides[iSideDistMin].A2 - sides[iSideDistMin].A1;
	Vector3d edge = vertices[iVertex1] - vertices[iVertex0];
	bool ifDirectionIsProper = edge * side > 0;
	double weight = corpol_weightForAssociation(iContour, iFacet);
	DBGPRINT("Adding contour # %d to the assoc list of edge %d", iContour, iEdge);
	// Create new association
	EdgeContourAssociation* assocForCurrentEdge = new EdgeContourAssociation(
			iContour, iSideDistMin, ifDirectionIsProper, weight);
	// Push it to the list
	edges[iEdge].assocList.push_back(*assocForCurrentEdge);
}


static double distVertexEdge(
		// this routine calculates distance
		Vector3d A,  				// from this vector
		Vector3d A1, 				// to the edge [A1, A2]
		Vector3d A2) {

	double dist;
	Vector3d edge = A2 - A1;
	if ((A1 - A) * edge > 0 && (A2 - A) * edge > 0) {
		dist = sqrt(qmod(A1 - A));
	} else if ((A1 - A) * edge < 0 && (A2 - A) * edge < 0) {
		dist = sqrt(qmod(A2 - A));
	} else {

		Vector3d step = edge * 0.5;
		Vector3d Aproj = A1 + step;
		do {
			step *= 0.5;
			double scalarProd = (Aproj - A) * edge;
			if (scalarProd < EPS_COLLINEARITY && scalarProd > -EPS_COLLINEARITY) {
				break;
			} else if (scalarProd > 0) {
				Aproj -= step;
			} else if (scalarProd < 0) {
				Aproj += step;
			}
		} while (1);

		dist = sqrt(qmod(A - Aproj));
	}
	DBGPRINT("dist from (%lf, %lf, %lf) to the edge", A.x, A.y, A.z);
	DBGPRINT("   (%lf, %lf, %lf) - (%lf, %lf, %lf)", A1.x, A1.y, A1.z, A2.x, A2.y,
			A2.z);
	DBGPRINT("   is equal %lf", dist);
	return dist;
}

bool Polyhedron::corpol_edgeIsVisibleOnPlane(
		Edge& edge,
		Plane planeOfProjection) {
	DBG_START;
	int v0 = edge.v0;
	int v1 = edge.v1;
	int f0 = edge.f0;
	int f1 = edge.f1;
	Plane pi0 = facets[f0].plane;
	Plane pi1 = facets[f1].plane;
	double sign0 = pi0.norm * planeOfProjection.norm;
	double sign1 = pi1.norm * planeOfProjection.norm;

	if ((sign0 > EPS_COLLINEARITY && sign1 > EPS_COLLINEARITY)
			|| (sign0 < EPS_COLLINEARITY && sign1 < EPS_COLLINEARITY)) {
		DBGPRINT(
				"Edge is invisible: it's covered by facets, sign0 = %le, sign1 = %le",
				sign0, sign1);
		return false;
	}

	bool ifOrthogonalTo1stFacet = fabs(sign0) < EPS_COLLINEARITY;
	bool ifOrthogonalTo2ndFacet = fabs(sign1) < EPS_COLLINEARITY;

	if (!ifOrthogonalTo1stFacet && !ifOrthogonalTo2ndFacet) {
// Regular case. Nothing is orthogonal.
		DBGPRINT("\t\tRegular case. Nothing is orthogonal.");
		return true;
	}

	else if (ifOrthogonalTo1stFacet && ifOrthogonalTo2ndFacet) {
// When the edge is orthogonal to the plane of projection
		DBGPRINT("\t\tThe edge is orthogonal to the plane of projection");
		return false;
	}

	else if (ifOrthogonalTo1stFacet) {
// When only the first facets is orthogonal to the plane of projection
		DBGPRINT(
				"\t\tOnly the first facet is orthogonal to the plane of projection");
		return corpol_collinear_visibility(v0, v1, planeOfProjection, f0);
	}

	else // ifOrthogonalTo2ndFacet
	{
// When only the second facets is orthogonal to the plane of projection
		DBGPRINT(
				"\t\tOnly the second facets is orthogonal to the plane of projection");
		return corpol_collinear_visibility(v0, v1, planeOfProjection, f1);
	}
}

bool Polyhedron::corpol_collinear_visibility(
		int v0processed,
		int v1processed,
		Plane planeOfProjection,
		int ifacet) {
	DBG_START;
	int nv = facets[ifacet].numVertices;
	int* index = facets[ifacet].indVertices;
	Vector3d nu = planeOfProjection.norm;

// 1. Find the closest vertices to the plane of projection.
	int ivertexMax = -1;
	double scalarMax = 0.;
	for (int ivertex = 0; ivertex < nv; ++ivertex) {
		int v0 = index[ivertex];

		double scalar = nu * vertices[v0];
		if (ivertexMax == -1 || scalarMax < scalar) {
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

	if (qmod(mainEdge % nu) < EPS_COLLINEARITY) {
		DBGPRINT(
				"mainEdge (%d, %d) is orthogonal to the plane of projection, " "so we are taking another edge including ivertexMax (%d)",
				v0Max, v1Max, ivertexMax);
		v0Max = index[(ivertexMax - 1 + nv) % nv];
		v1Max = index[ivertexMax];
		vector0Max = planeOfProjection.project(vertices[v0Max]);
		vector1Max = planeOfProjection.project(vertices[v1Max]);
		mainEdge = vector1Max - vector0Max;
	}

// 2. Go around the facets, beginning from the closest vertices.
	for (int ivertex = 0; ivertex < nv; ++ivertex) {
		int v0 = index[ivertex];
		int v1 = index[ivertex + 1];
		Vector3d curEdge = vertices[v1] - vertices[v0];

		if ((v0 == v0processed && v1 == v1processed)
				|| (v0 == v1processed && v1 == v0processed)) {
			// 3. In case of non-positive scalar product
			// -- eliminate the edge from the buffer
			DBGPRINT("main edge = (%d, %d) ; proc edge = (%d, %d)", v0Max, v1Max, v0,
					v1);
			return curEdge * mainEdge > 0;
		}
	}
	DBGPRINT("Error. Edge (%d, %d) cannot be found in facets %d", v0processed,
			v1processed, ifacet);
	DBG_END;
	return false;
}


#define NDEBUG

void Polyhedron::preprocess_edges_add(
		int numEdgesMax,
		int v0,
		int v1,
		int f0,
		int f1) {
#ifndef NDEBUG
	printf("\n\n----------------\nTrying to add edge (%d, %d) \n", v0, v1);

	for (int i = 0; i < numEdges; ++i)
	printf("\tedges[%d] = (%d, %d)\n", i, edges[i].v0, edges[i].v1);
#endif 
	if (numEdges >= numEdgesMax) {
#ifndef NDEBUG
		printf("Warning. List is overflow\n");
#endif
		return;
	}

	if (v0 > v1) {
		int tmp = v0;
		v0 = v1;
		v1 = tmp;
	}

	if (f0 > f1) {
		int tmp = f0;
		f0 = f1;
		f1 = tmp;
	}

	int retvalfind = preprocess_edges_find(v0, v1);
#ifndef NDEBUG
	printf("retvalfind = %d\n", retvalfind);
#endif
	if (edges[retvalfind].v0 == v0 && edges[retvalfind].v1 == v1) {
#ifndef NDEBUG
		printf("Edge (%d, %d) already presents at position %d ! \n", v0, v1, retvalfind);
#endif
		return;
	}

// If not, add current edge to array of edges :
	for (int i = numEdges; i > retvalfind; --i) {
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

int Polyhedron::preprocess_edges_find(
		int v0,
		int v1) {
#ifndef NDEBUG
	printf("Trying to find edge (%d, %d) \n", v0, v1);
#endif

	if (v0 > v1) {
		int tmp = v0;
		v0 = v1;
		v1 = tmp;
	}

// Binary search :
	int first = 0; // Первый элемент в массиве

	int last = numEdges; // Последний элемент в массиве

#ifndef NDEBUG
	printf("\tfirst = %d, last = %d \n", first, last);
#endif

	while (first < last) {
		int mid = (first + last) / 2;
		int v0_mid = edges[mid].v0;
		int v1_mid = edges[mid].v1;

		if (v0 < v0_mid || (v0 == v0_mid && v1 <= v1_mid)) {
			last = mid;
		} else {
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
