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
	DEBUG_PRINT("Number of extracted support planes: %d",
				supportPlanes.size());

	/* 2. Map planes to dual space to obtain the set of points in it. */
	vector<Vector3d> supportPoints = mapPlanesToDualSpace(supportPlanes);

	/* 3. Construct convex hull in the dual space. */
	Polyhedron_3 polyhedronDual = constructConvexHull(supportPoints);

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
			ASSERT(error1 < 100 * EPS_MIN_DOUBLE);
			ASSERT(error2 < 100 * EPS_MIN_DOUBLE);
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

Polyhedron_3 Recoverer::constructConvexHull (vector<Vector3d> points)
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

	/* Use the algorithm of standard STATIC convex hull. */
	CGAL::convex_hull_3(pointsCGAL.begin(), pointsCGAL.end(), poly);

	/* Calculate equations of planes for each facet. */
	std::transform(poly.facets_begin(), poly.facets_end(), poly.planes_begin(),
			Plane_from_facet());

	DEBUG_PRINT("Convex hull of %d points contains %d extreme points.",
		points.size(), poly.size_of_vertices());

	DEBUG_END;
	return poly;
}
