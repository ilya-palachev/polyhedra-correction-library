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

/**
 * @file SupportFunctionDataItemExtractor.h
 * @brief Extractor of support function data item from given contour side
 * declaration.
 */

#ifndef SUPPORT_FUNCTION_DATA_ITEM_EXTRACTOR_H
#define SUPPORT_FUNCTION_DATA_ITEM_EXTRACTOR_H

#include "Constants.h"
#include "DataContainers/SupportFunctionData/SupportFunctionDataItem.h"
#include "DataContainers/ShadowContourData/SideOfContour/SideOfContour.h"

/** Extractor of support function data item from given contour side. */
class SupportFunctionDataItemExtractor
{
public:
	/** Empty constructor. */
	SupportFunctionDataItemExtractor();

	/** Destructor. */
	virtual ~SupportFunctionDataItemExtractor();

	/**
	 * Extracts support function data item from a given shadow contour
	 * side.
	 *
	 * @param side	Shadow contour side.
	 * @return Support function data item extracted from given side.
	 */
	virtual SupportFunctionDataItem run(SideOfContour *side);
};

/** The limit for which small values are transformed to 0. */
#define EPS_CGAL_SIGNED_ZERO_BUG_WORKAROUND_LIMIT (10. * EPS_MIN_DOUBLE)

/**
 * Nulls the value if it is less than the limit.
 *
 * @param a	The value
 */
static inline void nullValueIfSmall(double &a)
{
	a = fabs(a) < EPS_CGAL_SIGNED_ZERO_BUG_WORKAROUND_LIMIT ? 0. : a;
}

/** The limit for checked planarity of contour. */
#define EPS_SHADOW_CONTOUR_PLANARITY_LIMIT 1e-10

/**
 * Checks that given point lies in the given plane with some precision.
 *
 * @param point	The point.
 * @param plane	The plane.
 * @return Whether the point lies in the plane.
 */
static inline bool ifPointLiesInPlane(Vector3d point, Plane plane)
{
	return fabs(plane % point) <= EPS_SHADOW_CONTOUR_PLANARITY_LIMIT;
}

#endif /* SUPPORT_FUNCTION_DATA_ITEM_EXTRACTOR_H */
