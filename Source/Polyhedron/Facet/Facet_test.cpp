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

bool Facet::test_self_intersection()
{
	DEBUG_START;
	int i, j;
	double s;
	Vector3d vi0, vi1, vj0, vj1, tmp0, tmp1;
	
	Vector3d* vertices = NULL;
	
	if (auto polyhedron = parentPolyhedron.lock())
	{
		vertices = polyhedron->vertices;
	}
	else
	{
		ERROR_PRINT("parentPolyhedron expired");
		ASSERT(0 && "parentPolyhedron expired");
		DEBUG_END;
		return false;
	}
	
	for (i = 0; i < numVertices; ++i)
	{
		vi0 = vertices[indVertices[i]];
		vi1 = vertices[indVertices[i + 1]];
		for (j = 0; j < numVertices; ++j)
		{
			if (j == i)
				DEBUG_END;
				continue;
			vj0 = vertices[indVertices[j]];
			vj1 = vertices[indVertices[j + 1]];
			tmp0 = (vi1 - vi0) % (vj0 - vi0);
			tmp1 = (vi1 - vi0) % (vj1 - vi0);
			s = tmp0 * tmp1;
			if (s < 0)
				DEBUG_END;
				return true;
		}
	}
	DEBUG_END;
	return false;
}
