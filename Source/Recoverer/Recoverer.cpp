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

static double l1_distance(int n, double* x, double* y);
static double l1_norm(int n, double* x);
static double l2_distance(int n, double* x, double* y);
static double l2_norm(int n, double* x);
static double linf_distance(int n, double* x, double* y);
static double linf_norm(int n, double* x);

static void checkPolyhedronIDs(Polyhedron_3 polyhedron)
{
#ifndef NDEBUG
	DEBUG_START;
	long int iVertex = 0;
	for (auto vertex = polyhedron.vertices_begin();
			vertex != polyhedron.vertices_end(); ++vertex)
	{
		ASSERT(iVertex++ == vertex->id);
	}

	long int iFacet = 0;
	for (auto facet = polyhedron.facets_begin();
		facet != polyhedron.facets_end(); ++facet)
	{
		ASSERT(iFacet++ == facet->id);
	}

	long int iHalfedge = 0;
	for (auto halfedge = polyhedron.halfedges_begin();
		halfedge != polyhedron.halfedges_end(); ++halfedge)
	{
		ASSERT(iHalfedge++ == halfedge->id);
	}
	DEBUG_END;
#endif
}

#define DEFAULT_MAX_COORDINATE 1000000.

Recoverer::Recoverer() :
	ifBalancing(false), ifScaleMatrix(false), iXmax(0), iYmax(0), iZmax(0),
	vectorRegularizing(0., 0., 0.)
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
		Point_3 point0 = h->vertex()->point();
		Point_3 point1 = h->next()->vertex()->point();
		Point_3 point2 = h->opposite()->vertex()->point();
		return Polyhedron_3::Plane_3(point0, point1, point2);
	}
};

Polyhedron_3 Recoverer::constructConvexHullCGAL (vector<Vector3d> points)
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

	/* Assign vertex IDs that will be used later. */
	long int i = 0;
	for (auto vertex = poly.vertices_begin(); vertex != poly.vertices_end();
		 ++vertex)
	{
		vertex->id = i++;
	}

	/* Assign facet IDs that will be used later. */
	i = 0;
	for (auto facet = poly.facets_begin(); facet != poly.facets_end(); ++facet)
	{
		facet->id = i++;
	}
	
	/* Assert halfedge IDs that will be checked later. */
	i = 0;
	for (auto halfedge = poly.halfedges_begin();
		 halfedge != poly.halfedges_end(); ++halfedge)
	{
		halfedge->id = i++;
	}
	DEBUG_END;
	return poly;
}

PolyhedronPtr Recoverer::constructConvexHull (vector<Vector3d> points)
{
	DEBUG_START;
	Polyhedron_3 poly = constructConvexHullCGAL(points);

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

	/*m
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
#ifndef NDEBUG

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
#endif
	DEBUG_END;
}

/**
 * Stores the handles of vertexes that form a tetrahedron.
 */
typedef struct
{
	Polyhedron_3::Vertex_handle v0, v1, v2, v3;
} TetrahedronVertex;

/**
 * Counts the number of covering tetrahedrons for a given polyhedron.
 *
 * @param polyhedron	Convex hull of the set of directions
 */
static unsigned long int countCoveringTetrahedrons(Polyhedron_3& polyhedron)
{
	DEBUG_START;

	/*
	 * Test: Check whether the proper number of tetrahedrons has been found.
	 *
	 * First, calcualate the sum of vertex degrees
	 */
	unsigned long int degreeSum = 0;
	long int iVertex = 0;
	for (auto vertex = polyhedron.vertices_begin();
			vertex != polyhedron.vertices_end(); ++vertex)
	{
		ASSERT(iVertex++ == vertex->id);
		degreeSum += vertex->vertex_degree();
	}

	/* Second, the number of edges is a half of degrees sum. */
	unsigned long int numTetrahedrons = degreeSum / 2;

	/*
	 * Third, each of 3 edges incident to any vertex of degree 3, share the same
	 * tetrahedron, so we need to decrease the number of tetrahedrons by 2.
	 */
	for (auto vertex = polyhedron.vertices_begin();
			vertex != polyhedron.vertices_end(); ++vertex)
	{
		unsigned long int degree = vertex->vertex_degree();
		if (degree == 3)
		{
			numTetrahedrons -= 2;
		}
	}

	DEBUG_PRINT("Counted number of tetrahedrons must be %ld", numTetrahedrons);

	DEBUG_END;
	return numTetrahedrons;
}

/**
 * Finds covering tetrahedrons for a given polyhedron.
 *
 * @param polyhedron	Convex hull of the set of directions
 */
static list<TetrahedronVertex> findCoveringTetrahedrons(
		Polyhedron_3& polyhedron)
{
	DEBUG_START;

	/* Check that all facets of the polyhedron are triangles. */
	for (auto halfedge = polyhedron.halfedges_begin();
			halfedge != polyhedron.halfedges_end(); ++halfedge)
	{
		if (!halfedge->is_triangle())
		{
			ERROR_PRINT("Facet #%ld is not triangle, it has %ld edges.",
					(long int) halfedge->facet()->id,
					(long int) halfedge->facet_degree());
			DEBUG_END;
			exit(EXIT_FAILURE);
		}
	}

	auto comparer = [](TetrahedronVertex a, TetrahedronVertex b)
	{
		long int a0 = a.v0->id, a1 = a.v1->id, a2 = a.v2->id, a3 = a.v3->id;
		long int b0 = b.v0->id, b1 = b.v1->id, b2 = b.v2->id, b3 = b.v3->id;

		if (a0 < b0)
		{
			return true;
		}
		else if (a0 == b0)
		{
			if (a1 < b1)
			{
				return true;
			}
			else if (a1 == b1)
			{
				if (a2 < b2)
				{
					return true;
				}
				else if (a2 == b2)
				{
					if (a3 < b3)
					{
						return true;
					}
				}
			}
		}
		return false;
	};


	set<TetrahedronVertex, decltype(comparer)> tetrahedrons(comparer);

	auto vertexComparer = [](Polyhedron_3::Vertex_handle a,
			Polyhedron_3::Vertex_handle b)
	{
		return a->id < b->id;
	};

	set<Polyhedron_3::Vertex_handle, decltype(vertexComparer)>
				vertexSet(vertexComparer);

	for (auto halfedge = polyhedron.halfedges_begin();
			halfedge != polyhedron.halfedges_end(); ++halfedge)
	{

		vertexSet.clear();

		vertexSet.insert(halfedge->vertex());
		vertexSet.insert(halfedge->prev()->vertex());
		vertexSet.insert(halfedge->next()->vertex());
		vertexSet.insert(halfedge->opposite()->next()->vertex());

		ASSERT(vertexSet.size() == 4);

		TetrahedronVertex tetrahedron;
		auto itVertex = vertexSet.begin();
		tetrahedron.v0 = *(itVertex++);
		tetrahedron.v1 = *(itVertex++);
		tetrahedron.v2 = *(itVertex++);
		tetrahedron.v3 = *(itVertex++);

		tetrahedrons.insert(tetrahedron);
	}

	/*
	 * Construct the list from sorted set and return it.
	 */
	list<TetrahedronVertex> listTetrahedrons;
	for (auto itTetrahedron = tetrahedrons.begin();
			itTetrahedron != tetrahedrons.end(); ++itTetrahedron)
	{
		listTetrahedrons.push_back(*itTetrahedron);
	}

	DEBUG_END;
	return listTetrahedrons;
}

/**
 * Builds matrix of constraints from the polyhedron (which represents a convex
 * hull of the set of directions for which the support values are given).
 *
 * @param polyhedron	Convex hull of the set of directions
 * @param numConditions	The number of constraints (output)
 * @param matrix		The matrix of constraints (output)
 */
static taucs_ccs_matrix* buildMatrixByPolyhedron(Polyhedron_3 polyhedron,
		int& numConditions, bool ifScaleMatrix)
{
	DEBUG_START;

	int numHvalues = polyhedron.size_of_vertices();

	auto listTetrahedrons = findCoveringTetrahedrons(polyhedron);

#ifndef NDEBUG
	/* Check whether the proper number of tetrahedrons has been found. */
	ASSERT(listTetrahedrons.size() == countCoveringTetrahedrons(polyhedron));
#endif

	numConditions = listTetrahedrons.size();
	DEBUG_PRINT("Found %d covering tetrahedrons", numConditions);

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
	for (auto &tetrahedron : listTetrahedrons)
	{
		matrix->colptr[iCondition] = nColumnOffset;
//		DEBUG_PRINT("Setting Q->colptr[%d] = %d", iCondition, nColumnOffset);
		
		Point_3 zero(0., 0., 0.);
		Vector_3 u1(zero, tetrahedron.v0->point());
		Vector_3 u2(zero, tetrahedron.v1->point());
		Vector_3 u3(zero, tetrahedron.v2->point());
		Vector_3 u4(zero, tetrahedron.v3->point());

		int iVertex1 = tetrahedron.v0->id;
		int iVertex2 = tetrahedron.v1->id;
		int iVertex3 = tetrahedron.v2->id;
		int iVertex4 = tetrahedron.v3->id;

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

		double det1 = + CGAL::determinant(u2, u3, u4);
		double det2 = - CGAL::determinant(u1, u3, u4);
		double det3 = + CGAL::determinant(u1, u2, u4);
		double det4 = - CGAL::determinant(u1, u2, u3);
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
		
		matrix->rowind[nColumnOffset] = iVertex1;
		matrix->values.d[nColumnOffset] = det1;
		++nColumnOffset;
		matrix->rowind[nColumnOffset] = iVertex2;
		matrix->values.d[nColumnOffset] = det2;
		++nColumnOffset;
		matrix->rowind[nColumnOffset] = iVertex3;
		matrix->values.d[nColumnOffset] = det3;
		++nColumnOffset;
		matrix->rowind[nColumnOffset] = iVertex4;
		matrix->values.d[nColumnOffset] = det4;
		++nColumnOffset;

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


Vector_3 Recoverer::findMaxVertices(Polyhedron_3 polyhedron,
	int& imax0, int& imax1, int& imax2)
{
	DEBUG_START;
	int iVertex = polyhedron.vertices_begin()->id;
	iXmax = iYmax = iZmax = iVertex;
	
	/*
	 * These 3 variables are added to hadle the case when some of iXmax, iYmax
	 * or iZmax are equal.
	 */
	int iXmaxKeeper = iXmax;
	int iYmaxKeeper = iYmax;
	int iZmaxKeeper = iZmax;

	Point_3 pointFirst = polyhedron.vertices_begin()->point();
	double xmax = pointFirst.x(), ymax = pointFirst.y(), zmax = pointFirst.z();

	for (auto vertex = polyhedron.vertices_begin();
			vertex != polyhedron.vertices_end(); ++vertex)
	{
		iVertex = vertex->id;
		Point_3 point = vertex->point();

		if (point.x() > xmax)
		{
			xmax = point.x();
			iXmaxKeeper = iXmax;
			iXmax = iVertex;
		}

		if (point.y() > ymax)
		{
			ymax = point.y();
			iYmaxKeeper = iYmax;
			iYmax = iVertex;
		}
		
		if (point.z() > zmax)
		{
			zmax = point.z();
			iZmaxKeeper = iZmax;
			iZmax = iVertex;
		}
	}

	DEBUG_PRINT("%d-th vertex has maximal X coordinate = %lf", iXmax, xmax);
	DEBUG_PRINT("%d-th vertex has maximal Y coordinate = %lf", iYmax, ymax);
	DEBUG_PRINT("%d-th vertex has maximal Z coordinate = %lf", iZmax, zmax);

	/* Sort indices of vertices with maximal coordinates. */
	set<int> iMaxes;
	iMaxes.insert(iXmax);
	iMaxes.insert(iYmax);
	iMaxes.insert(iZmax);
	
	/*
	 * TODO: Handle case when iXmax = iYmax = iZmax and
	 * iMaxKeeper = iYmaxKeeper = iZmaxKeeper
	 */
	if (iMaxes.size() < 3)
	{
		if (iXmax == iYmax)
		{
			iYmax = iYmaxKeeper;
			iMaxes.insert(iYmax);
		}
		if (iYmax == iZmax)
		{
			iZmax = iZmaxKeeper;
			iMaxes.insert(iZmax);
		}
		if (iXmax == iZmax)
		{
			iXmax = iXmaxKeeper;
			iMaxes.insert(iXmax);
		}
	}
	ASSERT(iMaxes.size() == 3);

	auto itIMaxes = iMaxes.begin();
	imax0 = *(itIMaxes++);
	imax1 = *(itIMaxes++);
	imax2 = *(itIMaxes++);
	DEBUG_PRINT("And here are they sorted: %d, %d, %d", imax0, imax1, imax2);

	DEBUG_END;
	return Vector_3(1. / xmax, 1. / ymax, 1. / zmax);
}

taucs_ccs_matrix* Recoverer::regularizeSupportMatrix(taucs_ccs_matrix* matrix,
	double* hvalues, Polyhedron_3 polyhedron)
{
	DEBUG_START;

	taucs_ccs_matrix* Q = taucs_ccs_transpose(matrix);

	int imax0 = 0, imax1 = 0, imax2 = 0;
	Vector_3 vMax = findMaxVertices(polyhedron, imax0, imax1, imax2);
	vectorMaxHValues = Vector_3(hvalues[iXmax], hvalues[iYmax], hvalues[iZmax]);
	vectorRegularizing = Vector_3(
		hvalues[iXmax] * vMax.x(),
		hvalues[iYmax] * vMax.y(),
		hvalues[iZmax] * vMax.z());

	/* Regularize h-values vector. */
	Point_3 zero(0., 0., 0.);
	for (int iValue = 0; iValue < Q->n - 3; ++iValue)
	{
		/* Get vertex corresponding to the column of the matrix. */
		auto vertex = polyhedron.vertices_begin() + iValue;
		Point_3 point = vertex->point();
		Vector_3 vector(zero, point);
		double shift = -vector * vectorRegularizing;
		if (iValue < imax0)
			hvalues[iValue] += shift;
		else if (iValue < imax1 - 1)
			hvalues[iValue] = hvalues[iValue + 1] + shift;
		else if (iValue < imax2 - 2)
			hvalues[iValue] = hvalues[iValue + 2] + shift;
		else
			hvalues[iValue] = hvalues[iValue + 3] + shift;
	}
	DEBUG_PRINT("h-values have been successfully regularized.");
	
	
	/* Allocate memory for regularized matrix. */
	taucs_ccs_matrix* Qregularized = taucs_ccs_new(Q->m, Q->n - 3, 4 * Q->m);
	DEBUG_PRINT("Memory for regularized matrix has been allocated.");
	
	int iColumnReg = 0;
	int nOffsetReg = 0;
	for (int iColumn = 0; iColumn < Q->n; ++iColumn)
	{
		DEBUG_PRINT("Regularization of %d-th column of matrix.", iColumn);
		if ((iColumn == imax0) || (iColumn == imax1) || (iColumn == imax2))
		{
			DEBUG_PRINT("==================================================");
			DEBUG_PRINT("Skipping the column that is going to be eliminated.");
			continue;
		}

		Qregularized->colptr[iColumnReg] = nOffsetReg;
		DEBUG_PRINT("Column pointer #%d has been set to %d", iColumnReg,
			nOffsetReg);

		for (int nOffset = Q->colptr[iColumn];
			 nOffset < Q->colptr[iColumn + 1]; ++nOffset)
		{
			Qregularized->rowind[nOffsetReg] = Q->rowind[nOffset];
			Qregularized->values.d[nOffsetReg] = Q->values.d[nOffset];
			++nOffsetReg;
		}

		++iColumnReg;
	}
	Qregularized->colptr[iColumnReg] = nOffsetReg;
	
	taucs_ccs_matrix* matrixRegularized = taucs_ccs_transpose(Qregularized);
	taucs_ccs_free(Q);
	taucs_ccs_free(Qregularized);

	DEBUG_END;
	return matrixRegularized;
}

#define MAX_ACCEPTABLE_LINF_ERROR 1e-6

/**
 * Checks whether vectors vx, vy, vz are really the eigenvectors of the matrix Q
 * corresponding to the eigenvalue 0 (i. e. basis kernel vectors)
 *
 * @param polyhedron	The polyhedron (convex hull of support directions)
 * @param Qt			The transpose of the support matrix
 */
static bool checkSupportMatrix(Polyhedron_3 polyhedron, taucs_ccs_matrix* Qt)
{
	DEBUG_START;
	/* Set eigenvectors vx, vy, vz. */
	int numVertices = polyhedron.size_of_vertices();
	DEBUG_PRINT("Allocating 6 arrays of of size %d of type double",
			numVertices);
	double* eigenVectorX = new double[numVertices];
	double* eigenVectorY = new double[numVertices];
	double* eigenVectorZ = new double[numVertices];
	double* productX = new double[Qt->n];
	double* productY = new double[Qt->n];
	double* productZ = new double[Qt->n];

	auto itVertex = polyhedron.vertices_begin();
	for (int iVertex = 0; iVertex < numVertices; ++iVertex)
	{
		ASSERT(itVertex->id == iVertex);
		Point_3 point = itVertex->point();
		eigenVectorX[iVertex] = point.x();
		eigenVectorY[iVertex] = point.y();
		eigenVectorZ[iVertex] = point.z();
		++itVertex;
	}

	/* Check that eigenVectorX, eigenVectorY, vz really lie in the kernel of Q. */
	taucs_transpose_vec_times_matrix_nosub(eigenVectorX, Qt, productX);
	double errorXinf = linf_norm(numVertices, productX);
	DEBUG_PRINT("||eigenVectorX^{T} Q^{T}||_inf = %.16lf", errorXinf);
	double errorX1 = l1_norm(numVertices, productX);
	DEBUG_PRINT("||eigenVectorX^{T} Q^{T}||_1 = %.16lf", errorX1);
	double errorX2 = l2_norm(numVertices, productX);
	DEBUG_PRINT("||eigenVectorX^{T} Q^{T}||_2 = %.16lf", errorX2);

	taucs_transpose_vec_times_matrix_nosub(eigenVectorY, Qt, productY);
	double errorYinf = linf_norm(numVertices, productY);
	DEBUG_PRINT("||eigenVectorY^{T} Q^{T}||_inf = %.16lf", errorYinf);
	double errorY1 = l1_norm(numVertices, productY);
	DEBUG_PRINT("||eigenVectorY^{T} Q^{T}||_1 = %.16lf", errorY1);
	double errorY2 = l2_norm(numVertices, productY);
	DEBUG_PRINT("||eigenVectorY^{T} Q^{T}||_2 = %.16lf", errorY2);

	taucs_transpose_vec_times_matrix_nosub(eigenVectorZ, Qt, productZ);
	double errorZinf = linf_norm(numVertices, productZ);
	DEBUG_PRINT("||eigenVectorZ^{T} Q^{T}||_inf = %.16lf", errorZinf);
	double errorZ1 = l1_norm(numVertices, productZ);
	DEBUG_PRINT("||eigenVectorZ^{T} Q^{T}||_1 = %.16lf", errorZ1);
	double errorZ2 = l2_norm(numVertices, productZ);
	DEBUG_PRINT("||eigenVectorZ^{T} Q^{T}||_2 = %.16lf", errorZ2);

	delete[] eigenVectorX;
	delete[] eigenVectorY;
	delete[] eigenVectorZ;
	delete[] productX;
	delete[] productY;
	delete[] productZ;
	DEBUG_END;
	return (errorXinf < MAX_ACCEPTABLE_LINF_ERROR) &&
		(errorYinf < MAX_ACCEPTABLE_LINF_ERROR) &&
		(errorZinf < MAX_ACCEPTABLE_LINF_ERROR);
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
	Polyhedron_3 polyhedron = constructConvexHullCGAL(directions);
	checkPolyhedronIDs(polyhedron);

	/* 5. Build matrix by the polyhedron. */
	taucs_ccs_matrix* Qt = buildMatrixByPolyhedron(polyhedron,
		numConditions, ifScaleMatrix);
	checkPolyhedronIDs(polyhedron);

	/* 5.1. Check that vx, vy, and vz are really eigenvectors of our matrix. */
	bool ifCheckSuccessful = checkSupportMatrix(polyhedron, Qt);
	ASSERT_PRINT(ifCheckSuccessful, "Bad matrix.");

	/*
	 * 6. Regularize the undefinite matrix using known 3 kernel basis vectors.
	 * v1 = (u1x, ..., uMx)
	 * v2 = (u1y, ..., uMy)
	 * v3 = (u1z, ..., uMz)
	 */
	taucs_ccs_matrix* matrixRegularized = regularizeSupportMatrix(Qt, hvalues,
		polyhedron);
	checkPolyhedronIDs(polyhedron);
	taucs_ccs_free(Qt);

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

static double l1_norm(int n, double* x)
{
	DEBUG_START;
	double result = 0.;
	for (int i = 0; i < n; ++i)
	{
		result += fabs(x[i]);
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

static double l2_norm(int n, double* x)
{
	DEBUG_START;
	double result = 0.;
	for (int i = 0; i < n; ++i)
	{
		result += (x[i]) * (x[i]);
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

static double linf_norm(int n, double* x)
{
	DEBUG_START;
	double result = 0.;
	for (int i = 0; i < n; ++i)
	{
		result = fabs(x[i]) > result ? fabs(x[i]) : result;
	}
	DEBUG_END;
	return result;
}

PolyhedronPtr Recoverer::run(ShadeContourDataPtr SCData)
{
	DEBUG_START;
	int numConditions = 0, numHvalues = 0;
	double *hvalues = NULL;

	/* 1. Build the transpose of support matrix. */
	taucs_ccs_matrix* Qt = buildSupportMatrix(SCData, numConditions, numHvalues,
		hvalues);	
	analyzeTaucsMatrix(Qt, false);
	DEBUG_PRINT("Matrix has been built.");
	DEBUG_PRINT("Qt has %d rows and %d columns", Qt->m, Qt->n);

	/* Enable highest level of verbosity in TSNNLS package. */
	tsnnls_verbosity(10);
	char* strStderr = strdup("stderr");
	taucs_logfile(strStderr);

	/*
	 * 2.Calculate the reciprocal of the condition number.
	 *
	 * In order to be able to do this, we need first to transpose Qt to obtain
	 * the support matrix Q, since LAPACK's routine DGECON usually fails for
	 * matrices where the number of rows is less than the number of columns.
	 *
	 * You can see this at this page:
	 * http://www.netlib.org/lapack/explore-html/db/de4/dgecon_8f_source.html
	 *
	 * Here is the error:
	 *  ** On entry to DGECON parameter number  4 had an illegal value
	 *
	 * Here is the source code that generates the error:
	 *
	 * ELSE IF( lda.LT.max( 1, n ) ) THEN
	 * 		info = -4
	 * ...
	 * IF( info.NE.0 ) THEN
	 * 		CALL xerbla( 'DGECON', -info )
	 * 		RETURN
	 */

	double* Qtvals = taucs_convert_ccs_to_doubles(Qt);
	taucs_ccs_matrix* fixed = taucs_construct_sorted_ccs_matrix(Qtvals, Qt->n,
			Qt->m);
	free(Qtvals);
	taucs_ccs_free(Qt);
	Qt = fixed;

	taucs_ccs_matrix* Q = taucs_ccs_transpose(Qt);
	double conditionNumberQ = taucs_rcond(Q);
	DEBUG_PRINT("rcond(Q) = %.16lf",
			conditionNumberQ);

	double inRelErrTolerance = conditionNumberQ * conditionNumberQ *
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
	taucs_ccs_free(Q);
	taucs_ccs_free(Qt);

	DEBUG_END;
	return NULL;
}
