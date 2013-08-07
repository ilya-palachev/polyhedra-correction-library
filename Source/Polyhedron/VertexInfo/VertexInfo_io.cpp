#include "PolyhedraCorrectionLibrary.h"

void VertexInfo::fprint_my_format(FILE* file)
{
	REGULAR_PRINT(file, "%d ", numFacets);
	for (int j = 0; j < 3 * numFacets + 1; ++j)
		REGULAR_PRINT(file, " %d", indFacets[j]);
	REGULAR_PRINT(file, "\n");
}

void VertexInfo::my_fprint_all(FILE* file)
{
	int i;
	REGULAR_PRINT(file, "\n------------ VertexInfo %d: ------------\n", id);
//	REGULAR_PRINT(file, "id = %d\nnf = %d\n", id, nf);
//	REGULAR_PRINT(file, "vector = (%.2lf, %.2lf, %.2lf)\n",
//			vector.x, vector.y, vector.z);

	REGULAR_PRINT(file, "facets :          ");
	for (i = 0; i < numFacets; ++i)
		REGULAR_PRINT(file, "%d ", indFacets[i]);

	REGULAR_PRINT(file, "\nnext vertexes : ");
	for (i = numFacets + 1; i < 2 * numFacets + 1; ++i)
		REGULAR_PRINT(file, "%d ", indFacets[i]);

	REGULAR_PRINT(file, "\npositions :     ");
	for (i = 2 * numFacets + 1; i < 3 * numFacets + 1; ++i)
		REGULAR_PRINT(file, "%d ", indFacets[i]);

	if (parentPolyhedron == NULL)
		REGULAR_PRINT(file, "Warning! poly == NULL !\n");
}
