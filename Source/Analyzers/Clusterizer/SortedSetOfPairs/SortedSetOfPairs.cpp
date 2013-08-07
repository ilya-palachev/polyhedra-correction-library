/*
 * SortedSetOfPairs.cpp
 *
 *  Created on: 21.06.2013
 *      Author: ilya
 */

#include "PolyhedraCorrectionLibrary.h"

SortedSetOfPairs::SortedSetOfPairs(int Len)
{
	DEBUG_START;
	len = Len;
	num = 0;
	ii = new int[Len];
	jj = new int[Len];
	DEBUG_END;
}

SortedSetOfPairs::~SortedSetOfPairs()
{
	DEBUG_START;
	if (ii != NULL)
		delete[] ii;
	if (jj != NULL)
		delete[] jj;
	ii = NULL;
	jj = NULL;
	DEBUG_END;
}

void SortedSetOfPairs::add(int i, int j)
{
	DEBUG_START;
	if (num == len)
	{
		DEBUG_PRINT("Error. OVERFLOW in SortedSetOfPairs\n");
		DEBUG_END;
		return;
	}

	int first = 0; // Первый элемент в массиве
	int last = num; // Последний элемент в массиве

	while (first < last)
	{
		int mid = (first + last) / 2;
		if (i < ii[mid] || (i == ii[mid] && j < jj[mid]))
		{
			last = mid;
		}
		else
		{
			first = mid + 1;
		}
	}

//    if (ii[last] == i && jj[last] == j)
//        return;
//    if (ii[last + 1] == i && jj[last + 1] == j)
//        return;
	if (last > 0 && ii[last - 1] == i && jj[last - 1] == j)
	{
		DEBUG_END;
		return;
	}
	insert_int(ii, num, last, i);
	insert_int(jj, num, last, j);
	++num;
	DEBUG_END;
}

void SortedSetOfPairs::print()
{
	DEBUG_START;
	int i;
	for (i = 0; i < num; ++i)
	{
		DEBUG_PRINT("ssop[%d] = \t(%d, \t%d)\n", i, ii[i], jj[i]);
	}
	DEBUG_END;
}



