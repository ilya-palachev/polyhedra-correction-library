#include "PolyhedraCorrectionLibrary.h"

void EdgeList::my_fprint(FILE* file)
{
	int i;
	REGULAR_PRINT(file, "\n\n---------- EdgeList %d. ----------\n", id);
	REGULAR_PRINT(file, "num = %d\n", num);
	if (num > 0)
	{
		REGULAR_PRINT(file, "edge0 : ");
		for (i = 0; i < num; ++i)
		{
			REGULAR_PRINT(file, "%d ", this->edge0[i]);
		}
		REGULAR_PRINT(file, "\n");
		REGULAR_PRINT(file, "edge1 : ");
		for (i = 0; i < num; ++i)
		{
			REGULAR_PRINT(file, "%d ", this->edge1[i]);
		}
		REGULAR_PRINT(file, "\n");
		REGULAR_PRINT(file, "next_facet : ");
		for (i = 0; i < num; ++i)
		{
			REGULAR_PRINT(file, "%d ", this->next_facet[i]);
		}
		REGULAR_PRINT(file, "\n");
		REGULAR_PRINT(file, "next_direction : ");
		for (i = 0; i < num; ++i)
		{
			REGULAR_PRINT(file, "%d ", this->next_direction[i]);
		}
		REGULAR_PRINT(file, "\n");
		REGULAR_PRINT(file, "scalar_mult : ");
		for (i = 0; i < num; ++i)
		{
			REGULAR_PRINT(file, "%lf ", this->scalar_mult[i]);
		}
		REGULAR_PRINT(file, "\n");
		REGULAR_PRINT(file, "id_v_new : ");
		for (i = 0; i < num; ++i)
		{
			REGULAR_PRINT(file, "%d ", this->id_v_new[i]);
		}
		REGULAR_PRINT(file, "\n");
		REGULAR_PRINT(file, "isUsed : ");
		for (i = 0; i < num; ++i)
		{
			REGULAR_PRINT(file, "%d ", this->isUsed[i]);
		}
		REGULAR_PRINT(file, "\n");
	}
}
