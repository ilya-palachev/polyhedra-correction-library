#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Polyhedron.h"

void EdgeList::my_fprint(FILE* file) {
	int i;
	fprintf(file, 
			"\n\n---------- Printing EdgeList for facet[%d]. ----------\n",
			facet->get_id());
	fprintf(file, "num = %d\n", num);
	if (num > 0) {
		fprintf(file, "edge0 : ");
		for (i = 0; i < num; ++i) {
			fprintf(file, "%d ", this->edge0[i]);
		}
		fprintf(file, "\n");
		fprintf(file, "edge1 : ");
		for (i = 0; i < num; ++i) {
			fprintf(file, "%d ", this->edge1[i]);
		}
		fprintf(file, "\n");
		fprintf(file, "next_facet : ");
		for (i = 0; i < num; ++i) {
			fprintf(file, "%d ", this->next_facet[i]);
		}
		fprintf(file, "\n");
		fprintf(file, "next_direction : ");
		for (i = 0; i < num; ++i) {
			fprintf(file, "%d ", this->next_direction[i]);
		}
		fprintf(file, "\n");
		fprintf(file, "scalar_mult : ");
		for (i = 0; i < num; ++i) {
			fprintf(file, "%lf ", this->scalar_mult[i]);
		}
		fprintf(file, "\n");
	}
}
