/* 
 * Copyright (c) 2009-2013 Ilya Palachev <iliyapalachev@gmail.com>
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

#include "PolyhedraCorrectionLibrary.h"

void EdgeSetIntersected::my_fprint(FILE* file)
{
	DEBUG_START;
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
	DEBUG_END;
}

