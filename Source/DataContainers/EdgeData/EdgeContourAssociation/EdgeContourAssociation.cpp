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

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "DataContainers/EdgeData/EdgeContourAssociation/EdgeContourAssociation.h"

EdgeContourAssociation::EdgeContourAssociation() :
				indContour(-1),
				indNearestSide(-1),
				ifProperDirection(false),
				weight(0.)
{
	DEBUG_START;
	DEBUG_END;
}

EdgeContourAssociation::EdgeContourAssociation(int indContour_orig,
		int indNearestSide_orig, bool ifProperDirection_orig,
		double weight_orig) :
				indContour(indContour_orig),
				indNearestSide(indNearestSide_orig),
				ifProperDirection(ifProperDirection_orig),
				weight(weight_orig)
{
	DEBUG_START;
	DEBUG_END;
}

EdgeContourAssociation::EdgeContourAssociation(int indContour_orig) :
				indContour(indContour_orig),
				indNearestSide(-1),
				ifProperDirection(false),
				weight(0.)
{
	DEBUG_START;
	DEBUG_END;
}

EdgeContourAssociation::EdgeContourAssociation(
		const EdgeContourAssociation& orig) :
				indContour(orig.indContour),
				indNearestSide(orig.indNearestSide),
				ifProperDirection(orig.ifProperDirection),
				weight(orig.weight)
{
	DEBUG_START;
	DEBUG_END;
}

EdgeContourAssociation::~EdgeContourAssociation()
{
	DEBUG_START;
	DEBUG_END;
}

