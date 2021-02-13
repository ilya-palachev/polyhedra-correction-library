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

#include <cmath>

#include "Constants.h"
#include "DataContainers/ShadowContourData/SideOfContour/SideOfContour.h"
#include "DebugAssert.h"
#include "DebugPrint.h"

SideOfContour::SideOfContour() :
	confidence(0),
	type(EEdgeUnknown),
	A1(Vector3d(0, 0, 0)),
	A2(Vector3d(0, 0, 0))
{
	DEBUG_START;
	DEBUG_END;
}

SideOfContour::SideOfContour(const SideOfContour &orig) :
	confidence(orig.confidence), type(orig.type), A1(orig.A1), A2(orig.A2)
{
	DEBUG_START;
	DEBUG_END;
}

SideOfContour::~SideOfContour()
{
	DEBUG_START;
	DEBUG_END;
}

bool SideOfContour::operator==(const SideOfContour &side) const
{
	DEBUG_START;

	if (fabs(confidence - side.confidence) > EPS_MIN_DOUBLE)
	{
		DEBUG_END;
		return false;
	}

	if (type != side.type)
	{
		DEBUG_END;
		return false;
	}

	if (A1 != side.A1)
	{
		DEBUG_END;
		return false;
	}

	if (A2 != side.A2)
	{
		DEBUG_END;
		return false;
	}

	DEBUG_END;
	return true;
}

bool SideOfContour::operator!=(const SideOfContour &side) const
{
	DEBUG_START;
	bool returnValue = !(*this == side);
	DEBUG_END;
	return returnValue;
}
