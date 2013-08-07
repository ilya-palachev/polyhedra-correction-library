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
	polyhedron->preprocessAdjacency();
	EdgeConstructor* edgeConstructor = new EdgeConstructor(polyhedron,
			edgeData);
	edgeConstructor->run(edgeData);

	DEBUG_PRINT("Allocating 3 arrays of length %d", edgeData->numEdges);
	bufferBool = new bool[edgeData->numEdges];
	bufferInt0 = new int[edgeData->numEdges];
	bufferInt1 = new int[edgeData->numEdges];

	data->numContours = numContoursNeeded;
	if (data->contours)
	{
		delete[] data->contours;
		data->contours = NULL;
	}
	data->contours = new SContour[data->numContours];

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
	int* visibleEdgesSorted = bufferInt1;

	Vector3d nu = planeOfProjection.norm;

	DEBUG_PRINT("Creating contour #%d. Direction = (%lf, %lf, %lf)\n",
			idOfContour, nu.x, nu.y, nu.z);
	DEBUG_PRINT("numEdges = %d", edgeData->numEdges);

	EdgeSet edgesVisible;

	int iEdge = 0;
	for (EdgeSetIterator edge = edgeData->edges.begin();
			edge != edgeData->edges.end(); ++edge)
	{
		if (edgeIsVisibleOnPlane(*edge, planeOfProjection))
		{
			edgesVisible.insert(*edge);
			DEBUG_PRINT("\t visibility of edge #%d [%d, %d] "
					"checked : visible", iEdge, edge->v0, edge->v1);
		}
		else
		{
			DEBUG_PRINT("\t visibility of edge #%d [%d, %d] "
					"checked : invisible", iEdge, edge->v0, edge->v1);
		}
		++edge;
		++iEdge;
	}

	SContour* outputContour = new SContour;
	outputContour->id = idOfContour;
	outputContour->sides = new SideOfContour[edgesVisible.size()];
	outputContour->ns = edgesVisible.size();
	outputContour->plane = planeOfProjection;
	outputContour->poly = polyhedron;
	SideOfContour* sides = outputContour->sides;

	EdgeSetIterator edgeCurr = edgesVisible.begin();
	int iVertexCurr = edgeCurr->v1;
	for (int iSide = 0; iSide < edgesVisible.size();)
	{
		for (EdgeSetIterator edgeNext = edgesVisible.begin();
				edgeNext != edgesVisible.end(); ++edgeNext)
		{
			if ((edgeNext->v0 != iVertexCurr &&
							edgeNext->v1 != iVertexCurr) ||
					(edgeCurr->v0 == edgeNext->v0 &&
							edgeCurr->v1 == edgeNext->v1) ||
					(edgeCurr->v0 == edgeNext->v1 &&
							edgeCurr->v1 == edgeNext->v0))
			{
				continue;
			}

			Vector3d A1, A2;
			if (edgeNext->v0 == iVertexCurr)
			{
				A1 = polyhedron->vertices[edgeNext->v0];
				A2 = polyhedron->vertices[edgeNext->v1];
				iVertexCurr = edgeNext->v1;
			}
			else
			{
				A1 = polyhedron->vertices[edgeNext->v1];
				A2 = polyhedron->vertices[edgeNext->v0];
				iVertexCurr = edgeNext->v0;
			}
			A1 = planeOfProjection.project(A1);
			A2 = planeOfProjection.project(A2);

			sides[iSide].A1 = A1;
			sides[iSide].A2 = A2;
			sides[iSide].confidence = 1.;
			sides[iSide].type = EEdgeRegular;
			edgeCurr = edgeNext;
			break;
		}
	}

	DEBUG_END;
	return *outputContour;
}

bool ShadeContourConstructor::edgeIsVisibleOnPlane(Edge edge,
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
				"Edge is invisible: it's covered by facets, "
				"sign0 = %le, sign1 = %le",
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
				"\t\tOnly the first facet is orthogonal "
				"to the plane of projection");
		return collinearVisibility(v0, v1, planeOfProjection, f0);
	}

	else // ifOrthogonalTo2ndFacet
	{
// When only the second facets is orthogonal to the plane of projection
		DEBUG_PRINT(
				"\t\tOnly the second facets is orthogonal "
				"to the plane of projection");
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

