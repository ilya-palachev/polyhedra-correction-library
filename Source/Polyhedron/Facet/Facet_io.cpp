#include "PolyhedraCorrectionLibrary.h"

void Facet::my_fprint(FILE* file)
{
#ifndef NDEBUG
	int i;
	fprintf(file, "Facet %d: ", id);
	for (i = 0; i < numVertices; ++i)
		fprintf(file, "%d ", indVertices[i]);
	printf("\n");
#endif
}

void Facet::my_fprint_all(FILE* file)
{
#ifndef NDEBUG
	int i;
	fprintf(file, "\n------------ Facet %d: ------------\n", id);
	fprintf(file, "id = %d\n nv = %d\n", id, numVertices);
	fprintf(file, "a = %lf, b = %lf, c = %lf, d = %lf\n", plane.norm.x,
			plane.norm.y, plane.norm.z, plane.dist);
	fprintf(file,
			"plane : (%.2lf) * x + (%.2lf) * y + (%.2lf) * z + (%.2lf) = 0.)\n",
			plane.norm.x, plane.norm.y, plane.norm.z, plane.dist);

	fprintf(file, "index :       ");
	for (i = 0; i < numVertices; ++i)
		fprintf(file, "%d ", indVertices[i]);

	fprintf(file, "\nnext facets : ");
	for (i = numVertices + 1; i < 2 * numVertices + 1; ++i)
		fprintf(file, "%d ", indVertices[i]);

	fprintf(file, "\npositions :   ");
	for (i = 2 * numVertices + 1; i < 3 * numVertices + 1; ++i)
		fprintf(file, "%d ", indVertices[i]);

	fprintf(file, "\nrgb = (%d, %d, %d)\n", rgb[0], rgb[1], rgb[2]);

	if (parentPolyhedron == NULL)
		fprintf(file, "Warning! poly == NULL !\n");
	else
	{
		for (int iVertex = 0; iVertex < numVertices; ++iVertex)
		{
			int ind = indVertices[iVertex];
			Vector3d vector = parentPolyhedron->vertices[indVertices[iVertex]];
			fprintf(file, "vertices[%d] = (%lf, %lf, %lf)\n", ind, vector.x,
					vector.y, vector.z);
		}
	}
//
//	else
//		fprintf(file, "poly->numf = %d\n", poly->numf);
	test_pair_neighbours();
#endif
}

void Facet::fprint_default_0(FILE* file)
{
	fprintf(file, "%d %.16lf %.16lf %.16lf %.16lf", numVertices, plane.norm.x,
			plane.norm.y, plane.norm.z, plane.dist);
	for (int j = 0; j < numVertices; ++j)
		fprintf(file, " %d", indVertices[j]);
	fprintf(file, "\n");
}

void Facet::fprint_default_1(FILE* file)
{
	fprintf(file, "%d %d %.16lf %.16lf %.16lf %.16lf", id, numVertices,
			plane.norm.x, plane.norm.y, plane.norm.z, plane.dist);
	for (int j = 0; j < numVertices; ++j)
		fprintf(file, " %d", indVertices[j]);
	fprintf(file, "\n");
}

void Facet::fprint_my_format(FILE* file)
{
	fprintf(file, "%d %.16lf %.16lf %.16lf %.16lf", numVertices, plane.norm.x,
			plane.norm.y, plane.norm.z, plane.dist);
	for (int j = 0; j < 3 * numVertices + 1; ++j)
		fprintf(file, " %d", indVertices[j]);
	fprintf(file, "\n");
}

void Facet::fprint_ply_vertex(FILE* file)
{
	int v_id;
	for (int j = 0; j < numVertices; ++j)
	{
		v_id = indVertices[j];
		fprintf(file, "%.16lf %.16lf %.16lf %.16lf %.16lf %.16lf\n",
				parentPolyhedron->vertices[v_id].x,
				parentPolyhedron->vertices[v_id].y,
				parentPolyhedron->vertices[v_id].z, plane.norm.x, plane.norm.y,
				plane.norm.z);
	}
}

void Facet::fprint_ply_index(FILE* file)
{
	fprintf(file, "%d", numVertices);
	for (int j = 0; j < numVertices; ++j)
		fprintf(file, " %d", indVertices[j]);
	fprintf(file, "\n");
}

void Facet::fprint_ply_scale(FILE* file)
{
	fprintf(file, "%d", numVertices);
	for (int j = 0; j < numVertices; ++j)
		fprintf(file, " %d", indVertices[j]);
	fprintf(file, " %d %d %d\n", rgb[0], rgb[1], rgb[2]);
}
