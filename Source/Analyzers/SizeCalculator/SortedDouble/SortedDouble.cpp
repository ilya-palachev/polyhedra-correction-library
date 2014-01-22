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

#include <cstdlib>
#include <cstdio>

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "array_operations.h"
#include "Analyzers/SizeCalculator/SortedDouble/SortedDouble.h"

SortedDouble::SortedDouble(int Len) :
				len(Len),
				num(0)
{
	DEBUG_START;
	array = new double[len];
	DEBUG_END;
}

SortedDouble::~SortedDouble()
{
	DEBUG_START;
	if (array != NULL)
		delete[] array;
	DEBUG_END;
}

void SortedDouble::add(double x)
{
	DEBUG_START;

	int first, last, mid;

	if (num >= len)
	{
		ERROR_PRINT("Error. Overflow!!!\n");
		return;
	}

	first = 0; // Первый элемент в массиве
	last = num; // Последний элемент в массиве

	while (first < last)
	{
		mid = (first + last) / 2;
		if (x <= array[mid])
		{
			last = mid;
		}
		else
		{
			first = mid + 1;
		}
	}
	insert_double(array, num, last, x);
	++num;

	DEBUG_END;
}

double SortedDouble::calclulate()
{
	DEBUG_START;

	int i;
	double s;

	s = 0;
	for (i = 0; i < num; ++i)
	{
		if (i % 2 == 0)
			s -= array[i];
		else
			s += array[i];
	}

	DEBUG_END;
	return s;
}
