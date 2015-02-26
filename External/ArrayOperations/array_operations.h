/*
 * Copyright (c) 2009-2015 Ilya Palachev <iliyapalachev@gmail.com>
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

#ifndef ARRAY_OPERATIONS_H
#define ARRAY_OPERATIONS_H

int min_int(int a, int b);
int max_int(int a, int b);
void swap_int(int& a, int& b);

void cut_int(int* array, int len, int pos);
void insert_int(int* array, int len, int pos, int val);
void insert_bool(bool* array, int len, int pos, bool val);
void insert_double(double* array, int len, int pos, double val);
//void insert_binary(int n, int* edge0, int* edge1, double* scalar_mult, int v0, int v1, double sm);

#endif // ARRAY_OPERATIONS_H
