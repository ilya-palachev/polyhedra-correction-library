#include "PolyhedraCorrectionLibrary.h"

void EdgeSet::my_fprint(
		FILE* file) {
	int i;
	fprintf(file, "---------- EdgeSet (%d). ----------\n", num);
	if (num > 0) {
		fprintf(file, "edge0 : ");
		for (i = 0; i < num; ++i)
			fprintf(file, "%d ", edge0[i]);
		fprintf(file, "\n");
		fprintf(file, "edge1 : ");
		for (i = 0; i < num; ++i)
			fprintf(file, "%d ", edge1[i]);
		fprintf(file, "\n");

	}
}

