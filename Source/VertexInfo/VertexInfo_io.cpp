#include "PolyhedraCorrectionLibrary.h"

void VertexInfo::fprint_my_format(
		FILE* file) {
	fprintf(file, "%d ", numFacets);
	for (int j = 0; j < 3 * numFacets + 1; ++j)
		fprintf(file, " %d", indFacets[j]);
	fprintf(file, "\n");
}

void VertexInfo::my_fprint_all(
		FILE* file) {
	int i;
	fprintf(file, "\n------------ VertexInfo %d: ------------\n", id);
//	fprintf(file, "id = %d\nnf = %d\n", id, nf);
//	fprintf(file, "vector = (%.2lf, %.2lf, %.2lf)\n",
//			vector.x, vector.y, vector.z);

	fprintf(file, "facets :          ");
	for (i = 0; i < numFacets; ++i)
		fprintf(file, "%d ", indFacets[i]);

	fprintf(file, "\nnext vertexes : ");
	for (i = numFacets + 1; i < 2 * numFacets + 1; ++i)
		fprintf(file, "%d ", indFacets[i]);

	fprintf(file, "\npositions :     ");
	for (i = 2 * numFacets + 1; i < 3 * numFacets + 1; ++i)
		fprintf(file, "%d ", indFacets[i]);

	if (parentPolyhedron == NULL)
		fprintf(file, "Warning! poly == NULL !\n");
}
