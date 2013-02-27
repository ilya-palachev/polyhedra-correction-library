#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Polyhedron.h"

void Facet::my_fprint(FILE* file) {
	int i;
	fprintf(file, "Facet %d: ", id);
	for (i = 0; i < nv; ++i)
		fprintf(file, "%d ", index[i]);
	printf("\n");
}

void Facet::my_fprint_all(FILE* file) {
	int i;
	fprintf(file, "\n------------ Facet %d: ------------\n", id);
	fprintf(file, "id = %d\n nv = %d\n", id, nv);
	fprintf(file, "a = %lf, b = %lf, c = %lf, d = %lf\n",
			plane.norm.x, plane.norm.y, plane.norm.z, plane.dist);
	fprintf(file, "plane : (%.2lf) * x + (%.2lf) * y + (%.2lf) * z + (%.2lf) = 0.)\n",
			plane.norm.x, plane.norm.y, plane.norm.z, plane.dist);
	
	fprintf(file, "index :       ");
	for (i = 0; i < nv; ++i)
		fprintf(file, "%d ", index[i]);

	fprintf(file, "\nnext facets : ");
	for (i = nv + 1; i < 2 * nv + 1; ++i)
		fprintf(file, "%d ", index[i]);

	fprintf(file, "\npositions :   ");
	for (i = 2 * nv + 1; i < 3 * nv + 1; ++i)
		fprintf(file, "%d ", index[i]);

	fprintf(file, "\nrgb = (%d, %d, %d)\n",
			rgb[0], rgb[1], rgb[2]);

	if(poly == NULL)
		fprintf(file, "Warning! poly == NULL !\n");
//
//	else
//		fprintf(file, "poly->numf = %d\n", poly->numf);
        test_pair_neighbours();

}

void Facet::fprint_default_0(FILE* file) {
	fprintf(file, "%d %.16lf %.16lf %.16lf %.16lf",
		  nv,
		  plane.norm.x,
		  plane.norm.y,
		  plane.norm.z,
		  plane.dist);
	for(int j = 0; j < nv; ++j)
		fprintf(file, " %d", index[j]);
	fprintf(file, "\n");
}

void Facet::fprint_default_1(FILE* file) {
	fprintf(file, "%d %d %.16lf %.16lf %.16lf %.16lf",
		id,
		nv,
		plane.norm.x,
		plane.norm.y,
		plane.norm.z,
		plane.dist);
	for(int j = 0; j < nv; ++j)
		fprintf(file, " %d", index[j]);
	fprintf(file, "\n");
}

void Facet::fprint_my_format(FILE* file) {
	fprintf(file, "%d %.16lf %.16lf %.16lf %.16lf",
		  nv,
		  plane.norm.x,
		  plane.norm.y,
		  plane.norm.z,
		  plane.dist);
	for(int j = 0; j < 3 * nv + 1; ++j)
		fprintf(file, " %d", index[j]);
	fprintf(file, "\n");
}

void Facet::fprint_ply_vertex(FILE* file) {
	int v_id;
	for(int j = 0; j < nv; ++j)
	{
		v_id = index[j];
		fprintf(file, "%.16lf %.16lf %.16lf %.16lf %.16lf %.16lf\n",
				poly->vertex[v_id].x,
				poly->vertex[v_id].y,
				poly->vertex[v_id].z,
				plane.norm.x,
				plane.norm.y,
				plane.norm.z);
	}
}

void Facet::fprint_ply_index(FILE* file) {
	fprintf(file, "%d", nv);
	for(int j = 0; j < nv; ++j)
		fprintf(file, " %d", index[j]);
	fprintf(file, "\n");
}

void Facet::fprint_ply_scale(FILE* file) {
	fprintf(file, "%d", nv);
	for(int j = 0; j < nv; ++j)
		fprintf(file, " %d", index[j]);
	fprintf(file, " %d %d %d\n", rgb[0], rgb[1], rgb[2]);
}
