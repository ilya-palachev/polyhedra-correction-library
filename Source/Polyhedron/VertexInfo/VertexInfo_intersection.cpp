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

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "Polyhedron/VertexInfo/VertexInfo.h"
#include "Polyhedron/Polyhedron.h"

int VertexInfo::intersection_find_next_facet(Plane iplane, int facet_id, Polyhedron &polyhedron)
{
	DEBUG_START;

	int sgn_curr = polyhedron.signum(polyhedron.vertices[indFacets[2 * numFacets]], iplane);
	for (int i = 0; i < numFacets; ++i)
	{
		int sgn_prev = sgn_curr;
		sgn_curr = polyhedron.signum(polyhedron.vertices[indFacets[i + numFacets + 1]], iplane);
		if (sgn_curr != sgn_prev)
		{
			if (indFacets[i] != facet_id)
			{
				DEBUG_END;
				return indFacets[i];
			}
		}
	}
	return -1;
}
