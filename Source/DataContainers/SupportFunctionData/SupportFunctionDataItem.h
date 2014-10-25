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
 * (i. e. number of contour, number of side or anything else).
 */

#ifndef SUPPORT_FUNCTION_DATA_ITEM_H
#define SUPPORT_FUNCTION_DATA_ITEM_H

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
	SupportFunctionDataItemInfo *info;

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

	/**
	 * Assignment operator.
	 *
	 * @param item	The original item.
	 */
	SupportFunctionDataItem &operator= (const SupportFunctionDataItem
			&item);

	/** Destructor. */
	virtual ~SupportFunctionDataItem();
};

#endif /* SUPPORT_FUNCTION_DATA_ITEM_H */
