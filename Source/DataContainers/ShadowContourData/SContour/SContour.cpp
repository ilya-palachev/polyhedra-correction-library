/*
 * Copyright (c) 2009-2015 Ilya Palachev <iliyapalachev@gmail.com>
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

/**
 * @file SContour.cpp
 * @brief The implementation of (non-io) functions of SContour class,
 */

#include <CGAL/convex_hull_2.h>

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "DataContainers/ShadowContourData/SContour/SContour.h"

SContour::SContour() :
	id(-1),
	ns(0),
	plane(Plane(Vector3d(0, 0, 0), 0)),
	poly(NULL),
	sides(NULL)
{
	DEBUG_START;
	DEBUG_END;
}

SContour::SContour(const SContour& orig) :
	id(orig.id),
	ns(orig.ns),
	plane(orig.plane),
	poly(orig.poly),
	sides()
{
	DEBUG_START;
	auto DEBUG_VARIABLE normal = orig.plane.norm;
	ASSERT(!!plane.norm && "orig.plane.norm is null std::vector");
	sides = new SideOfContour[ns + 1];
	for (int i = 0; i < ns; ++i)
	{
		sides[i] = orig.sides[i];
	}
	DEBUG_END;
}

SContour::SContour(std::vector<Point_3> points, Plane planeOrig) :
	id(-1),
	ns((int) points.size()),
	plane(planeOrig),
	poly(NULL),
	sides()
{
	DEBUG_START;
	sides = new SideOfContour[ns];
	auto point = points.begin();
	for (int i = 0; i < ns; ++i)
	{
		ASSERT(point != points.end());
		sides[i].A1 = Vector3d(point->x(), point->y(), point->z());
		++point;
	}
	for (int i = 0; i < ns; ++i)
	{
		int iNext = (ns + i + 1) % ns;
		sides[i].A2 = sides[iNext].A1;
		ASSERT(std::isfinite(qmod(sides[i].A1 - sides[i].A2)));
		ASSERT(qmod(sides[i].A1 - sides[i].A2) > 0);
	}
	DEBUG_END;
}

SContour::~SContour()
{
	DEBUG_START;

	DEBUG_PRINT("Attention! shadow contour %d is being deleted now...\n", this->id);
	DEBUG_PRINT("polyhedron use count: %ld", poly.use_count());

	if (sides != NULL)
	{
		delete[] sides;
		sides = NULL;
	}
	DEBUG_END;
}

SContour& SContour::operator =(const SContour& scontour)
{
	DEBUG_START;
	id = scontour.id;
	ns = scontour.ns;
	plane = scontour.plane;
	poly = scontour.poly;

	if (sides)
	{
		delete[] sides;
		sides = NULL;
	}

	sides = new SideOfContour[ns];

	for (int iSide = 0; iSide < ns; ++iSide)
	{
		sides[iSide] = scontour.sides[iSide];
		ASSERT(sides[iSide].A1 == scontour.sides[iSide].A1);
		ASSERT(sides[iSide].A2 == scontour.sides[iSide].A2);
	}

	DEBUG_END;
	return *this;
}

bool SContour::operator ==(const SContour& scontour) const
{
	DEBUG_START;

	if (id != scontour.id)
	{
		DEBUG_END;
		return false;
	}

	if (ns != scontour.ns)
	{
		DEBUG_END;
		return false;
	}

	if (plane != scontour.plane)
	{
		DEBUG_END;
		return false;
	}

	if (poly != scontour.poly)
	{
		DEBUG_END;
		return false;
	}

	for (int iSide = 0; iSide < ns; ++iSide)
	{
		if (sides[iSide] != scontour.sides[iSide])
		{
			DEBUG_END;
			return false;
		}
	}

	DEBUG_END;
	return true;
}

bool SContour::operator !=(const SContour& scontour) const
{
	DEBUG_START;
	bool returnValue = !(*this == scontour);
	DEBUG_END;
	return returnValue;
}

std::vector<Vector3d> SContour::getPoints()
{
	DEBUG_START;
	std::vector<Vector3d> points;

	/* Iterate through the array of sides of current contour. */
	/*
	 * FIXME: Is it true that contours are ususally not connected between first
	 * and last sides?
	 */
	for (int iSide = 0; iSide < ns; ++iSide)
	{
		SideOfContour* sideCurr = &sides[iSide];
		ASSERT(std::isfinite(sideCurr->A1.x));
		ASSERT(std::isfinite(sideCurr->A1.y));
		ASSERT(std::isfinite(sideCurr->A1.z));

		/* Project current vertex to the plane of contour. */
		Vector3d vCurr = plane.project(sideCurr->A1);
		points.push_back(vCurr);
		
		/*
		 * Check that second vertex of previous side lies closely to first
		 * vertex of the current side.
		 */
		int iSidePrev = (ns + iSide - 1) % ns;
		SideOfContour* sidePrev DEBUG_VARIABLE =
				&sides[iSidePrev];
		ASSERT(std::isfinite(sidePrev->A2.x));
		ASSERT(std::isfinite(sidePrev->A2.y));
		ASSERT(std::isfinite(sidePrev->A2.z));
		DEBUG_PRINT("sides[%d]->A2 = (%lf, %lf, %lf)",
				iSidePrev,
				sidePrev->A2.x, sidePrev->A2.y,
				sidePrev->A2.z);
		DEBUG_PRINT("sides[%d]->A1 = (%lf, %lf, %lf)",
				iSide, sideCurr->A1.x, sideCurr->A1.y,
				sideCurr->A1.z);
		Vector3d diff DEBUG_VARIABLE = sidePrev->A2 - sideCurr->A1;
		ASSERT(std::isfinite(diff.x));
		ASSERT(std::isfinite(diff.y));
		ASSERT(std::isfinite(diff.z));
		DEBUG_PRINT("   difference = (%lf, %lf, %lf)",
				diff.x, diff.y, diff.z);


		if (qmod(diff) >= EPS_MIN_DOUBLE)
		{
			vCurr = plane.project(sidePrev->A2);
			points.push_back(vCurr);
			DEBUG_PRINT(COLOUR_RED
				"Warning: contour %d is not connected! "
				"A gap found between %d-th and %d-th sides."
				COLOUR_NORM, id, iSidePrev, iSide);
		}
	}
	DEBUG_END;
	return points;
}

static std::vector<Point_2> mapPointsToOXYplane(std::vector<Vector3d> points,
		Vector3d nu)
{
	DEBUG_START;
	Vector3d ez(0., 0., 1.);
	nu.norm(1.);
	Vector3d tau = nu % ez;

	double xCurr = 0., yCurr = 0.;
	std::vector<Point_2> pointsMapped;
	for(auto &point : points)
	{
		xCurr = point * tau;
		yCurr = point * ez;
		pointsMapped.push_back(Point_2(xCurr, yCurr));
	}
	DEBUG_END;
	return pointsMapped;
}

static std::vector<Point_3> mapPointsFromOXYplane(std::vector<Point_2> points,
		Vector_3 nu)
{
	DEBUG_START;

	ASSERT(!!Vector3d(nu.x(), nu.y(), nu.z()) && "nu is null vector");

	Vector_3 ez(0., 0, 1.);
	double length = sqrt(nu.squared_length());
	ASSERT(std::fpclassify(length) != FP_ZERO);
	nu = nu * 1. / length; /* Normalize std::vector \nu. */
	ASSERT(std::isfinite(nu.x()));
	ASSERT(std::isfinite(nu.y()));
	ASSERT(std::isfinite(nu.z()));
	Vector_3 tau = cross_product(nu, ez);

	std::vector<Point_3> pointsMapped;
	CGAL::Origin o;
	for (auto &point : points)
	{
		pointsMapped.push_back(o + tau * point.x() + ez * point.y());
	}
	DEBUG_END;
	return pointsMapped;
}

SContour *SContour::convexify()
{
	DEBUG_START;
	
	ASSERT(!!plane.norm && "nu is null vector");

	/*
	 * TODO: This is a temporal workaround. In future we need to perform
	 * convexification so that to remember what points are vertices of
	 * convex hull.
	 */
	auto points = getPoints();
	auto pointsMapped = mapPointsToOXYplane(points, plane.norm);
	
	std::vector<Point_2> hull;
	convex_hull_2(pointsMapped.begin(), pointsMapped.end(),
		std::back_inserter(hull));
	
	if ((int) hull.size() != (int) points.size())
	{
		ASSERT(hull.size() < points.size());
			MAIN_PRINT(COLOUR_RED
			"Warning: contour #%d is non-convex, its hull "
				"contains %d of %d of its points"
			COLOUR_NORM, id, (int) hull.size(),
			(int) points.size());
	}

	/* TODO: Add automatic conversion from Vector3d to Vector_3 !!! */
	auto extremePoints = mapPointsFromOXYplane(hull,
				Vector_3(plane.norm.x, plane.norm.y,
					plane.norm.z));

	SContour *contour = new SContour(extremePoints, plane);
	contour->id = id;
	contour->poly = poly;
#ifndef NDEBUG
	for (int iSide = 0; iSide < contour->ns; ++iSide)
	{
		SideOfContour *side = &contour->sides[iSide];
		ASSERT(qmod(side->A1 - side->A2) > 0);
	}
#endif /* NDEBUG */
	DEBUG_END;
	return contour;
}
