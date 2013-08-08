#include "PolyhedraCorrectionLibrary.h"

void SideOfContour::my_fprint(FILE* file)
{
	DEBUG_START;
	REGULAR_PRINT(file, "Printing content of side of contour\n");
	REGULAR_PRINT(file, "confidence = %lf (number from 0 to 1)\n", confidence);
	REGULAR_PRINT(file, "edge type = %d (that means the following:\n", type);
	REGULAR_PRINT(file,
			"// Unknown / not calculated\n"
					"EEdgeRegular     = 0 -- Regular edges, regular confidence\n"
					"EEdgeDummy       = 1 -- Dummy edge, not existing in reality\n"
					"                        (originating e.g. from a picture crop)\n"
					"EEdgeDust        = 2 -- Low confidence because of dust on stone surface\n"
					"EEdgeCavern      = 3 -- Low confidence because of cavern in the stone\n"
					"EEdgeMaxPointErr = 4 -- The edge erroneously is lower than other edges,\n"
					"                        which define a vertex (e.g. in the culet)\n"
					"EEdgeGlare       = 5 -- Low confidence because of photo glare\n\n");

	REGULAR_PRINT(file, "A1 = (%lf, %lf, %lf)\n", A1.x, A1.y, A1.z);
	REGULAR_PRINT(file, "A2 = (%lf, %lf, %lf)\n", A2.x, A2.y, A2.z);
	DEBUG_END;
}

void SideOfContour::my_fprint_short(FILE* file)
{
	DEBUG_START;
	REGULAR_PRINT(file, "%lf\t%d\t(%lf,%lf,%lf)\t(%lf,%lf,%lf)\n", confidence, type,
			A1.x, A1.y, A1.z, A2.x, A2.y, A2.z);
	DEBUG_END;
}
