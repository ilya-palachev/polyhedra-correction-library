#include "PolyhedraCorrectionLibrary.h"

//#define NDEBUG

int Polyhedron::corpol_preprocess(
		int& nume,
		Edge* &edges,
		int N,
		SContour* contours) {
	DBG_START
	;
	// + TODO: provide right orientation of facets
	int numeMax = 0;
	for (int i = 0; i < numf; ++i) {
		numeMax += facet[i].nv;
	}
	numeMax = numeMax / 2;

	edges = new Edge[numeMax];

	preprocess_edges(nume, numeMax, edges);
	corpol_prep_build_lists_of_visible_edges(nume, edges, N, contours);
	corpol_prep_map_between_edges_and_contours(nume, edges, N, contours);

	DBG_END
	;
	return 0;
}

int Polyhedron::preprocess_edges(
		int& nume,
		int numeMax,
		Edge* edges) {
	DBG_START
	;
	nume = 0;
	for (int i = 0; i < numf; ++i) {
		int nv = facet[i].nv;
		int * index = facet[i].index;
		for (int j = 0; j < nv; ++j) {
			preed_add(nume, // Number of already added edges to the list
					numeMax, // Number of edges which we are going add finally
					edges, // Array of edges
					index[j], // First vertex
					index[j + 1], // Second vertex
					i, // Current facet id
					index[nv + 1 + j]); // Id of its neighbour
		}
	}
#ifndef NDEBUG
	for (int i = 0; i < nume; ++i) {
		edges[i].my_fprint(stdout);
	}
#endif

	DBG_END
	;
	return 0;
}

int Polyhedron::corpol_prep_build_lists_of_visible_edges(
		int nume,
		Edge* edges,
		int ncont,
		SContour* contours) {
	DBG_START;

	for (int iedge = 0; iedge < nume; ++iedge) {

		DBGPRINT("Processing edge %d (%d, %d)\n", iedge, edges[iedge].v0,
				edges[iedge].v1);

		for (int icont = 0; icont < ncont; ++icont) {
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
	DBG_START
	;
	int v0 = edge.v0;
	int v1 = edge.v1;
	int f0 = edge.f0;
	int f1 = edge.f1;
	Plane pi0 = facet[f0].plane;
	Plane pi1 = facet[f1].plane;
	double sign0 = pi0.norm * planeOfProjection.norm;
	double sign1 = pi1.norm * planeOfProjection.norm;

	if ((sign0 > EPS_COLLINEARITY && sign1 > EPS_COLLINEARITY)
			|| (sign0 < EPS_COLLINEARITY && sign1 < EPS_COLLINEARITY)) {
		DBGPRINT(
				"Edge is invisible: it's covered by facets, sign0 = %le, sign1 = %le",
				sign0, sign1);
		DBG_END
		;
		return false;
	}

	bool ifOrthogonalTo1stFacet = fabs(sign0) < EPS_COLLINEARITY;
	bool ifOrthogonalTo2ndFacet = fabs(sign1) < EPS_COLLINEARITY;

	if (!ifOrthogonalTo1stFacet && !ifOrthogonalTo2ndFacet) {
		// Regular case. Nothing is orthogonal.
		DBGPRINT("\t\tRegular case. Nothing is orthogonal.");
		DBG_END
		;
		return true;
	}

	else if (ifOrthogonalTo1stFacet && ifOrthogonalTo2ndFacet) {
		// When the edge is orthogonal to the plane of projection
		DBGPRINT("\t\tThe edge is orthogonal to the plane of projection");
		DBG_END
		;
		return false;
	}

	else if (ifOrthogonalTo1stFacet) {
		// When only the first facet is orthogonal to the plane of projection
		DBGPRINT(
				"\t\tOnly the first facet is orthogonal to the plane of projection");
		DBG_END
		;
		return corpol_collinear_visibility(v0, v1, planeOfProjection, f0);
	}

	else // ifOrthogonalTo2ndFacet
	{
		// When only the second facet is orthogonal to the plane of projection
		DBGPRINT(
				"\t\tOnly the second facet is orthogonal to the plane of projection");
		DBG_END
		;
		return corpol_collinear_visibility(v0, v1, planeOfProjection, f1);
	}
}

bool Polyhedron::corpol_collinear_visibility(
		int v0processed,
		int v1processed,
		Plane planeOfProjection,
		int ifacet) {
	DBG_START
	;
	int nv = facet[ifacet].nv;
	int* index = facet[ifacet].index;
	Vector3d nu = planeOfProjection.norm;

	// 1. Find the closest vertex to the plane of projection.
	int ivertexMax = -1;
	double scalarMax = 0.;
	for (int ivertex = 0; ivertex < nv; ++ivertex) {
		int v0 = index[ivertex];

		double scalar = nu * vertex[v0];
		if (ivertexMax == -1 || scalarMax < scalar) {
			ivertexMax = ivertex;
			scalarMax = scalar;
		}
	}

	int v0Max = index[ivertexMax];
	int v1Max = index[ivertexMax + 1];
	Vector3d vector0Max = planeOfProjection.project(vertex[v0Max]);
	Vector3d vector1Max = planeOfProjection.project(vertex[v1Max]);
	Vector3d mainEdge = vector1Max - vector0Max;

	// 1.1. If mainEdge is orthogonal to the plane of projection,
	// then we take its another edge which includes vertex iverteMax

	if (qmod(mainEdge % nu) < EPS_COLLINEARITY) {
		DBGPRINT(
				"mainEdge (%d, %d) is orthogonal to the plane of projection, " "so we are taking another edge including ivertexMax (%d)",
				v0Max, v1Max, ivertexMax);
		v0Max = index[(ivertexMax - 1 + nv) % nv];
		v1Max = index[ivertexMax];
		vector0Max = planeOfProjection.project(vertex[v0Max]);
		vector1Max = planeOfProjection.project(vertex[v1Max]);
		mainEdge = vector1Max - vector0Max;
	}

	// 2. Go around the facet, beginning from the closest vertex.
	for (int ivertex = 0; ivertex < nv; ++ivertex) {
		int v0 = index[ivertex];
		int v1 = index[ivertex + 1];
		Vector3d curEdge = vertex[v1] - vertex[v0];

		if ((v0 == v0processed && v1 == v1processed)
				|| (v0 == v1processed && v1 == v0processed)) {
			// 3. In case of non-positive scalar product
			// -- eliminate the edge from the buffer
			DBGPRINT("main edge = (%d, %d) ; proc edge = (%d, %d)", v0Max, v1Max, v0,
					v1);
			DBG_END
			;
			return curEdge * mainEdge > 0;
		}
	}
	DBGPRINT("Error. Edge (%d, %d) cannot be found in facet %d", v0processed,
			v1processed, ifacet);
	DBG_END
	;
	return false;
}

int Polyhedron::corpol_prep_map_between_edges_and_contours(
		int nume,
		Edge* edges,
		int N,
		SContour* contours) {
	DBG_START
	;
	// NOTE: Testing needed.
	for (int iedge = 0; iedge < nume; ++iedge) {
		int v0 = edges[iedge].v0;
		int v1 = edges[iedge].v1;
		DBGPRINT("Processing edge #%d (%d, %d)", iedge, v0, v1);

		int ncont = edges[iedge].assocList.size();
		Vector3d A0 = vertex[v0];
		Vector3d A1 = vertex[v1];
		for (list<EdgeContourAssociation>::iterator itCont =
				edges[iedge].assocList.begin();
				itCont != edges[iedge].assocList.end();
				++itCont)
		{
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
		int& nume,
		int numeMax,
		Edge* edges,
		int v0,
		int v1,
		int f0,
		int f1) {
#ifndef NDEBUG
	printf("\n\n----------------\nTrying to add edge (%d, %d) \n", v0, v1);

	for (int i = 0; i < nume; ++i)
	printf("\tedges[%d] = (%d, %d)\n", i, edges[i].v0, edges[i].v1);
#endif 
	if (nume >= numeMax) {
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

	int retvalfind = preed_find(nume, edges, v0, v1);
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
	for (int i = nume; i > retvalfind; --i) {
		edges[i] = edges[i - 1];
	}
#ifndef NDEBUG
	printf("Adding edge (%d, %d) at position #%d\n", v0, v1, retvalfind);
#endif
	edges[retvalfind].v0 = v0;
	edges[retvalfind].v1 = v1;
	edges[retvalfind].f0 = f0;
	edges[retvalfind].f1 = f1;
	edges[retvalfind].id = nume;
	++nume;
}

int Polyhedron::preed_find(
		int nume,
		Edge* edges,
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

	int last = nume; // Последний элемент в массиве

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
