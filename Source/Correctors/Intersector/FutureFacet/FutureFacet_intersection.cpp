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

#include "Correctors/Intersector/EdgeSetIntersected/EdgeSetIntersected.h"
#include "Correctors/Intersector/FutureFacet/FutureFacet.h"
#include "DebugAssert.h"
#include "DebugPrint.h"

void FutureFacet::generate_facet(Facet &facet, int fid, Plane &iplane, int numv,
								 EdgeSetIntersected *es)
{
	DEBUG_START;
	int i, v0, v1, res_id;
	int *index;

	index = new int[nv];

	for (i = 0; i < nv; ++i)
	{
		v0 = edge0[i];
		v1 = edge1[i];
		if (v0 == v1)
			index[i] = v0;
		else
		{
			res_id = es->search_edge(v0, v1);
			if (res_id < 0)
			{
				ERROR_PRINT("Cannot find edge %d %d in edge set", v0, v1);
				res_id = -1;
			}
			else
			{
				res_id += numv;
			}
			index[i] = res_id;
		}
	}
	facet = Facet(fid, nv, iplane, index, NULL, false);
	if (index != NULL)
		delete[] index;
	DEBUG_END;
}
