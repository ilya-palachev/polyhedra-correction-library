/*
 * SortedDouble.cpp
 *
 *  Created on: 21.06.2013
 *      Author: ilya
 */

#include "PolyhedraCorrectionLibrary.h"

SortedDouble::SortedDouble(int Len) :
				len(Len),
				num(0)
{
	array = new double[len];
}

SortedDouble::~SortedDouble()
{
	if (array != NULL)
		delete[] array;
}

void SortedDouble::add(double x)
{

	int first, last, mid;

	if (num >= len)
	{
		printf("Error.OVERULL!!!\n");
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
}

double SortedDouble::calclulate()
{
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
	return s;
}
