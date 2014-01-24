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

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "Polyhedron/Facet/Facet.h"

void Facet::my_fprint(FILE* file)
{
	DEBUG_START;
	int i;
	REGULAR_PRINT(file, "Facet %d: ", id);
	if (indVertices == NULL && numVertices > 0)
	{
		ERROR_PRINT("Incorrect configuration of facet.");
		DEBUG_END;
		return;
	}
	for (i = 0; i < numVertices; ++i)
		REGULAR_PRINT(file, "%d ", indVertices[i]);
	REGULAR_PRINT(file, "\n");
	DEBUG_END;
}

void Facet::my_fprint_all(FILE* file)
{
	DEBUG_START;
	int i;
	REGULAR_PRINT(file, "\n------------ Facet %d: ------------\n", id);
	REGULAR_PRINT(file, "id = %d\n nv = %d\n", id, numVertices);
	REGULAR_PRINT(file, "a = %lf, b = %lf, c = %lf, d = %lf\n", plane.norm.x,
			plane.norm.y, plane.norm.z, plane.dist);
	REGULAR_PRINT(file,
			"plane : (%.2lf) * x + (%.2lf) * y + (%.2lf) * z + (%.2lf) = 0.)\n",
			plane.norm.x, plane.norm.y, plane.norm.z, plane.dist);

	if (indVertices == NULL && numVertices > 0)
	{
		ERROR_PRINT("Incorrect configuration of facet.");
		DEBUG_END;
		return;
	}

	if (numVertices == 0)
	{
		REGULAR_PRINT(file, "Facet is empty.\n");
		DEBUG_END;
		return;
	}

	REGULAR_PRINT(file, "index :       ");
	for (i = 0; i < numVertices; ++i)
	{
		REGULAR_PRINT(file, "%d ", indVertices[i]);
	}
	REGULAR_PRINT(file, "(%d)", indVertices[numVertices]);

	/* For debugging purposes we assert when the cycling vertex is not equal to
	 * the first vertex. */
	ASSERT(indVertices[numVertices] == indVertices[0]);

	REGULAR_PRINT(file, "\nnext facets : ");
	for (i = numVertices + 1; i < 2 * numVertices + 1; ++i)
	{
		REGULAR_PRINT(file, "%d ", indVertices[i]);
	}

	REGULAR_PRINT(file, "\npositions :   ");
	for (i = 2 * numVertices + 1; i < 3 * numVertices + 1; ++i)
	{
		REGULAR_PRINT(file, "%d ", indVertices[i]);
	}

	REGULAR_PRINT(file, "\nrgb = (%d, %d, %d)\n", rgb[0], rgb[1], rgb[2]);

	if (auto polyhedron = parentPolyhedron.lock())
	{
		for (int iVertex = 0; iVertex < numVertices; ++iVertex)
		{
			int ind = indVertices[iVertex];
			if (ind >= polyhedron->numVertices || ind < 0)
			{
				DEBUG_PRINT("Invalid index of vertex in facet #%d", id);
				DEBUG_END;
				return;
			}
			DEBUG_VARIABLE Vector3d vector = polyhedron->vertices[ind];
			REGULAR_PRINT(file, "vertices[%d] = (%lf, %lf, %lf)\n", ind,
							vector.x, vector.y, vector.z);
		}
		test_pair_neighbours();
	}
	else
	{
		ERROR_PRINT("parentPolyhedron expired");
		ASSERT(0 && "parentPolyhedron expired");
		DEBUG_END;
		return;
	}
	
	DEBUG_END;
}

void Facet::fprint_default_0(FILE* file)
{
	DEBUG_START;
	ALWAYS_PRINT(file, "%d %.16lf %.16lf %.16lf %.16lf", numVertices, plane.norm.x,
			plane.norm.y, plane.norm.z, plane.dist);
	for (int j = 0; j < numVertices; ++j)
		ALWAYS_PRINT(file, " %d", indVertices[j]);
	ALWAYS_PRINT(file, "\n");
	DEBUG_END;
}

void Facet::fprint_default_1(FILE* file)
{
	DEBUG_START;
	ALWAYS_PRINT(file, "%d %d %.16lf %.16lf %.16lf %.16lf", id, numVertices,
			plane.norm.x, plane.norm.y, plane.norm.z, plane.dist);
	for (int j = 0; j < numVertices; ++j)
		ALWAYS_PRINT(file, " %d", indVertices[j]);
	ALWAYS_PRINT(file, "\n");
	DEBUG_END;
}

void Facet::fprint_my_format(FILE* file)
{
	DEBUG_START;
	ALWAYS_PRINT(file, "%d %.16lf %.16lf %.16lf %.16lf", numVertices, plane.norm.x,
			plane.norm.y, plane.norm.z, plane.dist);
	for (int j = 0; j < 3 * numVertices + 1; ++j)
		ALWAYS_PRINT(file, " %d", indVertices[j]);
	ALWAYS_PRINT(file, "\n");
	DEBUG_END;
}

void Facet::fprint_ply_vertex(FILE* file)
{
	DEBUG_START;
	int v_id;
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
		return;
	}
	
	for (int j = 0; j < numVertices; ++j)
	{
		v_id = indVertices[j];
		ALWAYS_PRINT(file, "%.16lf %.16lf %.16lf %.16lf %.16lf %.16lf\n",
				vertices[v_id].x,
				vertices[v_id].y,
				vertices[v_id].z, plane.norm.x, plane.norm.y,
				plane.norm.z);
	}
	DEBUG_END;
}

void Facet::fprint_ply_index(FILE* file)
{
	DEBUG_START;
	ALWAYS_PRINT(file, "%d", numVertices);
	for (int j = 0; j < numVertices; ++j)
		ALWAYS_PRINT(file, " %d", indVertices[j]);
	ALWAYS_PRINT(file, "\n");
	DEBUG_END;
}

void Facet::fprint_ply_scale(FILE* file)
{
	DEBUG_START;
	ALWAYS_PRINT(file, "%d", numVertices);
	for (int j = 0; j < numVertices; ++j)
		ALWAYS_PRINT(file, " %d", indVertices[j]);
	ALWAYS_PRINT(file, " %d %d %d\n", rgb[0], rgb[1], rgb[2]);
	DEBUG_END;
}
