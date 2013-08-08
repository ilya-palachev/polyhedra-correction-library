#include "PolyhedraCorrectionLibrary.h"

void SContour::my_fprint(FILE* file)
{
	DEBUG_START;
	REGULAR_PRINT(file, "Printing content of shade contour #%d\n", id);
	REGULAR_PRINT(file, "id = %d (id of the contour)\n", id);
	REGULAR_PRINT(file, "ns = %d (number of sides)\n", ns);
	REGULAR_PRINT(file, "plane = ((%lf) * x + (%lf) * y + (%lf) * z + (%lf) = 0\n",
			plane.norm.x, plane.norm.y, plane.norm.z, plane.dist);
	REGULAR_PRINT(file, "These are that sides:\n");
	REGULAR_PRINT(file, "confidence\t"
			"type\t"
			"vector A1\t"
			"vector A2\t\n");
	for (int i = 0; i < ns; ++i)
	{
		sides[i].my_fprint_short(file);
	}
	DEBUG_END;
}
