#include "Polyhedron.h"

void FutureFacet::my_fprint(FILE* file) {
		int i;
	fprintf(file, "\n\n---------- FutureFacet %d. ----------\n", id);
	if (nv > 0) {
		fprintf(file, "edge0 : ");
		for (i = 0; i < nv; ++i) {
			fprintf(file, "%d ", this->edge0[i]);
		}
		fprintf(file, "\n");
		fprintf(file, "edge1 : ");
		for (i = 0; i < nv; ++i) {
			fprintf(file, "%d ", this->edge1[i]);
		}
		fprintf(file, "\n");
	}

}
