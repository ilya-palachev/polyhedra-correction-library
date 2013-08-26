#include "PolyhedraCorrectionLibrary.h"

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
		REGULAR_PRINT(file, "%d ", indVertices[i]);
	REGULAR_PRINT(file, "(%d)", indVertices[numVertices]);

	/* For debugging purposes we assert when the cycling vertex is not equal to
	 * the first vertex. */
	ASSERT(indVertices[numVertices] == indVertices[0]);

	REGULAR_PRINT(file, "\nnext facets : ");
	for (i = numVertices + 1; i < 2 * numVertices + 1; ++i)
		REGULAR_PRINT(file, "%d ", indVertices[i]);

	REGULAR_PRINT(file, "\npositions :   ");
	for (i = 2 * numVertices + 1; i < 3 * numVertices + 1; ++i)
		REGULAR_PRINT(file, "%d ", indVertices[i]);

	REGULAR_PRINT(file, "\nrgb = (%d, %d, %d)\n", rgb[0], rgb[1], rgb[2]);

	if (parentPolyhedron == NULL)
	{
		DEBUG_PRINT("Warning! parentPolyhedron == NULL !\n");
	}
	else if (parentPolyhedron->vertices == NULL)
	{
		DEBUG_PRINT("Warning! parentPolyhedron->vertices == NULL !\n");
	}
	else
	{
		for (int iVertex = 0; iVertex < numVertices; ++iVertex)
		{
			int ind = indVertices[iVertex];
			if (ind >= parentPolyhedron->numVertices || ind < 0)
			{
				DEBUG_PRINT("Invalid index of vertex in facet #%d", id);
				DEBUG_END;
				return;
			}
			Vector3d vector = parentPolyhedron->vertices[ind];
			REGULAR_PRINT(file, "vertices[%d] = (%lf, %lf, %lf)\n", ind, vector.x,
					vector.y, vector.z);
		}
	}
	test_pair_neighbours();
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
	for (int j = 0; j < numVertices; ++j)
	{
		v_id = indVertices[j];
		ALWAYS_PRINT(file, "%.16lf %.16lf %.16lf %.16lf %.16lf %.16lf\n",
				parentPolyhedron->vertices[v_id].x,
				parentPolyhedron->vertices[v_id].y,
				parentPolyhedron->vertices[v_id].z, plane.norm.x, plane.norm.y,
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
