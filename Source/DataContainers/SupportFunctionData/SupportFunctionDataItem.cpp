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
 * @file SupportFunctionDataItem.cpp
 * @brief Support function data items contain support value and support
 * direction assiciated with it. Also it can contain a pointer to auxiliary
 * info structure that stores the information about the origin of this value
 * (i. e. number of contour, number of side or anything else)
 * - implementation.
 */

#include "DebugPrint.h"
#include "DataContainers/SupportFunctionData/SupportFunctionDataItem.h"

SupportFunctionDataItem::SupportFunctionDataItem() : direction(Vector3d(0., 0., 0.)), value(0.), info(NULL)
{
	DEBUG_START;
	DEBUG_END;
}

SupportFunctionDataItem::SupportFunctionDataItem(const SupportFunctionDataItem &item) :
	direction(item.direction), value(item.value), info(item.info)
{
	DEBUG_START;
	DEBUG_END;
}

SupportFunctionDataItem::SupportFunctionDataItem(const Vector3d d, const double v) : direction(d), value(v), info(NULL)
{
	DEBUG_START;
	DEBUG_END;
}
SupportFunctionDataItem::~SupportFunctionDataItem()
{
	DEBUG_START;
	DEBUG_END;
}

SupportFunctionDataItem &SupportFunctionDataItem::operator=(const SupportFunctionDataItem &item)
{
	DEBUG_START;
	direction = item.direction;
	value = item.value;
	info = item.info;
	DEBUG_END;
	return *this;
}
