#include "PolyhedraCorrectionLibrary.h"

void SContour::my_fprint(FILE* file)
{
#ifndef NDEBUG
	fprintf(file, "Printing content of shade contour #%d\n", id);
	fprintf(file, "id = %d (id of the contour)\n", id);
	fprintf(file, "ns = %d (number of sides)\n", ns);
	fprintf(file, "plane = ((%lf) * x + (%lf) * y + (%lf) * z + (%lf) = 0\n",
			plane.norm.x, plane.norm.y, plane.norm.z, plane.dist);
	fprintf(file, "These are that sides:\n");
	fprintf(file, "confidence\t"
			"type\t"
			"vector A1\t"
			"vector A2\t\n");
	for (int i = 0; i < ns; ++i)
	{
		sides[i].my_fprint_short(file);
	}
#endif
}
