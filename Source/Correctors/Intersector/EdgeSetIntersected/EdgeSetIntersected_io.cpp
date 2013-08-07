#include "PolyhedraCorrectionLibrary.h"

void EdgeSetIntersected::my_fprint(FILE* file)
{
	int i;
	REGULAR_PRINT(file, "---------- EdgeSetIntersected (%d). ----------\n", num);
	if (num > 0)
	{
		REGULAR_PRINT(file, "edge0 : ");
		for (i = 0; i < num; ++i)
			REGULAR_PRINT(file, "%d ", edge0[i]);
		REGULAR_PRINT(file, "\n");
		REGULAR_PRINT(file, "edge1 : ");
		for (i = 0; i < num; ++i)
			REGULAR_PRINT(file, "%d ", edge1[i]);
		REGULAR_PRINT(file, "\n");
	}
}

