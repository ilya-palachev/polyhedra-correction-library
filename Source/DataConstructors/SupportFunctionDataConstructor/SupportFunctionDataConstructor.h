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
 * @file SupportFunctionDataConstructor.h
 * @brief Constructor class for support function data.
 */

#ifndef SUPPORT_FUNCTION_DATA_CONSTRUCTOR_H
#define SUPPORT_FUNCTION_DATA_CONSTRUCTOR_H

#include "DataContainers/SupportFunctionData/SupportFunctionData.h"
#include "DataContainers/ShadowContourData/ShadowContourData.h"
#include "DataContainers/ShadowContourData/SContour/SContour.h"
#include "DataContainers/ShadowContourData/SideOfContour/SideOfContour.h"
#include "Polyhedron_3/Polyhedron_3.h"

/** Constructs support function data from different types of related data. */
class SupportFunctionDataConstructor
{
private:
	/** Whether to balance or not shadow contours before processing. */
	bool ifBalanceShadowContours_;

	/** Whether to convexify or not shadow contours before processing. */
	bool ifConvexifyShadowContours_;

	/** Whether to extract support function data items only by points. */
	bool ifExtractItemsByPoints_;

	/** The vector used for shifting during balancing. */
	Vector3d balancingVector_;

	/**
	 * IDs points of tangient vertices (for case when data is constructed
	 * for the given polyhedron).
	 */
	std::vector<int> tangientIDs_;

public:
	/** Empty constructor. */
	SupportFunctionDataConstructor();

	/** Destructor. */
	~SupportFunctionDataConstructor();

	/** Enables shadow contours balancing. */
	void enableBalanceShadowContours();

	/** Enables shadow contours convexification. */
	void enableConvexifyShadowContour();

	/** Enables the extraction of support function data items by points. */
	void enableExtractItemsByPoints();

	Vector3d balancingVector()
	{
		return balancingVector_;
	}

	/**
	 * Generates support function data from shadow contour data.
	 *
	 * @param data			Shadow contour data
	 * @param numMaxContours	The maximum number of contours to be
	 * 				analyzed
	 *
	 * @return The support function data generated from shadow contours
	 */
	SupportFunctionDataPtr run(ShadowContourDataPtr data, int numMaxContours);

	/**
	 * Generates support function data from polyhedron and support
	 * directions by the definition of support function.
	 *
	 * @param polyhedron	The polyhedron
	 * @param directions	The vector of support directions
	 *
	 * @return The support function data generated for given polyhedron and
	 * support direcitons.
	 */
	SupportFunctionDataPtr run(std::vector<Point_3> directions,
		Polyhedron_3 polyhedron);

	/**
	 * Gets IDs of tangient vertices that have been obtained during the
	 * construction of support function data.
	 *
	 * @return IDs of tangient vertices.
	 */
	std::vector<int> getTangientIDs();
};

#endif /* SUPPORT_FUNCTION_DATA_CONSTRUCTOR_H */
