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

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "Constants.h"
#include "Recoverer/Recoverer.h"
#include "DataContainers/ShadeContourData/ShadeContourData.h"
#include "DataContainers/ShadeContourData/SContour/SContour.h"
#include "Polyhedron/Facet/Facet.h"
#include <VertexInfo.h>
#include <VertexInfo.h>

Recoverer::Recoverer()
{
	DEBUG_START;
	DEBUG_END;
}

Recoverer::~Recoverer()
{
	DEBUG_START;
	DEBUG_END;
}

Polyhedron* Recoverer::buildNaivePolyhedron(ShadeContourDataPtr SCData)
{
	DEBUG_START;

	/* 1. Extract support planes from shadow contour data. */
	vector<Plane> supportPlanes = extractSupportPlanes(SCData);
	DEBUG_PRINT("Number of extracted support planes: %ld",
		(long unsigned int) supportPlanes.size());

	/* 2. Map planes to dual space to obtain the set of points in it. */
	vector<Vector3d> supportPoints = mapPlanesToDualSpace(supportPlanes);

	/* 3. Construct convex hull in the dual space. */
	PolyhedronPtr polyhedronDual = constructConvexHull(supportPoints);

	/* 4. Map dual polyhedron to the primal space. */
	PolyhedronPtr polyhedron = buildDualPolyhedron(polyhedronDual);

	DEBUG_END;
	return NULL;
}

Polyhedron* Recoverer::buildDualNonConvexPolyhedron(ShadeContourDataPtr SCData)
{
	DEBUG_START;

	/* 1. Extract support planes from shadow contour data. */
	vector<Plane> supportPlanes = extractSupportPlanes(SCData);
	DEBUG_PRINT("Number of extracted support planes: %ld",
		(long unsigned int) supportPlanes.size());

	/* 2. Map planes to dual space to obtain the set of points in it. */
	vector<Vector3d> supportPoints = mapPlanesToDualSpace(supportPlanes);

	/* 3. Normalize all points so that to put the to the sphere. */
	vector<Vector3d> supportPointsNormalized;
	supportPointsNormalized.insert(supportPointsNormalized.begin(),
			supportPoints.begin(), supportPoints.end());
	int iVertex = 0;
	for (auto &v : supportPointsNormalized)
	{
		ASSERT(v == supportPoints[iVertex++]);
		v.norm(1.);
	}

	/* 4. Construct convex hull in the dual space. */
	PolyhedronPtr polyhedronDual = constructConvexHull(supportPointsNormalized);

	ASSERT((unsigned long int) polyhedronDual->numVertices ==
			supportPoints.size());

	/* 5. Restore saved coordinates of vectors. */
	iVertex = 0;
	for (auto &v : supportPoints)
	{
		polyhedronDual->vertices[iVertex++] = v;
	}

	/* 6. Print resulting polyhedron to the file. */
	polyhedronDual->fprint_ply_scale(1000000.,
		"../poly-data-out/poly-dual-nonconvex-debug.ply", "dual-polyhedron");

	DEBUG_END;
	return NULL;
}

Polyhedron* Recoverer::buildContours(ShadeContourDataPtr SCData)
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
		for (int iSide = 0; iSide < contourCurr->ns; ++iSide)
		{
			/* Nota Bene: here we read only 1st vertex of current side. */
			p->vertices[iVertex] = contourCurr->sides[iSide].A1;
			p->facets[iContour].indVertices[iSide] = iVertex;
			++iVertex;
		}
	}
	p->set_parent_polyhedron_in_facets();

	/* Print resulting polyhedron to the file. */
	p->fprint_ply_scale(1000000., "../poly-data-out/poly-contours.ply",
		"contours");

	DEBUG_END;
	return NULL;
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
		Vector3d normal = contourCurr->plane.norm;

		/* Iterate through the array of sides of current contour. */
		for (int iSide = 0; iSide < contourCurr->ns; ++iSide)
		{
			SideOfContour* sideCurr = &contourCurr->sides[iSide];

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
			
			double error1 = supportPlane.norm * sideCurr->A1 +
				supportPlane.dist;
			double error2 = supportPlane.norm * sideCurr->A2 +
				supportPlane.dist;

			DEBUG_PRINT("   side #%d\t%le\t%le", iSide, error1, error2);
			
			/* TODO: Here should be more strict conditions. */
			ASSERT(fabs(error1) < 100 * EPS_MIN_DOUBLE);
			ASSERT(fabs(error2) < 100 * EPS_MIN_DOUBLE);
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
	polyhedronDualPCL->fprint_ply_scale(1000000.,
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
	pDual->fprint_ply_scale(1000., "../poly-data-out/poly-primal-debug.ply", 
		"primal-polyhedron");

	DEBUG_END;
	return pDual;
}

