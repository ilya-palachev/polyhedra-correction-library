/*
 * Copyright (c) 2009-2016 Ilya Palachev <iliyapalachev@gmail.com>
 *
 * This file is part of Polyhedra Correction Library.
 *
 * Polyhedra Correction Library is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Polyhedra Correction Library is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Polyhedra Correction Library.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "Correctors/Intersector/EdgeList/EdgeList.h"

void EdgeList::my_fprint(FILE *file)
{
	DEBUG_START;
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
	DEBUG_END;
}
