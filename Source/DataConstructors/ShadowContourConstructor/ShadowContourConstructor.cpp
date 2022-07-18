/*
 * Copyright (c) 2009-2016 Ilya Palachev <iliyapalachev@gmail.com>
 *
 * This file is part of Polyhedra Correction Library.
 *
 * Polyhedra Correction Library is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Polyhedra Correction Library is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Polyhedra Correction Library.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#define _USE_MATH_DEFINES
#undef __STRICT_ANSI__
#include <cmath>

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "Constants.h"
#include "DataConstructors/ShadowContourConstructor/ShadowContourConstructor.h"
#include "DataConstructors/EdgeConstructor/EdgeConstructor.h"
#include "DataContainers/ShadowContourData/ShadowContourData.h"
#include "DataContainers/ShadowContourData/SContour/SContour.h"
#include "Polyhedron/Facet/Facet.h"

ShadowContourConstructor::ShadowContourConstructor(PolyhedronPtr p, ShadowContourDataPtr d) :
	PDataConstructor(p), data(d), bufferBool(NULL), bufferInt0(NULL), bufferInt1(NULL), edgeData(new EdgeData())
{
	DEBUG_START;
	DEBUG_END;
}

ShadowContourConstructor::~ShadowContourConstructor()
{
	DEBUG_START;
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
	DEBUG_END;
}

void ShadowContourConstructor::run(int numContoursNeeded, double firstAngle)
{
	DEBUG_START;
	ASSERT(polyhedron->nonZeroPlanes());

	polyhedron->preprocessAdjacency();
	EdgeConstructor *edgeConstructor = new EdgeConstructor(polyhedron);
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

		createContour(iContour, planeOfProjection, &data->contours[iContour]);
	}

	delete edgeConstructor;

	data->fprint(stdout);

	DEBUG_END;
}

void ShadowContourConstructor::analyzeEdgeVisibility(ShadowContourDataPtr SCData)
{
	DEBUG_START;
	polyhedron->preprocessAdjacency();
	EdgeConstructor *edgeConstructor = new EdgeConstructor(polyhedron);
	edgeConstructor->run(edgeData);

	for (const auto &edge : edgeData->edges)
	{
		int numVisible = 0;
		for (int i = 0; i < SCData->numContours; ++i)
		{
			SContour contour = SCData->contours[i];
			Plane plane = contour.plane;

			if (edgeIsVisibleOnPlane(edge, plane))
				++numVisible;
		}
		std::cout << "Edge [" << edge.v0 << ", " << edge.v1 << "] is visible on " << numVisible << " contours; it is "
				  << "[" << polyhedron->vertices[edge.v0] << ", " << polyhedron->vertices[edge.v1] << "]" << std::endl;
	}
	DEBUG_END;
}

void ShadowContourConstructor::createContour(int idOfContour, Plane planeOfProjection, SContour *outputContour)
{
	DEBUG_START;

	DEBUG_VARIABLE Vector3d nu = planeOfProjection.norm;

	DEBUG_PRINT("Creating contour #%d. Direction = (%lf, %lf, %lf)\n", idOfContour, nu.x, nu.y, nu.z);
	DEBUG_PRINT("numEdges = %d", edgeData->numEdges);

	EdgeSet edgesVisible;
	for (int i = 0; i < polyhedron->numVertices; ++i)
	{
		std::cout << polyhedron->vertices[i] << std::endl;
	}

	int iEdge = 0;
	for (EdgeSetIterator edge = edgeData->edges.begin(); edge != edgeData->edges.end(); ++edge)
	{
		if (edgeIsVisibleOnPlane(*edge, planeOfProjection))
		{
			edgesVisible.insert(*edge);
			DEBUG_PRINT("\t visibility of edge #%d [%d, %d] "
						"checked : visible",
						iEdge, edge->v0, edge->v1);
		}
		else
		{
			DEBUG_PRINT("\t visibility of edge #%d [%d, %d] "
						"checked : invisible",
						iEdge, edge->v0, edge->v1);
		}
		++iEdge;
	}

	outputContour->id = idOfContour;
	outputContour->sides = new SideOfContour[edgesVisible.size()];
	outputContour->ns = edgesVisible.size();
	outputContour->plane = planeOfProjection;
	outputContour->poly = polyhedron;
	SideOfContour *sides = outputContour->sides;

	DEBUG_PRINT("Number of visible edges = %ld", (long unsigned int)edgesVisible.size());

	if (edgesVisible.empty())
	{
		DEBUG_END;
		return;
	}

#ifndef NDEBUG
	for (auto &edge : edgesVisible)
	{
		DEBUG_PRINT("Visible edge: [%ld, %ld]", edge.v0, edge.v1);
	}
#endif

	EdgeSetIterator edgeCurr = edgesVisible.begin();
	int iVertexCurr = edgeCurr->v1;
	for (int iSide = 0; iSide < (int)edgesVisible.size(); ++iSide)
	{
		bool ifEdgeNextFound = false;

		for (EdgeSetIterator edgeNext = edgesVisible.begin(); edgeNext != edgesVisible.end(); ++edgeNext)
		{
			DEBUG_PRINT("edgeCurr = edge #%d (i. e. [%d, %d])", edgeCurr->id, edgeCurr->v0, edgeCurr->v1);
			DEBUG_PRINT("edgeNext = edge #%d (i. e. [%d, %d])", edgeNext->id, edgeNext->v0, edgeNext->v1);
			if ((edgeNext->v0 != iVertexCurr && edgeNext->v1 != iVertexCurr) ||
				(edgeCurr->v0 == edgeNext->v0 && edgeCurr->v1 == edgeNext->v1) ||
				(edgeCurr->v0 == edgeNext->v1 && edgeCurr->v1 == edgeNext->v0))
			{
				DEBUG_PRINT("Continuing...");
				continue;
			}

			Vector3d A1, A2;
			if (edgeNext->v0 == iVertexCurr)
			{
				DEBUG_PRINT("Projection of edge [%d, %d] is "
							"added to contour",
							edgeNext->v0, edgeNext->v1);
				A1 = polyhedron->vertices[edgeNext->v0];
				A2 = polyhedron->vertices[edgeNext->v1];
				iVertexCurr = edgeNext->v1;
			}
			else
			{
				DEBUG_PRINT("Projection of edge [%d, %d] is "
							"added to contour",
							edgeNext->v1, edgeNext->v0);
				A1 = polyhedron->vertices[edgeNext->v1];
				A2 = polyhedron->vertices[edgeNext->v0];
				iVertexCurr = edgeNext->v0;
			}
			A1 = planeOfProjection.project(A1);
			A2 = planeOfProjection.project(A2);

			DEBUG_PRINT("Setting side #%d of contour", iSide);
			sides[iSide].A1 = A1;
			sides[iSide].A2 = A2;
			sides[iSide].confidence = 1.;
			sides[iSide].type = EEdgeRegular;
			edgeCurr = edgeNext;
			ifEdgeNextFound = true;
			break;
		}
		ASSERT_PRINT(ifEdgeNextFound, "Failed to find next edge");
	}

	DEBUG_END;
}

bool ShadowContourConstructor::edgeIsVisibleOnPlane(Edge edge, Plane planeOfProjection)
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
#ifndef NDEBUG
	std::cout << pi0 << std::endl << pi1 << std::endl;
#endif /* NDEBUG */
	ASSERT(!!pi0.norm);
	ASSERT(!!pi1.norm);
	ASSERT(!equal(pi0.norm, Vector3d(0., 0., 0.), EPS_MIN_DOUBLE));
	ASSERT(!equal(pi1.norm, Vector3d(0., 0., 0.), EPS_MIN_DOUBLE));

	if ((sign0 > EPS_COLLINEARITY && sign1 > EPS_COLLINEARITY) ||
		(sign0 < -EPS_COLLINEARITY && sign1 < -EPS_COLLINEARITY))
	{
		DEBUG_PRINT("Edge is invisible: it's covered by facets, "
					"sign0 = %le, sign1 = %le",
					sign0, sign1);
		DEBUG_END;
		return false;
	}

	bool ifOrthogonalTo1stFacet = fabs(sign0) < EPS_COLLINEARITY;
	bool ifOrthogonalTo2ndFacet = fabs(sign1) < EPS_COLLINEARITY;

	if (!ifOrthogonalTo1stFacet && !ifOrthogonalTo2ndFacet)
	{
		// Regular case. Nothing is orthogonal.
		DEBUG_PRINT("\t\tRegular case. Nothing is orthogonal.");
		DEBUG_END;
		return true;
	}

	else if (ifOrthogonalTo1stFacet && ifOrthogonalTo2ndFacet)
	{
		// When the edge is orthogonal to the plane of projection
		DEBUG_PRINT("\t\tThe edge is orthogonal to the plane of projection");
		DEBUG_END;
		return false;
	}

	else if (ifOrthogonalTo1stFacet)
	{
		// When only the first facets is orthogonal to the plane of projection
		DEBUG_PRINT("\t\tOnly the first facet is orthogonal "
					"to the plane of projection");
		DEBUG_END;
		return collinearVisibility(v0, v1, planeOfProjection, f0) || collinearVisibility(v1, v0, planeOfProjection, f0);
	}

	else // ifOrthogonalTo2ndFacet
	{
		// When only the second facets is orthogonal to the plane of projection
		DEBUG_PRINT("\t\tOnly the second facets is orthogonal "
					"to the plane of projection");
		DEBUG_END;
		return collinearVisibility(v0, v1, planeOfProjection, f1) || collinearVisibility(v1, v0, planeOfProjection, f1);
	}
}

bool ShadowContourConstructor::collinearVisibility(int v0processed, int v1processed, Plane planeOfProjection,
												   int ifacet)
{
	DEBUG_START;
	polyhedron->facets[ifacet].my_fprint(stdout);
	int nv = polyhedron->facets[ifacet].numVertices;
	auto &index = polyhedron->facets[ifacet].indVertices;
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
	Vector3d vector0Max = planeOfProjection.project(polyhedron->vertices[v0Max]);
	Vector3d vector1Max = planeOfProjection.project(polyhedron->vertices[v1Max]);
	Vector3d mainEdge = vector1Max - vector0Max;

	// 1.1. If mainEdge is orthogonal to the plane of projection,
	// then we take its another edge which includes vertex iverteMax

	if (qmod(mainEdge % nu) < EPS_COLLINEARITY)
	{
		DEBUG_PRINT("mainEdge (%d, %d) is orthogonal to the plane of projection, ", v0Max, v1Max);
		DEBUG_PRINT("so we are taking another edge including ivertexMax (%d)", ivertexMax);
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

		if ((v0 == v0processed && v1 == v1processed) || (v0 == v1processed && v1 == v0processed))
		{
			// 3. In case of non-positive scalar product
			// -- eliminate the edge from the buffer
			DEBUG_PRINT("main edge = (%d, %d) ; proc edge = (%d, %d)", v0Max, v1Max, v0, v1);
			DEBUG_END;
			return curEdge * mainEdge > 0;
		}
	}
	ERROR_PRINT("Error. Edge (%d, %d) cannot be found in facets %d", v0processed, v1processed, ifacet);
	DEBUG_END;
	return false;
}

Point_2 project(Point_3 point, Vector_3 normal)
{
	CGAL::Origin origin;
	Vector_3 ez(0., 0., 1.);
	Vector_3 tau = CGAL::cross_product(ez, normal);
	Vector_3 vector = point - origin;
	double x = vector * tau;
	double y = vector * ez;
	return Point_2(x, y);
}

Point_3 unproject(Point_2 projection, Vector_3 normal)
{
	CGAL::Origin origin;
	Vector_3 ez(0., 0., 1.);
	Vector_3 tau = CGAL::cross_product(ez, normal);
	return origin + projection.x() * tau + projection.y() * ez;
}

std::pair<std::vector<Point_3>, std::vector<int>> generateProjection(Polyhedron_3 polyhedron, Vector_3 normal)
{
	DEBUG_START;
	std::vector<Point_2> points;
	for (auto vertex = polyhedron.vertices_begin(); vertex != polyhedron.vertices_end(); ++vertex)
	{
		points.push_back(project(vertex->point(), normal));
	}
	std::vector<Point_2> hull;
	convex_hull_2(points.begin(), points.end(), std::back_inserter(hull));
	std::cout << "   Hull contains " << hull.size() << " vetrices." << std::endl;
	std::vector<int> indices(hull.size());
	for (int &i : indices)
		i = INT_NOT_INITIALIZED;
	bool ifConflicts = false;
	for (int i = 0; i < (int)points.size(); ++i)
	{
		int iCurrent = 0;
		Point_2 point = points[i];
		for (auto it = hull.begin(); it != hull.end(); ++it)
		{
			Point_2 pointExtreme = *it;
			if (equal(point.x(), pointExtreme.x()) && equal(point.y(), pointExtreme.y()))
			{
				if (indices[iCurrent] != INT_NOT_INITIALIZED)
				{
					std::cerr << "Conflicting " << indices[iCurrent] << " and " << i << " for hull vertex #" << iCurrent
							  << std::endl;
					ifConflicts = true;
				}
				indices[iCurrent] = i;
				break;
			}
			++iCurrent;
		}
	}
	if (ifConflicts)
	{
		ERROR_PRINT("Conflicts for hull points found!");
		exit(EXIT_FAILURE);
	}

	std::vector<Point_3> projection;
	for (auto point : hull)
	{
		projection.push_back(unproject(point, normal));
	}
	DEBUG_END;
	return std::make_pair(projection, indices);
}
