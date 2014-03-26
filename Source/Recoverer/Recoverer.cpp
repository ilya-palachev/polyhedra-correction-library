/*
 * Copyright (c) 2009-2014 Ilya Palachev <iliyapalachev@gmail.com>
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

#include <cmath>
#include <set>
#include <boost/concept_check.hpp>

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "Constants.h"
#include "DataContainers/ShadeContourData/ShadeContourData.h"
#include "DataContainers/ShadeContourData/SContour/SContour.h"
#include "Polyhedron/Facet/Facet.h"
#include "Polyhedron/VertexInfo/VertexInfo.h"
#include "Analyzers/SizeCalculator/SizeCalculator.h"
#include "Recoverer/Recoverer.h"

using namespace std;

#define DEFAULT_MAX_COORDINATE 1000000.

Recoverer::Recoverer() :
	ifBalancing(false), ifScaleMatrix(false)
{
	DEBUG_START;
	DEBUG_END;
}

Recoverer::~Recoverer()
{
	DEBUG_START;
	DEBUG_END;
}

void Recoverer::enableBalancing(void)
{
	DEBUG_START;
	ifBalancing = true;
	DEBUG_END;
}

void Recoverer::enableMatrixScaling(void)
{
	DEBUG_START;
	ifScaleMatrix = true;
	DEBUG_END;
}

PolyhedronPtr Recoverer::buildNaivePolyhedron(ShadeContourDataPtr SCData)
{
	DEBUG_START;

	/* 1. Balance contours if it is required. */
	if (ifBalancing)
	{
		balanceAllContours(SCData);
	}

	/* 2. Extract support planes from shadow contour data. */
	vector<Plane> supportPlanes = extractSupportPlanes(SCData);
	DEBUG_PRINT("Number of extracted support planes: %ld",
		(long unsigned int) supportPlanes.size());

	/* 3. Map planes to dual space to obtain the set of points in it. */
	vector<Vector3d> supportPoints = mapPlanesToDualSpace(supportPlanes);

	/* 4. Construct convex hull in the dual space. */
	PolyhedronPtr polyhedronDual = constructConvexHull(supportPoints);

	/* 5. Map dual polyhedron to the primal space. */
	PolyhedronPtr polyhedron = buildDualPolyhedron(polyhedronDual);

	DEBUG_END;
	return polyhedronDual;
}

PolyhedronPtr Recoverer::buildDualNonConvexPolyhedron(ShadeContourDataPtr
		SCData)
{
	DEBUG_START;

	/* 1. Balance contours if it is required. */
	if (ifBalancing)
	{
		balanceAllContours(SCData);
	}

	/* 2. Extract support planes from shadow contour data. */
	vector<Plane> supportPlanes = extractSupportPlanes(SCData);
	DEBUG_PRINT("Number of extracted support planes: %ld",
		(long unsigned int) supportPlanes.size());

	/* 3. Map planes to dual space to obtain the set of points in it. */
	vector<Vector3d> supportPoints = mapPlanesToDualSpace(supportPlanes);

	/* 4. Normalize all points so that to put the to the sphere. */
	vector<Vector3d> supportPointsNormalized;
	supportPointsNormalized.insert(supportPointsNormalized.begin(),
			supportPoints.begin(), supportPoints.end());
	int iVertex = 0;
	for (auto &v : supportPointsNormalized)
	{
		ASSERT(v == supportPoints[iVertex++]);
		v.norm(1.);
	}

	/* 5. Construct convex hull in the dual space. */
	PolyhedronPtr polyhedronDual = constructConvexHull(supportPointsNormalized);

	ASSERT((unsigned long int) polyhedronDual->numVertices ==
			supportPoints.size());

	/* 6. Restore saved coordinates of vectors. */
	iVertex = 0;
	for (auto &v : supportPoints)
	{
		polyhedronDual->vertices[iVertex++] = v;
	}

	/* 7. Print resulting polyhedron to the file. */
	polyhedronDual->fprint_ply_autoscale(DEFAULT_MAX_COORDINATE,
		"../poly-data-out/poly-dual-nonconvex-debug.ply", "dual-polyhedron");

	DEBUG_END;
	return polyhedronDual;
}

PolyhedronPtr Recoverer::buildMaybeDualContours(bool ifDual,
		ShadeContourDataPtr SCData)
{
	DEBUG_START;

	/* New polyhedron will have 1 facet for each shadow contour. */
	int numFacets = SCData->numContours;

	/* Count the total number of vertices required for polyhedron. */
	int numVertices = 0;
	for (int iContour = 0; iContour < SCData->numContours; ++iContour)
	{
		numVertices += SCData->contours[iContour].ns;
	}
	PolyhedronPtr p(new Polyhedron(numVertices, numFacets));

	/* Add a facet for each shadow contour. */
	int iVertex = 0;
	for (int iContour = 0; iContour < SCData->numContours; ++iContour)
	{
		SContour* contourCurr = &SCData->contours[iContour];
		Facet* facetNew = new Facet(iContour, contourCurr->ns,
				contourCurr->plane, NULL);
		p->facets[iContour] = *facetNew;

		/* We hope that compiler will apply loop unswitching here. */
		if (ifDual)
		{
			vector<Plane>  planes = extractSupportPlanes(contourCurr);
			vector<Vector3d> points = mapPlanesToDualSpace(planes);
			int iSide = 0;
			for (auto &point : points)
			{
				p->vertices[iVertex] = point;
				p->facets[iContour].indVertices[iSide++] = iVertex;
				++iVertex;
			}
		}
		else
		{
			for (int iSide = 0; iSide < contourCurr->ns; ++iSide)
			{
				/* Nota Bene: here we read only 1st vertex of current side. */
				p->vertices[iVertex] = contourCurr->sides[iSide].A1;
				p->facets[iContour].indVertices[iSide] = iVertex;
				++iVertex;
			}
		}

		/* Cycling vertex. */
		p->facets[iContour].indVertices[p->facets[iContour].numVertices] =
				p->facets[iContour].indVertices[0];
	}
	p->set_parent_polyhedron_in_facets();

	DEBUG_END;
	return p;
}

PolyhedronPtr Recoverer::buildDualContours(ShadeContourDataPtr SCData)
{
	DEBUG_START;

	/* Balance contours if it is required. */
	if (ifBalancing)
	{
		balanceAllContours(SCData);
	}

	PolyhedronPtr p = buildMaybeDualContours(true, SCData);

	/* Print resulting polyhedron to the file. */
	p->fprint_ply_autoscale(DEFAULT_MAX_COORDINATE,
		"../poly-data-out/poly-dual-contours.ply", "contours");

	DEBUG_END;
	return p;
}

PolyhedronPtr Recoverer::buildContours(ShadeContourDataPtr SCData)
{
	DEBUG_START;

	/* Balance contours if it is required. */
	if (ifBalancing)
	{
		balanceAllContours(SCData);
	}

	PolyhedronPtr p = buildMaybeDualContours(false, SCData);

	/* Print resulting polyhedron to the file. */
	p->fprint_ply_autoscale(DEFAULT_MAX_COORDINATE,
			"../poly-data-out/poly-contours.ply", "contours");

	DEBUG_END;
	return p;
}

vector<Plane> Recoverer::extractSupportPlanes(SContour* contour)
{
	DEBUG_START;
	vector<Plane> supportPlanes;
	Vector3d normal = contour->plane.norm;

	/* Iterate through the array of sides of current contour. */
	for (int iSide = 0; iSide < contour->ns; ++iSide)
	{
		SideOfContour* sideCurr = &contour->sides[iSide];

		/*
		 * Here the plane that is incident to points A1 and A2 of the
		 * current side and collinear to the vector of projection.
		 *
		 * TODO: Here should be the calculation of the best plane fitting
		 * these conditions. Current implementation can produce big errors.
		 */
		Vector3d supportPlaneNormal = (sideCurr->A1 - sideCurr->A2) %
			normal;
		Plane supportPlane(supportPlaneNormal,
						   - supportPlaneNormal * sideCurr->A1);

		supportPlanes.push_back(supportPlane);

		DEBUG_VARIABLE double error1 = supportPlane.norm * sideCurr->A1 +
			supportPlane.dist;
		DEBUG_VARIABLE double error2 = supportPlane.norm * sideCurr->A2 +
			supportPlane.dist;

		DEBUG_PRINT("   side #%d\t%le\t%le", iSide, error1, error2);

		/* TODO: Here should be more strict conditions. */
		ASSERT(fabs(error1) < 100 * EPS_MIN_DOUBLE);
		ASSERT(fabs(error2) < 100 * EPS_MIN_DOUBLE);
	}
	DEBUG_END;
	return supportPlanes;
}

vector<Plane> Recoverer::extractSupportPlanes(ShadeContourDataPtr SCData)
{
	DEBUG_START;
	vector<Plane> supportPlanes;

	/* Iterate through the array of contours. */
	for (int iContour = 0; iContour < SCData->numContours; ++iContour)
	{
		DEBUG_PRINT("contour #%d", iContour);
		SContour* contourCurr = &SCData->contours[iContour];

		/*
		 * Extract support planes from one contour and insert it to common list.
		 */
		vector<Plane> supportPlanesPortion = extractSupportPlanes(contourCurr);
		for (auto &plane : supportPlanesPortion)
		{
			supportPlanes.push_back(plane);
		}
	}

	DEBUG_END;
	return supportPlanes;
}

vector<Vector3d> Recoverer::mapPlanesToDualSpace(vector<Plane> planes)
{
	DEBUG_START;
	vector<Vector3d> vectors;
	
	for (auto &plane : planes)
	{
		/*
		 * The transformation of polar duality works as follows:
		 *
		 * (ax + by + cz + d = 0) |--> (-a/d, -b/d, -c/d)
		 *
		 * and reverse.
		 */
		Vector3d vector = plane.norm;

		/* 3 multiplications is more efficient than 3 divisions: */
		vector *= -1. / plane.dist;
		vectors.push_back(vector);

		DEBUG_PRINT("map (%lf) x + (%lf) y + (%lf) z + (%lf) = 0  |--> "
			"(%lf, %lf, %lf)", plane.norm.x, plane.norm.y, plane.norm.z,
			plane.dist, vector.x, vector.y, vector.z);
	}
	
	return vectors;
	DEBUG_END;
}

/**
 * A functor computing the plane containing a triangular facet
 */
struct Plane_from_facet
{
	/*
	 * Calculate the plane of facet as the plane that contains 3 sequential
	 * points of the facet.
	 *
	 * TODO: It can produce big errors.
	 * Here should be more accurate calculation of the best fitting plane
	 * for the given set of points.
	 */
	Polyhedron_3::Plane_3 operator()(Polyhedron_3::Facet& f)
	{
		Polyhedron_3::Halfedge_handle h = f.halfedge();
		return Polyhedron_3::Plane_3(h->vertex()->point(),
				h->next()->vertex()->point(), h->opposite()->vertex()->point());
	}
};

PolyhedronPtr Recoverer::constructConvexHull (vector<Vector3d> points)
{
	DEBUG_START;
	Polyhedron_3 poly;

	/* Convert Vector3d objects to Point_3 objects. */
	std::vector<Point_3> pointsCGAL;
	for (auto &point : points)
	{
		Point_3 pointCGAL(point.x, point.y, point.z);
		pointsCGAL.push_back(pointCGAL);

		DEBUG_PRINT("Convert Vector3d(%lf, %lf, %lf) to Point_3(%lf, %lf, %lf)",
					point.x, point.y, point.z,
					pointCGAL.x(), pointCGAL.y(), pointCGAL.z());

		ASSERT(fabs(point.x - pointCGAL.x()) < EPS_MIN_DOUBLE);
		ASSERT(fabs(point.y - pointCGAL.y()) < EPS_MIN_DOUBLE);
		ASSERT(fabs(point.z - pointCGAL.z()) < EPS_MIN_DOUBLE);
	}
	DEBUG_PRINT("There are %ld PCL points and %ld CGAL points",
		(long unsigned int) points.size(),
		(long unsigned int) pointsCGAL.size());
	auto point = points.begin();
	auto pointCGAL = pointsCGAL.begin();
	do
	{
		ASSERT(fabs(point->x - pointCGAL->x()) < EPS_MIN_DOUBLE);
		ASSERT(fabs(point->y - pointCGAL->y()) < EPS_MIN_DOUBLE);
		ASSERT(fabs(point->z - pointCGAL->z()) < EPS_MIN_DOUBLE);
	} while(++point != points.end() && ++pointCGAL != pointsCGAL.end());

	/* Use the algorithm of standard STATIC convex hull. */
	CGAL::convex_hull_3(pointsCGAL.begin(), pointsCGAL.end(), poly);

	/* Calculate equations of planes for each facet. */
	std::transform(poly.facets_begin(), poly.facets_end(), poly.planes_begin(),
			Plane_from_facet());

	DEBUG_PRINT("Convex hull of %ld points contains %ld extreme points.",
		(long unsigned int) points.size(),
		(long unsigned int) poly.size_of_vertices());
	
	DEBUG_PRINT("Output polyhedron contains %ld vertices, %ld halfedges and "
		"%ld facets",
		(long unsigned int) poly.size_of_vertices(),
		(long unsigned int) poly.size_of_halfedges(),
		(long unsigned int) poly.size_of_facets());

	/* Convert CGAL polyhedron to PCL polyhedron: */
	PolyhedronPtr polyhedronDualPCL(new Polyhedron(poly));

	polyhedronDualPCL->set_parent_polyhedron_in_facets();
	polyhedronDualPCL->my_fprint(stdout);
	
	/*
	 * TODO: We should not use absolute paths or .. here !!!
	 * Default output directory should be determined by environmental variable
	 * or by a config file.
	 */
	polyhedronDualPCL->fprint_ply_autoscale(DEFAULT_MAX_COORDINATE,
		"../poly-data-out/poly-dual-debug.ply", "dual-polyhedron");
	
	DEBUG_END;
	return polyhedronDualPCL;
}

PolyhedronPtr Recoverer::buildDualPolyhedron(PolyhedronPtr p)
{
	DEBUG_START;

	/* Preprocess the initial polyhedron. */
	p->preprocessAdjacency();

	/* Allocate dual polyhedron. */
	PolyhedronPtr pDual(new Polyhedron());

	pDual->numVertices = p->numFacets;
	pDual->numFacets = p->numVertices;

	pDual->vertices = new Vector3d[pDual->numVertices];
	pDual->facets = new Facet[pDual->numFacets];

	/* Prepare and map the vector of planes to vector of points. */
	vector<Plane> planes;
	for (int iFacet = 0; iFacet < p->numFacets; ++iFacet)
	{
		planes.push_back(p->facets[iFacet].plane);
	}

	vector<Vector3d> vertices = mapPlanesToDualSpace(planes);

	int iVertex = 0;
	for (auto &vertex : vertices)
	{
		pDual->vertices[iVertex++] = vertex;
	}

	/*
	 * Create dual facets using the information computed during
	 * pre-processing.
	 */
	for (int iVertex = 0; iVertex < p->numVertices; ++iVertex)
	{
		Facet *facetCurr = &pDual->facets[iVertex];
		facetCurr->numVertices = p->vertexInfos[iVertex].numFacets;
		facetCurr->indVertices = new int[3 * facetCurr->numVertices + 1];
		for (int iFacet = 0; iFacet < facetCurr->numVertices; ++iFacet)
		{
			facetCurr->indVertices[iFacet] =
				p->vertexInfos[iVertex].indFacets[iFacet];
		}
	}

	/* Preprocess dual polyhedron. */
	pDual->set_parent_polyhedron_in_facets();
	pDual->preprocessAdjacency();
	pDual->my_fprint(stdout);
	pDual->fprint_ply_autoscale(DEFAULT_MAX_COORDINATE,
			"../poly-data-out/poly-primal-debug.ply", "primal-polyhedron");

	DEBUG_END;
	return pDual;
}

void Recoverer::shiftAllContours(ShadeContourDataPtr SCData, Vector3d shift)
{
	DEBUG_START;
	for (int iContour = 0; iContour < SCData->numContours; ++iContour)
	{
		SContour* contourCurr = &SCData->contours[iContour];
		for (int iSide = 0; iSide < contourCurr->ns; ++iSide)
		{
			SideOfContour* sideCurr = &contourCurr->sides[iSide];
			sideCurr->A1 += shift;
			sideCurr->A2 += shift;
		}
		/*
		 * If we want new plane to include point (x + xt, y + yt, z + zt) for
		 * each point (x, y, z) that is included in plane
		 * a * x + b * y + c * z + d = 0
		 * then its free coefficient must become equal to
		 * d - a * xt - b * yt - c * zt
		 *
		 * TODO: For our case usually we shift points in vertical planes on a
		 * vertical vector, thus, the plane will not actually move.
		 */
		contourCurr->plane.dist -= contourCurr->plane.norm * shift;
	}
	DEBUG_END;
}

void Recoverer::balanceAllContours(ShadeContourDataPtr SCData)
{
	DEBUG_START;
	/* Construct polyhedron consisting of contours as facets. */
	PolyhedronPtr p = buildMaybeDualContours(false, SCData);

	/* Calculate the mass center of contours. */
	SizeCalculator* sizeCalculator = new SizeCalculator(p);
	Vector3d center = sizeCalculator->calculateSurfaceCenter();

	/* Shift all contours on z component of the vector of mass center. */
	Vector3d ez(0., 0., 1.);
	shiftAllContours(SCData, - (ez * center) * ez);
	DEBUG_END;
}

#define NUM_NONZERO_EXPECTED 4

static void analyzeTaucsMatrix(taucs_ccs_matrix* Q, bool ifAnalyzeExpect)
{
	DEBUG_START;

	int* numElemRow = (int*) calloc (Q->m, sizeof(int));

	for (int iCol = 0; iCol < Q->n + 1; ++iCol)
	{
		DEBUG_PRINT("Q->colptr[%d] = %d", iCol, Q->colptr[iCol]);
		if (iCol < Q->n)
		{
			for (int iRow = Q->colptr[iCol]; iRow < Q->colptr[iCol + 1]; ++iRow)
			{
				DEBUG_PRINT("Q[%d][%d] = %.16lf", Q->rowind[iRow], iCol,
						Q->values.d[iRow]);
				numElemRow[Q->rowind[iRow]]++;
			}
		}
	}
	DEBUG_PRINT("Q->colptr[%d] - Q->colptr[%d] = %d", Q->n, 0,
			Q->colptr[Q->n] - Q->colptr[0]);

	if (ifAnalyzeExpect)
	{
		int numUnexcpectedNonzeros = 0;
		for (int iRow = 0; iRow < Q->m; ++iRow)
		{
			DEBUG_PRINT("%d-th row of Q has %d elements.", iRow,
					numElemRow[iRow]);
			if (numElemRow[iRow] != NUM_NONZERO_EXPECTED)
			{
				DEBUG_PRINT("Warning: unexpected number of nonzero elements in "
						"row");
				++numUnexcpectedNonzeros;
			}
		}
		DEBUG_PRINT("Number of rows with unexpected number of nonzero elements "
				"is %d", numUnexcpectedNonzeros);
	}
	free(numElemRow);
	DEBUG_END;
}

typedef struct
{
	int u0;
	int u1;
	int u2;
	int u3;
} Quadruple;

typedef struct
{
	int iVertex;
	double det;
} IrowAndValue;

/**
 * Builds matrix of constraints from the polyhedron (which represents a convex
 * hull of the set of directions for which the support values are given).
 *
 * @param polyhedron	Convex hull of the set of directions
 * @param numConditions	The number of constraints (output)
 * @param matrix		The matrix of constraints (output)
 */
static taucs_ccs_matrix* buildMatrixByPolyhedron(PolyhedronPtr polyhedron,
		int& numConditions, bool ifScaleMatrix)
{
	DEBUG_START;

	/* Check that all facets of the polyhedron are triangles. */
	for (int iFacet = 0; iFacet < polyhedron->numFacets; ++iFacet)
	{
		if (polyhedron->facets[iFacet].numVertices != 3)
		{
			ERROR_PRINT("Facet #%d is not triangle. ", iFacet);
			DEBUG_END;
			exit(EXIT_FAILURE);
		}
	}

	/* Preprocess the polyhedron. */
	polyhedron->preprocessAdjacency();

	int numHvalues = polyhedron->numVertices;

	auto comparer = [](Quadruple e0, Quadruple e1)
	{
		int e0min, e0max, e1min, e1max;
		if (e0.u0 < e0.u1)
		{
			e0min = e0.u0;
			e0max = e0.u1;
		}
		else
		{
			e0min = e0.u1;
			e0max = e0.u0;
		}
		
		if (e1.u0 < e1.u1)
		{
			e1min = e1.u0;
			e1max = e1.u1;
		}
		else
		{
			e1min = e1.u1;
			e1max = e1.u0;
		}
		
		return (e0min < e1min) || (e0min == e1min && e0max < e1max);
	};
	set<Quadruple, decltype(comparer)> edgesReported(comparer),
			edgesProved(comparer), edgesCurrent(comparer);

	for (int iVertex = 0; iVertex < polyhedron->numVertices; ++iVertex)
	{
		VertexInfo* vinfoCurr = &polyhedron->vertexInfos[iVertex];
		edgesCurrent.clear();

		for (int iPosition = 0; iPosition < vinfoCurr->numFacets; ++iPosition)
		{
			int iFacetNeighbor =
					vinfoCurr->indFacets[vinfoCurr->numFacets + 1 +
					                     iPosition];
			int iPositionNeighbor =
					vinfoCurr->indFacets[2 * vinfoCurr->numFacets + 1 +
					                     iPosition];
			Facet* facetNeighbor = &polyhedron->facets[iFacetNeighbor];
			ASSERT(iPositionNeighbor < facetNeighbor->numVertices);
			int iVertexNeighbor =
					facetNeighbor->indVertices[iPositionNeighbor + 1];

			int iPositionPrev = (vinfoCurr->numFacets + iPosition - 1) %
					vinfoCurr->numFacets;
			int iFacetNeighborPrev =
					vinfoCurr->indFacets[vinfoCurr->numFacets + 1 +
					                     iPositionPrev];
			int iPositionNeighborPrev =
					vinfoCurr->indFacets[2 * vinfoCurr->numFacets + 1 +
					                     iPositionPrev];
			Facet* facetNeighborPrev = &polyhedron->facets[iFacetNeighborPrev];
			ASSERT(iPositionNeighborPrev < facetNeighborPrev->numVertices);
			int iVertexNeighborPrev =
					facetNeighborPrev->indVertices[iPositionNeighborPrev + 1];
					
			int iPositionNext = (vinfoCurr->numFacets + iPosition + 1) %
					vinfoCurr->numFacets;
			int iFacetNeighborNext =
					vinfoCurr->indFacets[vinfoCurr->numFacets + 1 +
					                     iPositionNext];
			int iPositionNeighborNext =
					vinfoCurr->indFacets[2 * vinfoCurr->numFacets + 1 +
					                     iPositionNext];
			Facet* facetNeighborNext = &polyhedron->facets[iFacetNeighborNext];
			ASSERT(iPositionNeighborNext < facetNeighborNext->numVertices);
			int iVertexNeighborNext =
					facetNeighborNext->indVertices[iPositionNeighborNext + 1];
			
			Quadruple edgeCurrent = {iVertex, iVertexNeighbor,
					iVertexNeighborPrev, iVertexNeighborNext};
			edgesCurrent.insert(edgeCurrent);
		}

		DEBUG_PRINT("Processing vertex #%d, it's degree = %d. Incident edges "
				"are:", iVertex, vinfoCurr->numFacets);
		for (auto &edge DEBUG_VARIABLE: edgesCurrent)
		{
			DEBUG_PRINT("   edge [%d, %d, <%d>, <%d>]", edge.u0, edge.u1,
					edge.u2, edge.u3);
		}

		if (vinfoCurr->numFacets == 3)
		{
			bool ifProved = false;
			for (auto &edgeCurrent : edgesCurrent)
			{
				if (edgesProved.find(edgeCurrent) != edgesProved.end())
				{
					DEBUG_PRINT("Convexity of edge [%d, %d, <%d>, <%d>] already"
							" proved, so all convexity of all edges is proved.",
							edgeCurrent.u0, edgeCurrent.u1, edgeCurrent.u2,
							edgeCurrent.u3);
					ifProved = true;
					break;
				}
			}

			if (!ifProved)
			{
				auto edgeCurrent = edgesCurrent.begin();
				DEBUG_PRINT("Reporting edge [%d, %d, <%d>, <%d>]",
							edgeCurrent->u0, edgeCurrent->u1, edgeCurrent->u2,
							edgeCurrent->u3);
				edgesReported.insert(*edgeCurrent);
			}

			edgesProved.insert(edgesCurrent.begin(),
					edgesCurrent.end());
		}
		else
		{
			for (auto &edgeCurrent : edgesCurrent)
			{
				if (edgesProved.find(edgeCurrent) == edgesProved.end())
				{
					DEBUG_PRINT("Reporting edge [%d, %d, <%d>, <%d>]",
								edgeCurrent.u0, edgeCurrent.u1, edgeCurrent.u2,
								edgeCurrent.u3);
					edgesProved.insert(edgeCurrent);
					edgesReported.insert(edgeCurrent);
				}
			}
		}
	}

	numConditions = edgesReported.size();

	/*
	 * Create TAUCS sparse matrix with
	 * N = numHvalues rows
	 * M = numConditions columns
	 * NNZ = 4 * numConditions non-zero values (4 non-zero values in each 
	 * column)
	 * 
	 * The created matrix is the transposed support matrix, because it's simpler
	 * to create it in this form (due to specific of TAUCS interface).
	 */
	taucs_ccs_matrix* matrix = taucs_ccs_new(numHvalues, numConditions,
		4 * numConditions);

	int iCondition = 0;
	int nColumnOffset = 0;
	for (auto &edge : edgesReported)
	{
		matrix->colptr[iCondition] = nColumnOffset;
//		DEBUG_PRINT("Setting Q->colptr[%d] = %d", iCondition, nColumnOffset);
		
		int iVertex1 = edge.u0;
		int iVertex2 = edge.u1;
		int iVertex3 = edge.u2;
		int iVertex4 = edge.u3;

		/*
		 * Usual condition looks as follows:
		 *
		 * | h1   u1x   u1y   u1z | | 1   u1x   u1y   u1z |
		 * | h2   u2x   u2y   u2z | | 1   u2x   u2y   u2z |
		 * | h3   u3x   u3y   u3z | | 1   u3x   u3y   u3z |  >  0
		 * | h4   u4x   u4y   u4z | | 1   u4x   u4y   u4z |
		 *
		 * where h1, h2, h3 and h4 are support values which have
		 * numbers iVertex1, iVertex2, iVertex3 and iVertex4 here.
		 */

		Vector3d u1 = polyhedron->vertices[iVertex1];
		Vector3d u2 = polyhedron->vertices[iVertex2];
		Vector3d u3 = polyhedron->vertices[iVertex3];
		Vector3d u4 = polyhedron->vertices[iVertex4];
		double det1 = u2 % u3 * u4;
		double det2 = - u1 % u3 * u4;
		double det3 = u1 % u2 * u4;
		double det4 = - u1 % u2 * u3;
		double det = det1 + det2 + det3 + det4;
		if (det < 0)
		{
			det1 = -det1;
			det2 = -det2;
			det3 = -det3;
			det4 = -det4;
		}

		if (ifScaleMatrix)
		{
			double normRow = sqrt(det1 * det1 + det2 * det2 +
					det3 * det3 + det4 * det4);
			double coeff = 1. / normRow;
			det1 *= coeff;
			det2 *= coeff;
			det3 *= coeff;
			det4 *= coeff;
		}
		
		/*
		 * Now sort pairs of vertex's IDs and corresponding determinant values
		 * in order to store them to the matrix in the accending order.
		 */

		IrowAndValue iav1 = {iVertex1, det1};
		IrowAndValue iav2 = {iVertex2, det2};
		IrowAndValue iav3 = {iVertex3, det3};
		IrowAndValue iav4 = {iVertex4, det4};

		auto comparer = [](IrowAndValue a, IrowAndValue b)
		{
			return a.iVertex < b.iVertex;
		};

		set<IrowAndValue, decltype(comparer)> iavQuadruple(comparer);
		iavQuadruple.insert(iav1);
		iavQuadruple.insert(iav2);
		iavQuadruple.insert(iav3);
		iavQuadruple.insert(iav4);
		
		for (auto &iav : iavQuadruple)
		{
			DEBUG_PRINT("Printing value %.16lf to position (%d, %d)",
					iav.det, iav.iVertex, iCondition);
			matrix->rowind[nColumnOffset] = iav.iVertex;
			matrix->values.d[nColumnOffset] = iav.det;
			++nColumnOffset;
		}

		++iCondition;
	}
	
	matrix->colptr[iCondition] = nColumnOffset;

#ifndef NDEBUG
	for (int iCondition = 0; iCondition < numConditions; ++iCondition)
	{
		ASSERT(matrix->colptr[iCondition] == 4 * iCondition);
	}
	
	analyzeTaucsMatrix(matrix, false);
#endif
	
	DEBUG_END;
	return matrix;
}

static taucs_ccs_matrix* regularizeSupportMatrix(taucs_ccs_matrix* matrix,
	PolyhedronPtr polyhedron)
{
	DEBUG_START;

	/*
	 * Find IDs of vertices that have maximal coordinates of polyhedron's 
	 * vertices.
	 * 
	 * Columns of transposed support matrix which have these IDs will be then
	 * eliminated.
	 */
	Vector3d vMax = polyhedron->vertices[0];
	int iXmax = 0, iYmax = 0, iZmax = 0;
	for (int iVertex = 0; iVertex < polyhedron->numVertices; ++iVertex)
	{
		if (polyhedron->vertices[iVertex].x > vMax.x)
		{
			vMax.x = polyhedron->vertices[iVertex].x;
			iXmax = iVertex;
		}

		if (polyhedron->vertices[iVertex].y > vMax.y)
		{
			vMax.y = polyhedron->vertices[iVertex].y;
			iYmax = iVertex;
		}
		
		if (polyhedron->vertices[iVertex].z > vMax.z)
		{
			vMax.z = polyhedron->vertices[iVertex].z;
			iZmax = iVertex;
		}
	}
	/* TODO: Handle case when some of iXmax, iYmax, iZmax are equal. */
	DEBUG_PRINT("%d-th vertex has maximal X coordinate = %lf", iXmax, vMax.x);
	DEBUG_PRINT("%d-th vertex has maximal Y coordinate = %lf", iYmax, vMax.y);
	DEBUG_PRINT("%d-th vertex has maximal Z coordinate = %lf", iZmax, vMax.z);
	
	/*
	 * Since we are going to divide onto components of vMax, we will calculate
	 * its reciprocal beforehand.
	 */
	vMax.x = 1. / vMax.x;
	vMax.y = 1. / vMax.y;
	vMax.z = 1. / vMax.z;
	
	/* TODO: Check that vx, vy, and vz are really eigenvectors of our matrix. */
	
	/* Allocate memory for regularized matrix. */
	taucs_ccs_matrix* matrixRegularized = taucs_ccs_new(matrix->m - 3, 
		matrix->n, 4 * (matrix->m - 3));
	DEBUG_PRINT("Memory for regularized matrix has been allocated.");
	
	int iConditionReg = 0;
	int nOffsetReg = 0;
	for (int iCondition = 0; iCondition < matrix->n; ++iCondition)
	{
		DEBUG_PRINT("Regularization of %d-th column of matrix.", iCondition);
		if ((iCondition == iXmax) ||
			(iCondition == iYmax) ||
			(iCondition == iZmax))
		{
			DEBUG_PRINT("Skipping the column that is going to be eliminated.");
			continue;
		}

		matrixRegularized->colptr[iConditionReg] = nOffsetReg;
		DEBUG_PRINT("Column pointer #%d has been set to %d", iConditionReg,
			nOffsetReg);

		for (int nOffset = matrix->colptr[iCondition];
			 nOffset < matrix->colptr[iCondition + 1]; ++nOffset)
		{
			DEBUG_PRINT("Reading matrix from offset %d", nOffset);
			
			matrixRegularized->rowind[nOffsetReg] = matrix->rowind[nOffset];
			DEBUG_PRINT("Row index at offset %d has been set to %d",
				nOffsetReg, matrix->rowind[nOffset]);

			int index = matrix->n * matrix->rowind[nOffset] + iCondition;
			DEBUG_PRINT("Actually it is [%d][%d], or %d-th element of the "
				"matrix", matrix->rowind[nOffset], iCondition, index);
			
			Vector3d vertex = polyhedron->vertices[index];
			DEBUG_PRINT("Corresponding vertex is (%lf, %lf, %lf)",
				vertex.x, vertex.x, vertex.z);
			
			matrixRegularized->values.d[nOffsetReg] =
				matrix->values.d[nOffset] * (1. - vertex * vMax);
			DEBUG_PRINT("Value at offset %d has been set to %lf", nOffsetReg,
				matrixRegularized->values.d[nOffsetReg]);

			++nOffsetReg;
		}
		
		++iConditionReg;
	}
	
	DEBUG_END;
	return matrixRegularized;
}

taucs_ccs_matrix* Recoverer::buildSupportMatrix(ShadeContourDataPtr SCData,	
	int& numConditions, int& numHvalues, double*& hvalues)
{
	DEBUG_START;

	/* 1. Balance contours if it is required. */
	if (ifBalancing)
	{
		balanceAllContours(SCData);
	}

	/* 2. Extract support planes from shadow contour data. */
	vector<Plane> supportPlanes = extractSupportPlanes(SCData);
	DEBUG_PRINT("Number of extracted support planes: %ld",
		(long unsigned int) supportPlanes.size());

	/* 3. Get normal vectors of support planes and normalize them. */
	vector<Vector3d> directions;
	numHvalues = supportPlanes.size();
	hvalues = new double[numHvalues];
	int iValue = 0;
	for (auto &plane : supportPlanes)
	{
		if (plane.dist < 0)
		{
			plane.dist = -plane.dist;
			plane.norm = -plane.norm;
		}
		Vector3d normal = plane.norm;
		normal.norm(1.);
		directions.push_back(normal);
		hvalues[iValue++] = plane.dist;
	}

	/* 4. Construct convex hull of the set of normal vectors. */
	PolyhedronPtr polyhedron = constructConvexHull(directions);
	polyhedron->preprocessAdjacency();
	for (int iVertex = 0; iVertex < polyhedron->numVertices; ++iVertex)
	{
		int degree = polyhedron->vertexInfos[iVertex].numFacets;
		DEBUG_PRINT("Vertex %d has degree %d", iVertex, degree);
		if (degree > 3)
		{
			DEBUG_PRINT("--- more than 3");
		}
	}

	/* 5. Build matrix by the polyhedron. */
	taucs_ccs_matrix* matrix = buildMatrixByPolyhedron(polyhedron, 
		numConditions, ifScaleMatrix);

	/*
	 * 6. Regularize the undefinite matrix using known 3 kernel basis vectors.
	 * v1 = (u1x, ..., uMx)
	 * v2 = (u1y, ..., uMy)
	 * v3 = (u1z, ..., uMz)
	 */
	taucs_ccs_matrix* matrixRegularized = regularizeSupportMatrix(matrix,
		polyhedron);
	taucs_ccs_free(matrix);

	DEBUG_END;
	return matrixRegularized;
}

static double l1_distance(int n, double* x, double* y)
{
	DEBUG_START;
	double result = 0.;
	for (int i = 0; i < n; ++i)
	{
		result += fabs(x[i] - y[i]);
	}
	DEBUG_END;
	return result;
}

static double l2_distance(int n, double* x, double* y)
{
	DEBUG_START;
	double result = 0.;
	for (int i = 0; i < n; ++i)
	{
		result += (x[i] - y[i]) * (x[i] - y[i]);
	}
	DEBUG_END;
	return result;
}

static double linf_distance(int n, double* x, double* y)
{
	DEBUG_START;
	double result = 0.;
	for (int i = 0; i < n; ++i)
	{
		result = fabs(x[i] - y[i]) > result ? fabs(x[i] - y[i]) : result;
	}
	DEBUG_END;
	return result;
}

PolyhedronPtr Recoverer::run(ShadeContourDataPtr SCData)
{
	DEBUG_START;
	int numConditions = 0, numHvalues = 0;
	double *hvalues = NULL;

	/* 1. Build the support matrix. */
	taucs_ccs_matrix* Qt = buildSupportMatrix(SCData, numConditions, numHvalues,
		hvalues);	
	analyzeTaucsMatrix(Qt, false);
	DEBUG_PRINT("Matrix has been built.");
	DEBUG_PRINT("Qt has %d rows and %d columns", Qt->m, Qt->n);

	/* Enable highest level of verbosity in TSNNLS package. */
	tsnnls_verbosity(10);
	char* strStderr = strdup("stderr");
	taucs_logfile(strStderr);

	/* 2. Calculate the reciprocal of the condition number. */
	double conditionNumberQt = taucs_rcond(Qt);
	DEBUG_PRINT("rcond(Q) = %.16lf",
			conditionNumberQt);

	double inRelErrTolerance = conditionNumberQt * conditionNumberQt *
			EPS_MIN_DOUBLE;
	DEBUG_PRINT("inRelErrTolerance = %.16lf", inRelErrTolerance);

	double outResidualNorm;

	/* 3. Run the main TSNNLS algorithm of minimization. */
	double* h = t_snnls(Qt, hvalues, &outResidualNorm, inRelErrTolerance + 100.,
			1);
	DEBUG_PRINT("Function t_snnls has returned pointer %p.", (void*) h);

	char* errorTsnnls;
	tsnnls_error(&errorTsnnls);
	ALWAYS_PRINT(stdout, "Error from tsnnls: %s", errorTsnnls);

	DEBUG_PRINT("outResidualNorm = %.16lf", outResidualNorm);

	ALWAYS_PRINT(stdout, "||h - h0||_{1} = %.16lf",
			l1_distance(numHvalues, hvalues, h));
	ALWAYS_PRINT(stdout, "||h - h0||_{2} = %.16lf",
			l2_distance(numHvalues, hvalues, h));
	ALWAYS_PRINT(stdout, "||h - h0||_{inf} = %.16lf",
			linf_distance(numHvalues, hvalues, h));

	free(h);
	taucs_ccs_free(Qt);

	DEBUG_END;
	return NULL;
}
