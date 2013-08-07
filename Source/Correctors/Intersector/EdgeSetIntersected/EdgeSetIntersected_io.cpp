#include "PolyhedraCorrectionLibrary.h"

void EdgeSetIntersected::my_fprint(FILE* file)
{
#ifndef NDEBUG
	int i;
	fprintf(file, "---------- EdgeSetIntersected (%d). ----------\n", num);
	if (num > 0)
	{
		fprintf(file, "edge0 : ");
		for (i = 0; i < num; ++i)
			fprintf(file, "%d ", edge0[i]);
		fprintf(file, "\n");
		fprintf(file, "edge1 : ");
		for (i = 0; i < num; ++i)
			fprintf(file, "%d ", edge1[i]);
		fprintf(file, "\n");

	}
#endif
}

