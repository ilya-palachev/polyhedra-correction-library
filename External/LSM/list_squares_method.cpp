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

#include <iostream>
#include <cmath>

#include "list_squares_method.h"

void runListSquaresMethod(int n, double * x, double * y, double * z, double& a, double& b,
		double& c, double& d)
{
	double Ex2 = 0, Ex = 0, Ey2 = 0, Ey = 0, Ez2 = 0, Ez = 0, Exy = 0, Exz = 0,
			Eyz = 0;
	double** matr;
	double** m_it;

	for (int i = 0; i < n; i++)
	{
		Ex += x[i];
		Ey += y[i];
		Ez += z[i];
		Ex2 += x[i] * x[i];
		Ey2 += y[i] * y[i];
		Ez2 += z[i] * z[i];
		Exy += x[i] * y[i];
		Exz += x[i] * z[i];
		Eyz += y[i] * z[i];
	}

	matr = new double*[3];
	for (int i = 0; i < 3; i++)
		matr[i] = new double[3];
	m_it = new double*[3];
	for (int i = 0; i < 3; i++)
		m_it[i] = new double[3];

	double *lam = new double[3];

	matr[0][0] = Ex2 - Ex * Ex / n;
	matr[1][1] = Ey2 - Ey * Ey / n;
	matr[2][2] = Ez2 - Ez * Ez / n;

	matr[0][1] = matr[1][0] = Exy - Ex * Ey / n;
	matr[0][2] = matr[2][0] = Exz - Ex * Ez / n;
	matr[1][2] = matr[2][1] = Eyz - Ey * Ez / n;

	jacobi(3, matr, lam, m_it);
	double D1 = -(Ex * m_it[0][0] + Ey * m_it[1][0] + Ez * m_it[2][0]) / n;
	double D2 = -(Ex * m_it[0][1] + Ey * m_it[1][1] + Ez * m_it[2][1]) / n;
	double D3 = -(Ex * m_it[0][2] + Ey * m_it[1][2] + Ez * m_it[2][2]) / n;

	double s1 = 0;
	double s2 = 0;
	double s3 = 0;
	double sw;
	for (int i = 0; i < n; i++)
	{
		sw = (x[i] * m_it[0][0] + y[i] * m_it[1][0] + z[i] * m_it[2][0] + D1);
		s1 += sw * sw;
	}
	for (int i = 0; i < n; i++)
	{
		sw = (x[i] * m_it[0][1] + y[i] * m_it[1][1] + z[i] * m_it[2][1] + D2);
		s2 += sw * sw;
	}
	for (int i = 0; i < n; i++)
	{
		sw = (x[i] * m_it[0][2] + y[i] * m_it[1][2] + z[i] * m_it[2][2] + D3);
		s3 += sw * sw;
	}
	sw = s1;
	a = m_it[0][0];
	b = m_it[1][0];
	c = m_it[2][0];
	d = D1;

	if (s2 < sw)
	{
		sw = s2;
		a = m_it[0][1];
		b = m_it[1][1];
		c = m_it[2][1];
		d = D2;
	}
	if (s3 < sw)
	{
		a = m_it[0][2];
		b = m_it[1][2];
		c = m_it[2][2];
		d = D3;
	}
	for (int i = 0; i < 2; i++)
		delete[] matr[i];
	delete[] matr;

	for (int i = 0; i < 2; i++)
		delete[] m_it[i];
	delete[] m_it;

	delete[] lam;
}

void jacobi(int n, double ** a, double * d, double ** v)
{

	if (n == 0)
		return;
	double * b = new double[n + n];
	double * z = b + n;
	int i, j;

	for (i = 0; i < n; ++i)
	{
		z[i] = 0.;
		b[i] = d[i] = a[i][i];
		for (j = 0; j < n; ++j)
		{
			v[i][j] = i == j ? 1. : 0.;
		}
	}
	for (i = 0; i < 50; ++i)
	{
		double sm = 0.;
		int p, q;
		for (p = 0; p < n - 1; ++p)
		{
			for (q = p + 1; q < n; ++q)
			{
				sm += fabs(a[p][q]);
			}
		}
		if (fabs(sm) < 1e-20)
			break;
		const double tresh = i < 3 ? 0.2 * sm / (n * n) : 0.;
		for (p = 0; p < n - 1; ++p)
		{
			for (q = p + 1; q < n; ++q)
			{
				const double g = 1e12 * fabs(a[p][q]);
				if (i >= 3 && fabs(d[p]) > g && fabs(d[q]) > g)
					a[p][q] = 0.;
				else if (fabs(a[p][q]) > tresh)
				{
					const double theta = 0.5 * (d[q] - d[p]) / a[p][q];
					double t = 1. / (fabs(theta) + sqrt(1. + theta * theta));
					if (theta < 0)
						t = -t;
					const double c = 1. / sqrt(1. + t * t);
					const double s = t * c;
					const double tau = s / (1. + c);
					const double h = t * a[p][q];
					z[p] -= h;
					z[q] += h;
					d[p] -= h;
					d[q] += h;
					a[p][q] = 0.;
					for (j = 0; j < p; ++j)
					{
						const double g = a[j][p];
						const double h = a[j][q];
						a[j][p] = g - s * (h + g * tau);
						a[j][q] = h + s * (g - h * tau);
					}
					for (j = p + 1; j < q; ++j)
					{
						const double g = a[p][j];
						const double h = a[j][q];
						a[p][j] = g - s * (h + g * tau);
						a[j][q] = h + s * (g - h * tau);
					}
					for (j = q + 1; j < n; ++j)
					{
						const double g = a[p][j];
						const double h = a[q][j];
						a[p][j] = g - s * (h + g * tau);
						a[q][j] = h + s * (g - h * tau);
					}
					for (j = 0; j < n; ++j)
					{
						const double g = v[j][p];
						const double h = v[j][q];
						v[j][p] = g - s * (h + g * tau);
						v[j][q] = h + s * (g - h * tau);
					}
				}
			}
		}
		for (p = 0; p < n; ++p)
		{
			d[p] = (b[p] += z[p]);
			z[p] = 0.;
		}
	}
	delete[] b;
}

