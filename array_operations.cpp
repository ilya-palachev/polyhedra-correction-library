#include <stdio.h>

int min_int(int a, int b) {
	return a > b ? b : a;
}

int max_int(int a, int b) {
	return a > b ? a : b;
}

void swap_int(int& a, int& b) {
	int tmp = a;
	a = b;
	b = tmp;
}

void cut_int(int* array, int len, int pos) {
	int i;
	if (len < 1 || pos < 0 || pos > len - 1) {
		printf("cut_int: Error. pos = %d, but len = %d\n", pos, len);
		return;
	}
	for (i = pos; i < len; ++i)
		if (i < len - 1)
			array[i] = array[i + 1];
}

void insert_int(int* array, int len, int pos, int val) {
	int i;

	if (len < 0 || pos < 0 || pos > len) {
		printf("insert_int: Error. pos = %d, but len = %d\n", pos, len);
		return;
	}

	for (i = len; i > pos; --i)
		array[i] = array[i - 1];
	array[pos] = val;
}

void insert_double(double* array, int len, int pos, double val) {
	int i;
	if (len < 0 || pos < 0 || pos > len) {
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
