#include "PolyhedraCorrectionLibrary.h"

//#define NDEBUG

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
		int nv = facets[i].numVertices;
		int * index = facets[i].indVertices;
		for (int j = 0; j < nv; ++j) {
			preed_add(numEdgesMax, // Number of edges which we are going add finally
					index[j], // First vertices
					index[j + 1], // Second vertices
					i, // Current facets id
					index[nv + 1 + j]); // Id of its neighbor
		}
	}
#ifndef NDEBUG
	for (int i = 0; i < numEdges; ++i) {
		edges[i].my_fprint(stdout);
	}
#endif

	DBG_END;
}

void Polyhedron::corpol_prepFindAssociations() {
	DBG_START;
	for (int iContour = 0; iContour < numFacets; ++iContour) {
		corpol_prepFindAssiciations_withContour(iContour);
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
		int iContour, int iFacet) {
	DBG_START;
	int numVertex = facets[iFacet].numVertices;
	int* indVertices = facets[iFacet].indVertices;
	for (int iVertex = 0; iVertex < numVertex; ++iVertex) {
		int iEdge = preed_find(indVertices[iVertex], indVertices[iVertex  + 1]);
		corpol_prepFindAssociations_withContour_forFacetEdge(iContour, iFacet,
				iEdge);
	}
	DBG_END;
}

static double distVertexEdge(
		// this routine calculates distance
		Vector3d A,  				// from this vector
		Vector3d A1, 				// to the edge [A1, A2]
		Vector3d A2) {
	double matrix[] = { A2.y - A1.y, A2.x - A1.x, 0, 0, A2.z - A1.z, A2.y - A1.y,
			A2.x - A1.x, A2.y - A1.y, A2.z - A1.z };
	double rightHandSide[] = { A1.x * (A2.y - A1.y) + A1.y * (A2.x - A1.x), A1.y
			* (A2.z - A1.z) + A1.z * (A2.y - A1.y), A.x * (A2.x - A1.x)
			+ A.y * (A2.y - A1.y) + A.z * (A2.z - A1.z) };
	double matrixFactorized[9];
	int indexPivot[3];
	char formOfEqulibration;
	double rowScaleFactors[3];
	double columnScaleFactors[3];
	double solution[3];
	double reciprocalConditionNumber;
	double forwardErrorBound;
	double backwardErrorRelative;
	double rpivot[9]; // ??? Don't know what it is

	lapack_int info = LAPACKE_dgesvx(LAPACK_ROW_MAJOR, 'E', 'N', 3, 1, matrix, 3,
			matrixFactorized, 3, indexPivot, &formOfEqulibration, rowScaleFactors,
			columnScaleFactors, rightHandSide, 3, solution, 3,
			&reciprocalConditionNumber, &forwardErrorBound, &backwardErrorRelative,
			rpivot);

	if (info != 0) {
		if (info < 0) {
			DBGPRINT("LAPACKE_dgesvx: the %d-th argument "
					"had an illegal value",
					-info);
		} else if (info <= 3) {
			DBGPRINT(
					"LAPACKE_dgesvx: U(%d, %d) is exactly zero.  "
					"The factorization has"
					"been completed, but the factor U is exactly"
					"singular, so the solution and error bounds"
					"could not be computed.",
					info, info);
		} else if (info == 4) {
			DBGPRINT(
					"LAPACKE_dgesvx: U is non-singular, "
					"but RCOND is less than machine"
					"precision, meaning that the matrix is singular"
					"to working precision.  Nevertheless, the"
					"solution and error bounds are computed because"
					"there are a number of situations where the"
					"computed solution can be more accurate than the"
					"value of RCOND would suggest.");
		} else {
			DBGPRINT("LAPACKE_dgesvx: FATAL. Unknown output value");
		}
	}
	Vector3d* Aproj = new Vector3d(rightHandSide[0], rightHandSide[1],
			rightHandSide[2]);
	return sqrt(qmod(A - *Aproj));
}

static double weightForAssociations(double x) {
	return x * x;
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
	double y = norm1 * directionOfProjection;
	if (x + y >= 0)
	{
		if (x - y >= 0)
			return x;
		else
			return -x;
	}
	else
	{
		if (x - y >= 0)
			return y;
		else
			return -y;
	}
}

void Polyhedron::corpol_prepFindAssociations_withContour_forFacetEdge(
		int iContour,
		int iFacet,
		int iEdge) {
	DBG_START;
	if (corpol_visibilityForAssociation(iContour, iEdge) <
			-EPSILON_EDGE_CONTOUR_VISIBILITY)
		return;
	int iVertex1 = edges[iEdge].v0;
	int iVertex2 = edges[iEdge].v1;
	SideOfContour* sides = contours[iContour].sides;
	int numSides = contours[iContour].ns;
	int iSideDistMin;
	double distMin;
	for (int iSide = 0; iSide < numSides; ++iSide) {
		Vector3d v0 = sides[iSide].A1;
		Vector3d v1 = sides[iSide].A2;
		double distCurr1 = distVertexEdge(vertices[iVertex1], v0, v1);
		double distCurr2 = distVertexEdge(vertices[iVertex2], v0, v1);
		double distCurr = distCurr1 + distCurr2;
		if (iSide == 0 || distMin > distCurr) {
			iSideDistMin = iSide;
		}
	}
	Vector3d side = sides[iSideDistMin].A2 - sides[iSideDistMin].A1;
	Vector3d edge = vertices[iVertex2] - vertices[iVertex1];
	bool ifDirectionIsProper = edge * side > 0;

	double weight = corpol_weightForAssociation(iContour, iFacet);

	EdgeContourAssociation* assocForCurrentEdge = new EdgeContourAssociation(
			iContour, iSideDistMin, ifDirectionIsProper, weight);
	edges[iEdge].assocList.push_back(*assocForCurrentEdge);
	DBG_END;
}

int Polyhedron::corpol_prep_build_lists_of_visible_edges() {
	DBG_START	;

	for (int iedge = 0; iedge < numEdges; ++iedge) {

		DBGPRINT("Processing edge %d (%d, %d)\n", iedge, edges[iedge].v0,
				edges[iedge].v1);

		for (int icont = 0; icont < numContours; ++icont) {
			Plane planeOfProjection = contours[icont].plane;
			bool ifVisibleOnCurrCont = corpol_edgeIsVisibleOnPlane(edges[iedge],
					planeOfProjection);
			if (ifVisibleOnCurrCont) {
				edges[iedge].assocList.push_back(EdgeContourAssociation(icont));
			}
		}

		edges[iedge].my_fprint(stdout);
	}

	DBG_END;
	return 0;
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
				"mainEdge (%d, %d) is orthogonal to the plane of projection, "
"so we are taking another edge including ivertexMax (%d)",
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

int Polyhedron::corpol_prep_map_between_edges_and_contours() {
	DBG_START;
// NOTE: Testing needed.
	for (int iedge = 0; iedge < numEdges; ++iedge) {
		int v0 = edges[iedge].v0;
		int v1 = edges[iedge].v1;
		DBGPRINT("Processing edge #%d (%d, %d)", iedge, v0, v1);

		int ncont = edges[iedge].assocList.size();
		Vector3d A0 = vertices[v0];
		Vector3d A1 = vertices[v1];
		for (list<EdgeContourAssociation>::iterator itCont =
				edges[iedge].assocList.begin(); itCont != edges[iedge].assocList.end();
				++itCont) {
			DBGPRINT("\tFind nearest on contour #%d", itCont->indContour);
			contours[itCont->indContour].my_fprint(stdout);

			int ns = contours[itCont->indContour].ns;
			SideOfContour * sides = contours[itCont->indContour].sides;
			int isideMin = -1;
			double minDist = 0.;
			bool direction = true;
			for (int iside = 0; iside < ns; ++iside) {
				Vector3d B0 = sides[iside].A1;
				Vector3d B1 = sides[iside].A2;
				double dist0 = qmod(A0 - B0) + qmod(A1 - B1);
				double dist1 = qmod(A0 - B1) + qmod(A1 - B0);
				if (dist0 < minDist || isideMin == -1) {
					minDist = dist0;
					isideMin = iside;
					direction = true;
				}
				if (dist1 < minDist) {
					minDist = dist1;
					isideMin = iside;
					direction = false;
				}
			}
			itCont->indNearestSide = isideMin;
			itCont->ifProperDirection = direction;

			DBGPRINT("\tThe nearest is side #%d = ", isideMin);
			sides[isideMin].my_fprint_short(stdout);
		}

		edges[iedge].my_fprint(stdout);
	}

	DBG_END;
	return 0;
}

#define NDEBUG

void Polyhedron::preed_add(
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

	int retvalfind = preed_find(v0, v1);
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

int Polyhedron::preed_find(
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
