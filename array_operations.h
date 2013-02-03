#ifndef ARRAY_OPERATIONS_H
#define ARRAY_OPERATIONS_H

int min_int(int a, int b);
int max_int(int a, int b);
void swap_int(int& a, int& b);

void cut_int(int* array, int len, int pos);
void insert_int(int* array, int len, int pos, int val);
void insert_double(double* array, int len, int pos, double val);
void insert_binary(int n, int* edge0, int* edge1, double* scalar_mult, int v0, int v1, double sm);

#endif // ARRAY_OPERATIONS_H
