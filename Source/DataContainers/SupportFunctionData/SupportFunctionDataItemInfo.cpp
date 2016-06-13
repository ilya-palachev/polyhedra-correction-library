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
 * @file SupportFunctionDataItemInfo.cpp
 * @brief General information about support data item (its origin)
 * - implementation.
 */

#include "DebugPrint.h"
#include "Vector3d.h"
#include "DataContainers/SupportFunctionData/SupportFunctionDataItemInfo.h"

SupportFunctionDataItemInfo::SupportFunctionDataItemInfo() :
	point(Vector3d(0., 0., 0.)),
	iContour(-1),
	numSidesContour(-1),
	segment(),
	normalShadow()
{
	DEBUG_START;
	DEBUG_END;
}

SupportFunctionDataItemInfo &SupportFunctionDataItemInfo::operator=(
			const SupportFunctionDataItemInfo &info)
{
	DEBUG_START;
	point = info.point;
	iContour = info.iContour;
	numSidesContour = info.numSidesContour;
	segment = info.segment;
	normalShadow = info.normalShadow;
	DEBUG_END;
	return *this;
}

SupportFunctionDataItemInfo::~SupportFunctionDataItemInfo()
{
	DEBUG_START;
	DEBUG_END;
}
