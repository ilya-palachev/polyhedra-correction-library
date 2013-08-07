#include "PolyhedraCorrectionLibrary.h"

void FutureFacet::my_fprint(FILE* file)
{
	int i;
	REGULAR_PRINT(file, "\n\n---------- FutureFacet %d. ----------\n", id);
	if (nv > 0)
	{
		REGULAR_PRINT(file, "edge0 : ");
		for (i = 0; i < nv; ++i)
		{
			REGULAR_PRINT(file, "%d ", this->edge0[i]);
		}
		REGULAR_PRINT(file, "\n");
		REGULAR_PRINT(file, "edge1 : ");
		for (i = 0; i < nv; ++i)
		{
			REGULAR_PRINT(file, "%d ", this->edge1[i]);
		}
		REGULAR_PRINT(file, "\n");
	}
}
