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

#include "stdio.h"
#include "array_operations.h"

int min_int(int a, int b)
{
	return a > b ? b : a;
}

int max_int(int a, int b)
{
	return a > b ? a : b;
}

void swap_int(int& a, int& b)
{
	int tmp = a;
	a = b;
	b = tmp;
}

void cut_int(int* array, int len, int pos)
{
	int i;
	if (len < 1 || pos < 0 || pos > len - 1)
	{
		printf("cut_int: Error. pos = %d, but len = %d\n", pos, len);
		return;
	}
	for (i = pos; i < len; ++i)
		if (i < len - 1)
			array[i] = array[i + 1];
}

void insert_int(int* array, int len, int pos, int val)
{
	int i;

	if (len < 0 || pos < 0 || pos > len)
	{
		printf("insert_int: Error. pos = %d, but len = %d\n", pos, len);
		return;
	}
#ifdef DEBUG1
	fprintf(stdout,
			"insert_int(array, len = %d, pos = %d, val = %d)\n",
			len, pos, val);
#endif

	for (i = len; i > pos; --i)
		array[i] = array[i - 1];
	array[pos] = val;
}

void insert_bool(bool* array, int len, int pos, bool val)
{
	int i;

	if (len < 0 || pos < 0 || pos > len)
	{
		printf("insert_int: Error. pos = %d, but len = %d\n", pos, len);
		return;
	}
#ifdef DEBUG1
	fprintf(stdout,
			"insert_bool(array, len = %d, pos = %d, val = %d)\n",
			len, pos, val);
#endif

	for (i = len; i > pos; --i)
		array[i] = array[i - 1];
	array[pos] = val;
}

void insert_double(double* array, int len, int pos, double val)
{
	int i;
	if (len < 0 || pos < 0 || pos > len)
	{
		printf("insert_double: Error. pos = %d, but len = %d\n", pos, len);
		return;
	}
	for (i = len; i > pos; --i)
		array[i] = array[i - 1];
	array[pos] = val;
}

//void insert_binary(int n, int* edge0, int* edge1, double* scalar_mult, int v0, int v1, double sm) {
//	int first = 0; // Первый элемент в массиве
//	int last = n; // Последний элемент в массиве
//
//	while (first < last) {
//		int mid = (first + last) / 2;
//		if (sm <= scalar_mult[mid]) {
//			last = mid;
//		} else {
//			first = mid + 1;
//		}
//	}
//
//	insert_double(scalar_mult, n, last, sm);
//	insert_int(edge0, n, last, v0);
//	insert_int(edge1, n, last, v1);
//
//}
//
//
