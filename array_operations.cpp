#include <stdio.h>


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
	if(len < 1 || pos < 0 || pos > len-1)
	{
		printf("cut_int: Error. pos = %d, but len = %d\n", pos, len);
		return;
	}
	for(i = pos; i < len; ++i)
		if(i < len - 1)
			array[i] = array[i + 1];
}

void insert_int(int* array, int len, int pos, int val)
{
	int i;

	if(len < 0 || pos < 0 || pos > len)
	{
		printf("insert_int: Error. pos = %d, but len = %d\n", pos, len);
		return;
	}

	//Added 2010 - 12 - 02
	//Динамическое выделение памяти
	//TODO: Что делать с утечкой памяти?

//	int size = sizeof(array) / sizeof(int);
//	if(size < len)
//	{
//		int* array0 = new int[len];
//		for(i = len; i > pos; --i)
//			array0[i] = array[i - 1];
//		array0[pos] = val;
//		for(i = pos - 1; i >=0; --i)
//			array0[i] = array[i];
//		array = array0;
//	}
	for(i = len; i > pos; --i)
		array[i] = array[i - 1];
	array[pos] = val;
}

void insert_double(double* array, int len, int pos, double val)
{
	int i;
	if(len < 0 || pos < 0 || pos > len)
	{
		printf("insert_double: Error. pos = %d, but len = %d\n", pos, len);
		return;
	}
	for(i = len; i > pos; --i)
		array[i] = array[i - 1];
	array[pos] = val;
}

void insert_binary(int n, int* edge_list, double* a, int edge0, int edge1, double x)
{
	int first = 0; // Первый элемент в массиве
	int last = n; // Последний элемент в массиве

	while(first < last)
	{
		int mid = ( first + last ) / 2;
		if(x <= a[mid])
		{
			last = mid;
		}
		else
		{
			first = mid + 1;
		}
	}

	insert_double(a, n, last, x);
	insert_int(edge_list, 2*n, 2*last, edge0);
	insert_int(edge_list, 2*n + 1, 2*last + 1, edge1);

}


