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
 * @file SupportFunctionDataItemInfo.h
 * @brief General information about support data item (its origin)
 * - declaration.
 */

#ifndef SUPPORT_FUNCTION_DATA_ITEM_INFO_H
#define SUPPORT_FUNCTION_DATA_ITEM_INFO_H

#include <memory>
#include "KernelCGAL/KernelCGAL.h"

class SupportFunctionDataItemInfo
{
public:
	/** The support point (tangient point). */
	Vector3d point;

	/** The ID of the item in the shadow contour (if it is from contour). */
	int iContour;

	/** The number of sides in the mother contour. */
	int numSidesContour;

	/** The original segment in the shadow contour. */
	Segment_3 segment;

	/** The normal vector to the original shadow contour. */
	Vector_3 normalShadow;

	/** The number of side in the shadow contour. */
	int iSide;

	/**
	 * The ID if the next item, that is its counter-clockwise neighbor on
	 * the current side on the shadow contour that it belongs to.
	 */
	int iNext;

	/** Empty constructor. */
	SupportFunctionDataItemInfo();

	/** Assignment operator. */
	SupportFunctionDataItemInfo &
	operator=(const SupportFunctionDataItemInfo &i);

	/** Virtual destructor. */
	virtual ~SupportFunctionDataItemInfo();
};

typedef std::shared_ptr<SupportFunctionDataItemInfo>
	SupportFunctionDataItemInfoPtr;

#endif /* SUPPORT_FUNCTION_DATA_ITEM_INFO_H */
