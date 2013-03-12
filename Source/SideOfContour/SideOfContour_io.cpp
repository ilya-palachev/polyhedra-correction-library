#include "Polyhedron.h"

void SideOfContour::my_fprint(FILE* file)
{
#ifndef NDEBUG
    fprintf(file, "Printing content of side of contour\n");
    fprintf(file, "confidence = %lf (number from 0 to 1)\n", confidence);
    fprintf(file, "edge type = %d (that means the following:\n", type);
    fprintf(file, "// Unknown / not calculated\n"
    "EEdgeRegular     = 0 -- Regular edges, regular confidence\n"
    "EEdgeDummy       = 1 -- Dummy edge, not existing in reality\n" 
    "                        (originating e.g. from a picture crop)\n"
    "EEdgeDust        = 2 -- Low confidence because of dust on stone surface\n"
    "EEdgeCavern      = 3 -- Low confidence because of cavern in the stone\n"
    "EEdgeMaxPointErr = 4 -- The edge erroneously is lower than other edges,\n"
    "                        which define a vertex (e.g. in the culet)\n"
    "EEdgeGlare       = 5 -- Low confidence because of photo glare\n\n");
    
    fprintf(file, "A1 = (%lf, %lf, %lf)\n", A1.x, A1.y, A1.z);
    fprintf(file, "A2 = (%lf, %lf, %lf)\n", A2.x, A2.y, A2.z);
#endif
}

void SideOfContour::my_fprint_short(FILE* file)
{
#ifndef NDEBUG
    fprintf(file, "%lf\t%d\t(%lf,%lf,%lf)\t(%lf,%lf,%lf)\n",
            confidence, type, A1.x, A1.y, A1.z, A2.x, A2.y, A2.z);
#endif
}
