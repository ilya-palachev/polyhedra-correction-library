/*
 * ShadeContourConstructor.cpp
 *
 *  Created on: 17.05.2013
 *      Author: iliya
 */

#include "PolyhedraCorrectionLibrary.h"

ShadeContourConstructor::ShadeContourConstructor(Polyhedron* p,
		ShadeContourData* d) :
				PDataConstructor(p),
				data(d),
				bufferBool(NULL),
				bufferInt0(NULL),
				bufferInt1(NULL),
				edgeData(NULL)
{
}

ShadeContourConstructor::~ShadeContourConstructor()
{
	if (bufferBool != NULL)
	{
		delete[] bufferBool;
		bufferBool = NULL;
	}
	if (bufferInt0 != NULL)
	{
		delete[] bufferInt0;
		bufferInt0 = NULL;
	}
	if (bufferInt1 != NULL)
	{
		delete[] bufferInt1;
		bufferInt1 = NULL;
	}
}

void ShadeContourConstructor::run(int numContoursNeeded, double firstAngle)
{
	DEBUG_START;
	EdgeConstructor* edgeConstructor = new EdgeConstructor(polyhedron,
			edgeData);
	edgeConstructor->run(edgeData);

	DEBUG_PRINT("Allocating 3 arrays of length %d", edgeData->numEdges);
	bool* bufferBool = new bool[edgeData->numEdges];
	int* bufferInt0 = new int[edgeData->numEdges];
	int* bufferInt1 = new int[edgeData->numEdges];

	for (int iContour = 0; iContour < data->numContours; ++iContour)
	{
		double angle = firstAngle + 2 * M_PI * (iContour) / data->numContours;
		Vector3d nu = Vector3d(cos(angle), sin(angle), 0);
		Plane planeOfProjection = Plane(nu, 0);

		data->contours[iContour] = createContour(iContour, planeOfProjection);
	}
	DEBUG_END;
}

SContour& ShadeContourConstructor::createContour(int idOfContour,
		Plane planeOfProjection)
{
	DEBUG_START;
	bool* ifVisibleEdges = bufferBool;
	int* visibleEdges = bufferInt0;
	int* visibleEdgesSorted = bufferInt1;

	Vector3d nu = planeOfProjection.norm;

	DEBUG_PRINT("Creating contour #%d. Direction = (%lf, %lf, %lf)\n",
			idOfContour, nu.x, nu.y, nu.z);
	DEBUG_PRINT("numEdges = %d", edgeData->numEdges);

	int numVisibleEdges = 0;
	bool ifVisibleCurrEdge;

	for (int iedge = 0; iedge < edgeData->numEdges; ++iedge)
	{
		DEBUG_PRINT("\t(1st processing)Processing edge # %d (%d, %d)\n", iedge,
				edgeData->edges[iedge].v0, edgeData->edges[iedge].v1);
		ifVisibleCurrEdge = edgeIsVisibleOnPlane(edgeData->edges[iedge],
				planeOfProjection);
		DEBUG_PRINT("\t visibility checked : %s",
				ifVisibleCurrEdge ? "visible" : "invisible");
		numVisibleEdges += ifVisibleCurrEdge;
		ifVisibleEdges[iedge] = ifVisibleCurrEdge;
	}

	int iedgeVisible = 0;
	for (int iedge = 0; iedge < edgeData->numEdges; ++iedge)
	{
		if (ifVisibleEdges[iedge])
		{
			visibleEdges[iedgeVisible++] = iedge;
			DEBUG_PRINT("visibleEdges[%d] = %d, it is (%d, %d)",
					iedgeVisible - 1, visibleEdges[iedgeVisible - 1],
					edgeData->edges[visibleEdges[iedgeVisible - 1]].v0,
					edgeData->edges[visibleEdges[iedgeVisible - 1]].v1);
		}
	}

	int numVisibleEdgesSorted = 0;
	int iedgeCurr = 0;
	visibleEdgesSorted[numVisibleEdgesSorted++] = visibleEdges[iedgeCurr];
	int ivertexCurr = edgeData->edges[visibleEdges[iedgeCurr]].v1;
	int numIterations = 0;

	/*
	 * We need to have sequences of sides like the following:
	 *    a0   a1
	 *    a1   a2
	 *    a2   a3
	 *    a3   a0
	 * But if we don't ensure that a1 is always followed by a1,
	 * and a2 with a2, and so on, there can arise the following
	 * non-correct contours:
	 *    a0   a1
	 *    a1   a2
	 *    a3   a2
	 *    a0   a3
	 * To avoid such situation we must remember whether to reverse
	 * the order of vertices of the edge, during its detection.
	 */
	bool* ifEdgeShouldBeReversed = bufferBool;

	// First edge is always not reversed:
	ifEdgeShouldBeReversed[visibleEdgesSorted[0]] = false;

	while (numVisibleEdgesSorted < numVisibleEdges)
	{
		DEBUG_PRINT("Searching next edge for edge # %d (%d, %d)", iedgeCurr,
				edgeData->edges[visibleEdges[iedgeCurr]].v0,
				edgeData->edges[visibleEdges[iedgeCurr]].v1);
		DEBUG_PRINT("numVisibleEdgesSorted = %d", numVisibleEdgesSorted);
		DEBUG_PRINT("numVisibleEdges = %d", numVisibleEdges);
		DEBUG_PRINT("ivertexCurr = %d", ivertexCurr);
		for (int iedgeNext = 0; iedgeNext < numVisibleEdges; ++iedgeNext)
		{
			DEBUG_PRINT("\tCandidate is # %d (%d, %d)", iedgeNext,
					edgeData->edges[visibleEdges[iedgeNext]].v0,
					edgeData->edges[visibleEdges[iedgeNext]].v1);
			if (iedgeNext == iedgeCurr)
			{
				continue;
			}
			if (edgeData->edges[visibleEdges[iedgeNext]].v0 == ivertexCurr)
			{
				iedgeCurr = iedgeNext;
				ivertexCurr = edgeData->edges[visibleEdges[iedgeCurr]].v1;
				visibleEdgesSorted[numVisibleEdgesSorted] =
						visibleEdges[iedgeCurr];
				ifEdgeShouldBeReversed[numVisibleEdgesSorted] = false;
				++numVisibleEdgesSorted;
				break;
			}
			else if (edgeData->edges[visibleEdges[iedgeNext]].v1 == ivertexCurr)
			{
				iedgeCurr = iedgeNext;
				ivertexCurr = edgeData->edges[visibleEdges[iedgeCurr]].v0;
				visibleEdgesSorted[numVisibleEdgesSorted] =
						visibleEdges[iedgeCurr];
				ifEdgeShouldBeReversed[numVisibleEdgesSorted] = true;
				++numVisibleEdgesSorted;
				break;
			}
		}
		if (numIterations++ > numVisibleEdges * edgeData->numEdges)
		{
			DEBUG_PRINT("Error. Infinite loop for search...");
			break;
		}
	}

	for (int iedge = 0; iedge < numVisibleEdgesSorted; ++iedge)
	{
		DEBUG_PRINT("visibleEdgesSorted[%d] = %d (%d, %d)", iedge,
				visibleEdgesSorted[iedge],
				edgeData->edges[visibleEdgesSorted[iedge]].v0,
				edgeData->edges[visibleEdgesSorted[iedge]].v1);
	}

	DEBUG_PRINT("Allocating \"outputContour\"");
	SContour* outputContour = new SContour;
	outputContour->id = idOfContour; // To make output more understandable
	outputContour->sides = new SideOfContour[numVisibleEdges];
	DEBUG_PRINT("Allocating \"sides\"");
	outputContour->ns = numVisibleEdges;
	outputContour->plane = planeOfProjection;
	outputContour->poly = polyhedron;
	SideOfContour* sides = outputContour->sides;

	for (int i = 0; i < numVisibleEdges; ++i)
	{
		Vector3d A1, A2;
		if (ifEdgeShouldBeReversed[i])
		{
			A1 =
					polyhedron->vertices[edgeData->edges[visibleEdgesSorted[i]]
							.v1];
			A2 =
					polyhedron->vertices[edgeData->edges[visibleEdgesSorted[i]]
							.v0];
		}
		else
		{
			A1 =
					polyhedron->vertices[edgeData->edges[visibleEdgesSorted[i]]
							.v0];
			A2 =
					polyhedron->vertices[edgeData->edges[visibleEdgesSorted[i]]
							.v1];
		}

		A1 = planeOfProjection.project(A1);
		A2 = planeOfProjection.project(A2);

		sides[i].A1 = A1;
		sides[i].A2 = A2;
		sides[i].confidence = 1.;
		sides[i].type = EEdgeRegular;
	}
	DEBUG_END;
	return *outputContour;
}

bool ShadeContourConstructor::edgeIsVisibleOnPlane(Edge& edge,
		Plane planeOfProjection)
{
	DEBUG_START;
	int v0 = edge.v0;
	int v1 = edge.v1;
	int f0 = edge.f0;
	int f1 = edge.f1;
	Plane pi0 = polyhedron->facets[f0].plane;
	Plane pi1 = polyhedron->facets[f1].plane;
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
		return collinearVisibility(v0, v1, planeOfProjection, f0);
	}

	else // ifOrthogonalTo2ndFacet
	{
// When only the second facets is orthogonal to the plane of projection
		DEBUG_PRINT(
				"\t\tOnly the second facets is orthogonal to the plane of projection");
		return collinearVisibility(v0, v1, planeOfProjection, f1);
	}
}

bool ShadeContourConstructor::collinearVisibility(int v0processed,
		int v1processed, Plane planeOfProjection, int ifacet)
{
	DEBUG_START;
	int nv = polyhedron->facets[ifacet].numVertices;
	int* index = polyhedron->facets[ifacet].indVertices;
	Vector3d nu = planeOfProjection.norm;

// 1. Find the closest vertices to the plane of projection.
	int ivertexMax = -1;
	double scalarMax = 0.;
	for (int ivertex = 0; ivertex < nv; ++ivertex)
	{
		int v0 = index[ivertex];

		double scalar = nu * polyhedron->vertices[v0];
		if (ivertexMax == -1 || scalarMax < scalar)
		{
			ivertexMax = ivertex;
			scalarMax = scalar;
		}
	}

	int v0Max = index[ivertexMax];
	int v1Max = index[ivertexMax + 1];
	Vector3d vector0Max = planeOfProjection.project(
			polyhedron->vertices[v0Max]);
	Vector3d vector1Max = planeOfProjection.project(
			polyhedron->vertices[v1Max]);
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
		vector0Max = planeOfProjection.project(polyhedron->vertices[v0Max]);
		vector1Max = planeOfProjection.project(polyhedron->vertices[v1Max]);
		mainEdge = vector1Max - vector0Max;
	}

// 2. Go around the facets, beginning from the closest vertices.
	for (int ivertex = 0; ivertex < nv; ++ivertex)
	{
		int v0 = index[ivertex];
		int v1 = index[ivertex + 1];
		Vector3d curEdge = polyhedron->vertices[v1] - polyhedron->vertices[v0];

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

