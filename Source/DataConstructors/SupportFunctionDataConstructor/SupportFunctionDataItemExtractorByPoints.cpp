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
 * @file SupportFunctionDataItemExtractorByPoints.cpp
 * @brief Extractor of support function data item from given contour side
 * by using only the points of the given side.
 * implementation.
 */

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "DataConstructors/SupportFunctionDataConstructor/SupportFunctionDataItemExtractorByPoints.h"

SupportFunctionDataItemExtractorByPoints::SupportFunctionDataItemExtractorByPoints() :
	SupportFunctionDataItemExtractor()
{
	DEBUG_START;
	DEBUG_END;
}

SupportFunctionDataItemExtractorByPoints::~SupportFunctionDataItemExtractorByPoints()
{
	DEBUG_START;
	DEBUG_END;
}

SupportFunctionDataItem SupportFunctionDataItemExtractorByPoints::run(
		SideOfContour *side)
{
	DEBUG_START;
	Vector3d A1 = side->A1;
	Vector3d A2 = side->A2;
	ASSERT(!!(A1 - A2) && qmod(A1 - A2) > 0.);

	/*
	 * The normal to the plane of contour can be calculated as
	 * side->A1 % side->A2,
	 *
	 * but the final expression can be simplified as follows:
	 * [a - b, [a, b]] = a (a - b, b) - b (a - b, a) =
	 *  = (a + b) (a, b) - a (b, b) - b (a, a)
	 *
	 *  And it seems that this can help to improve th precision.
	 */
	Vector3d normal = (A1 + A2) * (A1 * A2) - A1 * (A2 * A2) - A2 * (A1 * A1);
	ASSERT(!!normal && qmod(normal) > 0.);
	normal.norm(1.);

	/* CGAL::convex_hull_3 is buggy for 1e-15 values, so we null them. */
	nullValueIfSmall(normal.x);
	nullValueIfSmall(normal.y);
	nullValueIfSmall(normal.z);

	/*
	 * The only way to find right orientation of normal is to check that
	 * both points of the contour side lie at the positive halfspace of the
	 * plane.
	 */
	normal = normal * A1 < 0. ? -normal : normal;
	ASSERT(normal * A1 > 0.);
	ASSERT(normal * A2 > 0.);

	/* Calculate the support value corresponding to calculated direction */
	double value = normal * A1;

	/* Create support function data item. */
	SupportFunctionDataItem item;
	item.direction = normal;
	item.value = value;

	DEBUG_END;
	return item;
}
