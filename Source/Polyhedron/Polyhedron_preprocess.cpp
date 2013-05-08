#include "PolyhedraCorrectionLibrary.h"

void Polyhedron::preprocess_polyhedron()
{
//    printf("\tНачинаем предобработку ...\n");
	int i;
//    printf("\t\tЧистка граней: ");
	for (i = 0; i < numFacets; ++i)
	{
		if (facets[i].numVertices < 1)
			continue;
//        printf("%d, ", i);
//        printf("Preprocess_free facet %d\n", i);
		facets[i].preprocess_free();
	}
//    printf("\n\t\tПредобработка граней: ");
	for (i = 0; i < numFacets; ++i)
	{
		if (facets[i].numVertices < 1)
			continue;
//        printf("%d ", i);
//        printf("Preprocess facet %d\n", i);
		facets[i].preprocess();
//        if (facet[i].find_total(-1) != -1)
//            facet[i].my_fprint_all(stdout);
	}
	//	if(vertexinfo != NULL)
	//		delete[] vertexinfo;
//    printf("\n\t\tПредобработка вершин: ");
	vertexInfos = new VertexInfo[numVertices];
	for (i = 0; i < numVertices; ++i)
	{
//        printf("%d ", i);
//        printf("Preprocess vertexinfo %d\n", i);
		vertexInfos[i] = VertexInfo(i, vertices[i], &(*this));
//        printf("\tallocated\n");
		vertexInfos[i].preprocess();
	}
//    printf("\n");
}
