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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Gauss_test.h"
#include "matrix_invert.h"

void print_vector2(int n, double* v)
{
	int i;
	for (i = 0; i < n; ++i)
		printf("\t%lf\n", v[i]);
}

void mult_matrix_vector(int n, double* A, double* x, double* y);

void Gauss_test(int n)
{
	double *x, *b, *b0, *A, err1, err2;

	printf("Gauss_test:\n");
	printf("\tn = %d\n", n);

	x = new double[n];
	b = new double[n];
	b0 = new double[n];
	A = new double[n * n];

	create_A(n, A);
	create_x(n, x);
	mult_matrix_vector(n, A, x, b);
	mult_matrix_vector(n, A, x, b0);
	Gauss_string(n, A, b);

	subb(x, b, n);
	err1 = normb(x, n);

//    print_vector2(n, b);
	printf("\t||x - {1, 0, 1, ...}|| = %le\n", err1);

	create_A(n, A);
	mult_matrix_vector(n, A, b, x);
//    printf("x:\n");
//    print_vector2(n, x);
//    printf("b0:\n");
//    print_vector2(n, b0);

	subb(x, b0, n);
	err2 = normb(x, n);

//    printf("x - b0:\n");
//    print_vector2(n, x);
	printf("\t||A * x - b||          = %le\n", err2);

	if (x != NULL)
		delete[] x;
	if (b != NULL)
		delete[] b;
	if (b0 != NULL)
		delete[] b0;
	if (A != NULL)
		delete[] A;

}

inline double f(int n, int i, int j)
{
//Variant 1
	if (i > j)
		return n - i;
	else
		return n - j;

//Variant 2
//	if(i>j)
//		return 2000.-i;
//	else
//		return 2000.-j;

//Variant 3
//	return fabs(i-j);

//Variant 4
//   return 1./(i+j+1)+i+j;

//Variant 5
//	return i*i+j*j;
}

void create_A(int n, double* A)
{
	int i, j;
	for (i = 0; i < n; ++i)
		for (j = 0; j < n; ++j)
			A[i * n + j] = f(n, i, j);
}

void create_x(int n, double* x)
{
	int i;
	for (i = 0; i < n; ++i)
	{
		if (i % 2 == 0)
			x[i] = 1.;
		else
			x[i] = 0.;
	}
}

void mult_matrix_vector(int n, double* A, double* x, double* y)
{
	int i, j;
	for (i = 0; i < n; ++i)
	{
		y[i] = 0.;
		for (j = 0; j < n; ++j)
			y[i] += A[i * n + j] * x[j];
	}
}
