#include <math.h>
#include "PolyhedraCorrectionLibrary.h"
#include <ctime>

int Polyhedron::corpolTest(
		int numContours_input,
		int indFacetMoved,
		double maxMoveDelta,
		double shiftAngleFirst) {
	DBG_START;
	numContours = numContours_input;
	contours = new SContour[numContours];

	preprocess_polyhedron();

	int numEdgesMax = 0;
	for (int i = 0; i < numFacets; ++i) {
		numEdgesMax += facets[i].numVertices;
	}
	numEdgesMax = numEdgesMax / 2;

	numEdges = numEdgesMax;
	DBGPRINT("numEdges = %d", numEdges);
	edges = new Edge[numEdgesMax];

	preprocess_edges();

	corpolTest_createAllContours(shiftAngleFirst);

#ifndef NDEBUG
	printf("------------\n Begin of print contours in corpol_test\n");
	for (int i = 0; i < numContours; ++i) {
		contours[i].my_fprint(stdout);
	}
	printf("------------\n End of print contours in corpol_test\n");
#endif

//    corpolTest_slightRandomMove(maxMoveDelta);
//    corpolTest_slightRandomMoveVertex(maxMoveDelta, 0);
	corpolTest_slightRandomMoveFacet(maxMoveDelta, indFacetMoved);

	correct_polyhedron();

	if (edges != NULL) {
		delete[] edges;
		edges = NULL;
	}

	DBG_END;
	return 0;
}

SContour& Polyhedron::corpolTest_createOneContour(
		int idOfContour,
		Plane planeOfProjection,
		bool* bufferBool,
		int* bufferInt0,
		int* bufferInt1) {
	DBG_START;
	bool* ifVisibleEdges = bufferBool;
	int* visibleEdges = bufferInt0;
	int* visibleEdgesSorted = bufferInt1;

	Vector3d nu = planeOfProjection.norm;

	DBGPRINT("Creating contour #%d. Direction = (%lf, %lf, %lf)\n", idOfContour,
			nu.x, nu.y, nu.z);
	DBGPRINT("numEdges = %d", numEdges);

	int numVisibleEdges = 0;
	bool ifVisibleCurrEdge;

	for (int iedge = 0; iedge < numEdges; ++iedge) {
		DBGPRINT("\t(1st processing)Processing edge # %d (%d, %d)\n", iedge,
				edges[iedge].v0, edges[iedge].v1);
		ifVisibleCurrEdge = corpol_edgeIsVisibleOnPlane(edges[iedge],
				planeOfProjection);
		DBGPRINT("\t visibility checked : %s", ifVisibleCurrEdge ?
				"visible" : "invisible");
		numVisibleEdges += ifVisibleCurrEdge;
		ifVisibleEdges[iedge] = ifVisibleCurrEdge;
	}

	int iedgeVisible = 0;
	for (int iedge = 0; iedge < numEdges; ++iedge) {
		if (ifVisibleEdges[iedge]) {
			visibleEdges[iedgeVisible++] = iedge;
			DBGPRINT("visibleEdges[%d] = %d, it is (%d, %d)", iedgeVisible - 1,
					visibleEdges[iedgeVisible - 1],
					edges[visibleEdges[iedgeVisible - 1]].v0,
					edges[visibleEdges[iedgeVisible - 1]].v1);
		}
	}

	int numVisibleEdgesSorted = 0;
	int iedgeCurr = 0;
	visibleEdgesSorted[numVisibleEdgesSorted++] = visibleEdges[iedgeCurr];
	int ivertexCurr = edges[visibleEdges[iedgeCurr]].v1;
	int numIterations = 0;
	while (numVisibleEdgesSorted < numVisibleEdges) {
		DBGPRINT("Searching next edge for edge # %d (%d, %d)", iedgeCurr,
				edges[visibleEdges[iedgeCurr]].v0, edges[visibleEdges[iedgeCurr]].v1);
		DBGPRINT("numVisibleEdgesSorted = %d", numVisibleEdgesSorted);
		DBGPRINT("numVisibleEdges = %d", numVisibleEdges);
		DBGPRINT("ivertexCurr = %d", ivertexCurr);
		for (int iedgeNext = 0; iedgeNext < numVisibleEdges; ++iedgeNext) {
			DBGPRINT("\tCandidate is # %d (%d, %d)", iedgeNext,
					edges[visibleEdges[iedgeNext]].v0, edges[visibleEdges[iedgeNext]].v1);
			if (iedgeNext == iedgeCurr) {
				continue;
			}
			if (edges[visibleEdges[iedgeNext]].v0 == ivertexCurr) {
				iedgeCurr = iedgeNext;
				ivertexCurr = edges[visibleEdges[iedgeCurr]].v1;
				visibleEdgesSorted[numVisibleEdgesSorted++] = visibleEdges[iedgeCurr];
				break;
			} else if (edges[visibleEdges[iedgeNext]].v1 == ivertexCurr) {
				iedgeCurr = iedgeNext;
				ivertexCurr = edges[visibleEdges[iedgeCurr]].v0;
				visibleEdgesSorted[numVisibleEdgesSorted++] = visibleEdges[iedgeCurr];
				break;
			}
		}
		if (numIterations++ > numVisibleEdges * numEdges) {
			DBGPRINT("Error. Infinite loop for search...");
			break;
		}
	}

	for (int iedge = 0; iedge < numVisibleEdgesSorted; ++iedge) {
		DBGPRINT("visibleEdgesSorted[%d] = %d (%d, %d)", iedge,
				visibleEdgesSorted[iedge], edges[visibleEdgesSorted[iedge]].v0,
				edges[visibleEdgesSorted[iedge]].v1);
	}

	DBGPRINT("Allocating \"outputContour\"");
	SContour* outputContour = new SContour;
	outputContour->id = idOfContour; // To make output more understandable
	outputContour->sides = new SideOfContour[numVisibleEdges];
	DBGPRINT("Allocating \"sides\"");
	outputContour->ns = numVisibleEdges;
	outputContour->plane = planeOfProjection;
	outputContour->poly = this;
	SideOfContour* sides = outputContour->sides;

	for (int i = 0; i < numVisibleEdges; ++i) {
		Vector3d A1 = vertices[edges[visibleEdgesSorted[i]].v0];
		Vector3d A2 = vertices[edges[visibleEdgesSorted[i]].v1];

		A1 = planeOfProjection.project(A1);
		A2 = planeOfProjection.project(A2);

		sides[i].A1 = A1;
		sides[i].A2 = A2;
		sides[i].confidence = 1.;
		sides[i].type = EEdgeRegular;
	}
	DBG_END;
	return *outputContour;
}

int Polyhedron::corpolTest_createAllContours(
		double shiftAngleFirst) {
	DBG_START;
	DBGPRINT("Allocating 3 arrays of length %d", numEdges);
	bool* bufferBool = new bool[numEdges];
	int* bufferInt0 = new int[numEdges];
	int* bufferInt1 = new int[numEdges];

	for (int icont = 0; icont < numContours; ++icont) {
		double angle = shiftAngleFirst + 2 * M_PI * (icont) / numContours;
		Vector3d nu = Vector3d(cos(angle), sin(angle), 0);
		Plane planeOfProjection = Plane(nu, 0);

		contours[icont] = corpolTest_createOneContour(icont, planeOfProjection,
				bufferBool, bufferInt0, bufferInt1);
	}

	if (bufferBool != NULL) {
		delete[] bufferBool;
		bufferBool = NULL;
	}

	if (bufferInt0 != NULL) {
		delete[] bufferInt0;
		bufferInt0 = NULL;
	}

	if (bufferInt1 != NULL) {
		delete[] bufferInt1;
		bufferInt1 = NULL;
	}
	DBG_END;
	return 0;
}

static double genRandomDouble(
		double maxDelta) {
	srand((unsigned) time(0));
	int randomInteger = rand();
	double randomDouble = randomInteger;
	const double halfRandMax = RAND_MAX * 0.5;
	randomDouble -= halfRandMax;
	randomDouble /= halfRandMax;

	randomDouble *= maxDelta;

	return randomDouble;
}

void Polyhedron::corpolTest_slightRandomMove(
		double maxDelta) {

	for (int ifacet = 0; ifacet < numFacets; ++ifacet) {
		corpolTest_slightRandomMoveFacet(maxDelta, ifacet);
	}

	for (int ivertex = 0; ivertex < numVertices; ++ivertex) {
		corpolTest_slightRandomMoveVertex(maxDelta, ivertex);
	}
}

inline void Polyhedron::corpolTest_slightRandomMoveFacet(
		double maxMoveDelta,
		int ifacet) {
	Plane& plane = facets[ifacet].plane;
	plane.norm.x += genRandomDouble(maxMoveDelta);
	plane.norm.y += genRandomDouble(maxMoveDelta);
	plane.norm.z += genRandomDouble(maxMoveDelta);
	plane.dist += genRandomDouble(maxMoveDelta);
	double newNorm = sqrt(qmod(plane.norm));
	plane.norm.norm(1.);
	plane.dist /= newNorm;
}

inline void Polyhedron::corpolTest_slightRandomMoveVertex(
		double maxMoveDelta,
		int ivertex) {
	Vector3d& vector = vertices[ivertex];
	vector.x += genRandomDouble(maxMoveDelta);
	vector.y += genRandomDouble(maxMoveDelta);
	vector.z += genRandomDouble(maxMoveDelta);
}
