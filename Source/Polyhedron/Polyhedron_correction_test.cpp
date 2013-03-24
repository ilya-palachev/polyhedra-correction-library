#include <math.h>
#include "PolyhedraCorrectionLibrary.h"
#include <ctime>

int Polyhedron::corpolTest(
		int numContours,
		double maxMoveDelta) {
	DBG_START
	;
	SContour* contours = new SContour[numContours];

	int numEdges;
	Edge* edges;

	makeCube(1.);
	preprocess_polyhedron();

	int numEdgesMax = 0;
	for (int i = 0; i < numFacets; ++i) {
		numEdgesMax += facets[i].numVertices;
	}
	numEdgesMax = numEdgesMax / 2;

	edges = new Edge[numEdgesMax];

	preprocess_edges();

	corpolTest_createAllContours();

#ifndef NDEBUG
	printf("------------\n Begin of print contours in corpol_test\n");
	for (int i = 0; i < numContours; ++i) {
		contours[i].my_fprint(stdout);
	}
	printf("------------\n End of print contours in corpol_test\n");
#endif

//    corpolTest_slightRandomMove(maxMoveDelta);
//    corpolTest_slightRandomMoveVertex(maxMoveDelta, 0);
	corpolTest_slightRandomMoveFacet(maxMoveDelta, 0);

	correct_polyhedron();

	if (edges != NULL) {
		delete[] edges;
		edges = NULL;
	}

	DBG_END
	;
	return 0;
}

SContour& Polyhedron::corpolTest_createOneContour(
		int idOfContour,
		Plane planeOfProjection,
		bool* bufferBool,
		int* bufferInt0,
		int* bufferInt1) {
	DBG_START
	;
	bool* ifVisibleEdges = bufferBool;
	int* visibleEdges = bufferInt0;
	int* visibleEdgesSorted = bufferInt1;

	Vector3d nu = planeOfProjection.norm;

	DBGPRINT("Creating contour #%d. Direction = (%lf, %lf, %lf)\n", idOfContour,
			nu.x, nu.y, nu.z);

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
	DBG_END
	;
	return *outputContour;
}

int Polyhedron::corpolTest_createAllContours() {
	DBG_START
	;
	DBGPRINT("Allocating 3 arrays of length %d", numEdges);
	bool* bufferBool = new bool[numEdges];
	int* bufferInt0 = new int[numEdges];
	int* bufferInt1 = new int[numEdges];

	for (int icont = 0; icont < numContours; ++icont) {
		double angle = 2 * M_PI * (icont) / numContours;
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
	DBG_END
	;
	return 0;
}

void Polyhedron::makeCube(
		double halfSideLength) {
#ifndef NDEBUG
	printf("I'm going to clear polyhedon!!!\n");
#endif //NDEBUG
	if (vertices != NULL) {
		delete[] vertices;
		vertices = NULL;
	}
#ifndef NDEBUG
	printf("vertex deleted !!\n");
#endif //NDEBUG
	if (facets != NULL) {
		delete[] facets;
		facets = NULL;
	}
#ifndef NDEBUG
	printf("facet deleted !!\n");
#endif //NDEBUG
	if (vertexInfos != NULL) {
		delete[] vertexInfos;
		vertexInfos = NULL;
	}
#ifndef NDEBUG
	printf("vertexinfo deleted !!\n");
#endif //NDEBUG
	if (edgeLists != NULL) {
		delete[] edgeLists;
		edgeLists = NULL;
	}
#ifndef NDEBUG
	printf("edge_list deleted !!\n");
#endif //NDEBUG
#ifndef NDEBUG
	printf("Polyhedron has been cleared before recreating as a cube...\n");
#endif //NDEBUG
	numFacets = 6;
	numVertices = 8;

	vertices = new Vector3d[numVertices];
	facets = new Facet[numFacets];

	vertices[0] = Vector3d(-halfSideLength, -halfSideLength, -halfSideLength);
	vertices[1] = Vector3d(halfSideLength, -halfSideLength, -halfSideLength);
	vertices[2] = Vector3d(halfSideLength, halfSideLength, -halfSideLength);
	vertices[3] = Vector3d(-halfSideLength, halfSideLength, -halfSideLength);
	vertices[4] = Vector3d(-halfSideLength, -halfSideLength, halfSideLength);
	vertices[5] = Vector3d(halfSideLength, -halfSideLength, halfSideLength);
	vertices[6] = Vector3d(halfSideLength, halfSideLength, halfSideLength);
	vertices[7] = Vector3d(-halfSideLength, halfSideLength, halfSideLength);

	for (int ifacet = 0; ifacet < numFacets; ++ifacet) {
		int nv = facets[ifacet].numVertices = 4;
		facets[ifacet].indVertices = new int[3 * nv + 1];
		facets[ifacet].id = ifacet;
		facets[ifacet].parentPolyhedron = this;
	}

	facets[0].indVertices[0] = 0;
	facets[0].indVertices[1] = 3;
	facets[0].indVertices[2] = 2;
	facets[0].indVertices[3] = 1;
	facets[0].plane = Plane(Vector3d(0., 0., -1.), -halfSideLength);

	facets[1].indVertices[0] = 4;
	facets[1].indVertices[1] = 5;
	facets[1].indVertices[2] = 6;
	facets[1].indVertices[3] = 7;
	facets[1].plane = Plane(Vector3d(0., 0., 1.), -halfSideLength);

	facets[2].indVertices[0] = 1;
	facets[2].indVertices[1] = 2;
	facets[2].indVertices[2] = 6;
	facets[2].indVertices[3] = 5;
	facets[2].plane = Plane(Vector3d(1., 0., 0.), -halfSideLength);

	facets[3].indVertices[0] = 2;
	facets[3].indVertices[1] = 3;
	facets[3].indVertices[2] = 7;
	facets[3].indVertices[3] = 6;
	facets[3].plane = Plane(Vector3d(0., 1., 0.), -halfSideLength);

	facets[4].indVertices[0] = 0;
	facets[4].indVertices[1] = 4;
	facets[4].indVertices[2] = 7;
	facets[4].indVertices[3] = 3;
	facets[4].plane = Plane(Vector3d(-1., 0., 0.), -halfSideLength);

	facets[5].indVertices[0] = 0;
	facets[5].indVertices[1] = 1;
	facets[5].indVertices[2] = 5;
	facets[5].indVertices[3] = 4;
	facets[5].plane = Plane(Vector3d(0., -1., 0.), -halfSideLength);

	for (int i = 0; i < numFacets; ++i) {
		facets[i].parentPolyhedron = this;
	}

#ifndef NDEBUG
	printf("Polyhedron has been recreated as halfSideLength cube.\n");
#endif //NDEBUG
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
