#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Polyhedron.h"

void VertexInfo::fprint_my_format(FILE* file) {
	fprintf(file, "%d ", nf);
	for(int j = 0; j < 2*nf + 1; ++j)
		fprintf(file, " %d", index[j]);
	fprintf(file, "\n");
}

void VertexInfo::my_fprint_all(FILE* file) {
	int i;
	fprintf(file, "\n------------ VertexInfo %d: ------------\n", id);
//	fprintf(file, "id = %d\nnf = %d\n", id, nf);
//	fprintf(file, "vector = (%.2lf, %.2lf, %.2lf)\n",
//			vector.x, vector.y, vector.z);

	fprintf(file, "facets :    ");
	for (i = 0; i < nf; ++i)
		fprintf(file, "%d ", index[i]);

	fprintf(file, "\npositions : ");
	for (i = nf + 1; i < 2 * nf + 1; ++i)
		fprintf(file, "%d ", index[i]);

	if(poly == NULL)
		fprintf(file, "Warning! poly == NULL !\n");
}
