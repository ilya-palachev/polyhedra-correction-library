/*
 * Copyright (c) 2009-2014 Ilya Palachev <iliyapalachev@gmail.com>
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

#include "Gauss_string.h"
#include "DebugPrint.h"

int Gauss_find_main_element(int n, double* A, int coloumn);
void Gauss_I_swap(int n, double* A, double* b, int p, int q);
void Gauss_II_norm(int n, double* A, double* b, int p);
void Gauss_III_sub(int n, double* A, double* b, int p);

int Gauss_string(int n, double* A, double* b)
{

	int step, i, imain;

	//1. Прямой ход метода Гаусса
	for (step = 0; step < n; ++step)
	{

		//1.1. Поиск главного элемента по столбцу
		imain = Gauss_find_main_element(n, A, step);
		//1.2. Если все элементы нулевые, то решений нет :
		if (imain == -1)
		{
			printf(
					"Gauss_string : Error. At step %d cannot find main elemnt. det = 0\n",
					step);
			return 0;
		}
		//1.3. Переставляем строки
		Gauss_I_swap(n, A, b, step, imain);
		//1.4. Нормируем главную строку
		Gauss_II_norm(n, A, b, step);
		//1.5. Вычитаем ее из всех низлежащих строк со соответствующим множителем
		Gauss_III_sub(n, A, b, step);
	}
	//2. Обратный ход метода Гаусса
	for (step = n - 1; step >= 0; --step)
	{
		for (i = 0; i < step; ++i)
		{
			b[i] -= A[i * n + step] * b[step];
		}
	}
	return 1;
}

int Gauss_find_main_element(int n, double* A, int coloumn)
{
	int i, imain;
	double norm, tmp;
	imain = -1;
	norm = 0.;
	for (i = coloumn; i < n; ++i)
	{
		tmp = fabs(A[i * n + coloumn]);
		if (tmp > norm)
		{
			imain = i;
			norm = tmp;
		}
	}
	DEBUG_PRINT("Pivot = %le in column %d", norm, coloumn);
	return imain;
}

void Gauss_I_swap(int n, double* A, double* b, int p, int q)
{
	int i;
	double tmp;
	for (i = 0; i < n; ++i)
	{
		tmp = A[p * n + i];
		A[p * n + i] = A[q * n + i];
		A[q * n + i] = tmp;
	}
	tmp = b[p];
	b[p] = b[q];
	b[q] = tmp;
}

void Gauss_II_norm(int n, double* A, double* b, int p)
{
	int i;
	double norm = 1. / A[p * n + p];
	A[p * n + p] = 1.;
	for (i = p + 1; i < n; ++i)
	{
		A[p * n + i] *= norm;
	}
	b[p] *= norm;
}

void Gauss_III_sub(int n, double* A, double* b, int p)
{
	int i, j;
	double norm;
	for (i = p + 1; i < n; ++i)
	{
		norm = A[i * n + p];
		A[i * n + p] = 0.;
		for (j = p + 1; j < n; ++j)
		{
			A[i * n + j] -= norm * A[p * n + j];
		}
		b[i] -= norm * b[p];
	}
}
