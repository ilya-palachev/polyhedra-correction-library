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

/**
 * @file SupportFunctionDataItem.h
 * @brief Support function data items contain support value and support
 * direction assiciated with it. Also it can contain a pointer to auxiliary
 * info structure that stores the information about the origin of this value
 * (i. e. number of contour, number of side or anything else)
 * - declaration.
 */

#ifndef SUPPORT_FUNCTION_DATA_ITEM_H
#define SUPPORT_FUNCTION_DATA_ITEM_H

#include "DebugPrint.h"
#include "Vector3d.h"
#include "DataContainers/SupportFunctionData/SupportFunctionDataItemInfo.h"

/** Support function item with additional info. */
class SupportFunctionDataItem
{
public:
	/** The direction of support function measurement. */
	Vector3d direction;

	/** The support value corresonding to support direction. */
	double value;

	/** The additional information about support item. */
	SupportFunctionDataItemInfoPtr info;

	/** Empty constructor. */
	SupportFunctionDataItem();

	/**
	 * Copy constructor.
	 *
	 * @param item	The original item.
	 */
	SupportFunctionDataItem(const SupportFunctionDataItem &item);

	/**
	 * Constructor by values of direction vector and support value.
	 *
	 * @param d	The original support vector.
	 * @param v	The original support value.
	 */
	SupportFunctionDataItem(const Vector3d d, const double v);

	/** Destructor. */
	virtual ~SupportFunctionDataItem();

	/**
	 * Assignment operator.
	 *
	 * @param item	The original item.
	 */
	SupportFunctionDataItem &operator=(const SupportFunctionDataItem
			&item);
};

/**
 * Equality operator.
 *
 * @param left	The left side of equality.
 * @param right	The right side of equality.
 * @return True, if they are equal.
 */
inline bool operator==(const SupportFunctionDataItem &left,
	const SupportFunctionDataItem &right)
{
	DEBUG_START;
	bool ifEqual = left.direction == right.direction
		&& equal(left.value, right.value);
	DEBUG_END;
	return ifEqual; 
}

/**
 * Inequality operator.
 *
 * @param left	The left side of inequality.
 * @param right	The right side of inequality.
 * @return True, if they are inequal.
 */
inline bool operator!=(const SupportFunctionDataItem &left,
	const SupportFunctionDataItem &right)
{
	DEBUG_START;
	bool ifInequal = !(left == right);
	DEBUG_END;
	return ifInequal;
}

#endif /* SUPPORT_FUNCTION_DATA_ITEM_H */
