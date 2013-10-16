/* 
 * Copyright (c) 2009-2013 Ilya Palachev <iliyapalachev@gmail.com>
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

#include "PolyhedraCorrectionLibrary.h"

int VertexInfo::intersection_find_next_facet(Plane iplane, int facet_id)
{
	int i, sgn_prev, sgn_curr;
	sgn_curr = parentPolyhedron->signum(
			parentPolyhedron->vertices[indFacets[2 * numFacets]], iplane);
	for (i = 0; i < numFacets; ++i)
	{
		sgn_prev = sgn_curr;
		sgn_curr = parentPolyhedron->signum(
				parentPolyhedron->vertices[indFacets[i + numFacets + 1]],
				iplane);
		if (sgn_curr != sgn_prev)
			if (indFacets[i] != facet_id)
				return indFacets[i];
	}
	return -1;
}
