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

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "Gauss_string.h"
#include "Correctors/PointShifterWeighted/PointShifterWeighted.h"
#include "Polyhedron/Facet/Facet.h"
#include "Polyhedron/VertexInfo/VertexInfo.h"

double norm_vector(int n, double* a);

void print_matrix(FILE* file, int n, int m, double* A);

void print_matrix_bool(FILE* file, int n, int m, bool* A);


#define EPSILON 1e-11
#define EPS_DERIVATE 1e-8

PointShifterWeighted::PointShifterWeighted() :
		PCorrector(),
		x(),
		x1(),
		fx(),
		A(),
		khi(),
		tmp0(),
		tmp1(),
		tmp2(),
		tmp3()
{
	DEBUG_START;
	DEBUG_END;
}

PointShifterWeighted::PointShifterWeighted(PolyhedronPtr p) :
				PCorrector(p),
				x(),
				x1(),
				fx(),
				A(),
				khi(),
				tmp0(),
				tmp1(),
				tmp2(),
				tmp3()
{
	DEBUG_START;
	init();
	DEBUG_END;
}

PointShifterWeighted::PointShifterWeighted(Polyhedron* p) :
				PCorrector(p),
				x(),
				x1(),
				fx(),
				A(),
				khi(),
				tmp0(),
				tmp1(),
				tmp2(),
				tmp3()
{
	DEBUG_START;
	init();
	DEBUG_END;
}

void PointShifterWeighted::init()
{
	DEBUG_START;
	x = new double[n];
	tmp0 = new double[n];
	tmp1 = new double[n];
	tmp2 = new double[n];
	tmp3 = new double[n];
	x1 = new double[n];
	fx = new double[n];
	A = new double[n * n];
	khi = new bool[polyhedron->numFacets * polyhedron->numVertices];

	for (int i = 0; i < polyhedron->numVertices; ++i)
		for (int j = 0; j < polyhedron->numFacets; ++j)
			khi[i * polyhedron->numFacets + j] = (polyhedron->facets[j].find_vertex(i) != -1);
	DEBUG_END;
}

PointShifterWeighted::~PointShifterWeighted()
{
	DEBUG_START;
	if (x != NULL)
	{
		delete[] x;
		x = NULL;
	}
	if (tmp0 != NULL)
	{
		delete[] tmp0;
		tmp0 = NULL;
	}
	if (tmp1 != NULL)
	{
		delete[] tmp1;
		tmp1 = NULL;
	}
	if (tmp2 != NULL)
	{
		delete[] tmp2;
		tmp2 = NULL;
	}
	if (tmp3 != NULL)
	{
		delete[] tmp3;
		tmp3 = NULL;
	}
	if (x1 != NULL)
	{
		delete[] x1;
		x1 = NULL;
	}
	if (fx != NULL)
	{
		delete[] fx;
		fx = NULL;
	}
	if (A != NULL)
	{
		delete[] A;
		A = NULL;
	}
	if (khi != NULL)
	{
		delete[] khi;
		khi = NULL;
	}
	DEBUG_END;
}

#define nx(i) (3 * ((i) - ((i) > id)))
#define ny(i) (3 * ((i) - ((i) > id)) + 1)
#define nz(i) (3 * ((i) - ((i) > id)) + 2)
#define na(j) (3 * (polyhedron->numVertices - 1) + 4 * (j))
#define nb(j) (3 * (polyhedron->numVertices - 1) + 4 * (j) + 1)
#define nc(j) (3 * (polyhedron->numVertices - 1) + 4 * (j) + 2)
#define nd(j) (3 * (polyhedron->numVertices - 1) + 4 * (j) + 3)

#define x(i) x[nx(i)]
#define y(i) x[ny(i)]
#define z(i) x[nz(i)]
#define a(i) x[na(i)]
#define b(i) x[nb(i)]
#define c(i) x[nc(i)]
#define d(i) x[nd(i)]

#define khi(i, j) khi[(i) * polyhedron->numVertices + (j)]

void PointShifterWeighted::run(int id, Vector3d delta)
{
	DEBUG_START;
	DEBUG_VARIABLE int step;
	double err, err_new, gamma;
	bool success;

	int auto_step;

	DEBUG_VARIABLE double xx, yy, zz;
	DEBUG_VARIABLE double a, b, c, d;
	int nv, *index;
	for (int j = 0; j < polyhedron->numFacets; ++j)
	{
		a = polyhedron->facets[j].plane.norm.x;
		b = polyhedron->facets[j].plane.norm.y;
		c = polyhedron->facets[j].plane.norm.z;
		d = polyhedron->facets[j].plane.dist;
		index = polyhedron->facets[j].indVertices;
		nv = polyhedron->facets[j].numVertices;
		for (int i = 0; i < nv; ++i)
		{
			xx = polyhedron->vertices[index[i]].x;
			yy = polyhedron->vertices[index[i]].y;
			zz = polyhedron->vertices[index[i]].z;
			DEBUG_PRINT("v%d -> f%d : %lf\n", index[i], j,
					a * xx + b * yy + c * zz + d);
		}
	}

	polyhedron->vertices[id] = polyhedron->vertices[id] + delta;
	n = 4 * polyhedron->numFacets + 3 * (polyhedron->numVertices - 1);


	print_matrix_bool(stdout, polyhedron->numVertices, polyhedron->numFacets, khi);

	for (int i = 0; i < n; ++i)
	{
		x[i] = 0.;
	}
	K = 1;
	calculateFunctional();
	err = norm_vector(n, fx);

	step = 0;
	while (err > EPSILON)
	{
		calculateFuncitonalDerivative();
		calculateFunctional();
		err = norm_vector(n, fx);

		DEBUG_PRINT("step %d\terr = %le\n", step++, err);
		if (err > 100)
		{
			DEBUG_PRINT("Ошибка. Метод Ньютона разошелся\n");
			break;
		}
		print_matrix(stdout, n, n, A);

		success = Gauss_string(n, A, fx);
		if (!success)
		{
			DEBUG_PRINT("Stoping deform...\n");
			DEBUG_END;
			return;
		}
		for (int i = 0; i < n; ++i)
		{
			x1[i] = fx[i];
		}
		gamma = 2.;
		auto_step = 0;
		DEBUG_PRINT("\tauto_step : ");
		do
		{
			if (auto_step > 0)
				break;

			gamma *= 0.5;
			++auto_step;
			DEBUG_PRINT("%d ", auto_step);

			for (int i = 0; i < n; ++i)
				x[i] += gamma * x1[i];
			calculateFunctional();
			err_new = norm_vector(n, fx);
		} while (err_new > err);
		DEBUG_PRINT("\n");
	}

	for (int i = 0; i < polyhedron->numVertices; ++i)
	{
		if (i == id)
			continue;
		polyhedron->vertices[i].x += x(i);
		polyhedron->vertices[i].y += y(i);
		polyhedron->vertices[i].z += z(i);
	}
	for (int i = 0; i < polyhedron->numFacets; ++i)
	{
		polyhedron->facets[i].plane.norm.x += a(i);
		polyhedron->facets[i].plane.norm.y += b(i);
		polyhedron->facets[i].plane.norm.z += c(i);
		polyhedron->facets[i].plane.dist += d(i);
	}

	for (int j = 0; j < polyhedron->numFacets; ++j)
	{
		a = polyhedron->facets[j].plane.norm.x;
		b = polyhedron->facets[j].plane.norm.y;
		c = polyhedron->facets[j].plane.norm.z;
		d = polyhedron->facets[j].plane.dist;
		index = polyhedron->facets[j].indVertices;
		nv = polyhedron->facets[j].numVertices;
		for (int i = 0; i < nv; ++i)
		{
			xx = polyhedron->vertices[index[i]].x;
			yy = polyhedron->vertices[index[i]].y;
			zz = polyhedron->vertices[index[i]].z;
			DEBUG_PRINT("v%d -> f%d : %lf\n", index[i], j,
					a * xx + b * yy + c * zz + d);
		}
	}

	DEBUG_PRINT("End of deform_w\n");
	DEBUG_END;
}

void PointShifterWeighted::calculateFunctional()
{
	DEBUG_START;
	int i, j;
	double sx, sy, sz;
	double sa, sb, sc, sd;
	double aj, bj, cj, dj;
	double xi, yi, zi;
	double coeff;

	for (i = 0; i < polyhedron->numVertices; ++i)
	{
		if (i == id)
			continue;
		xi = polyhedron->vertices[i].x;
		yi = polyhedron->vertices[i].y;
		zi = polyhedron->vertices[i].z;
		sx = 0;
		sy = 0;
		sz = 0;
		for (j = 0; j < polyhedron->numFacets; ++j)
		{
			if (khi(i, j))
			{
				aj = polyhedron->facets[j].plane.norm.x;
				bj = polyhedron->facets[j].plane.norm.y;
				cj = polyhedron->facets[j].plane.norm.z;
				dj = polyhedron->facets[j].plane.dist;
				coeff = (aj + a(j)) * (xi + x(i))
				+ (bj + b(j)) * (yi + y(i))
				+ (cj + c(j)) * (zi + z(i))
				+ dj + d(j);
				sx += coeff * (aj + a(j));
				sy += coeff * (bj + b(j));
				sz += coeff * (cj + c(j));
			}
		}
		fx[nx(i)] = 2 * x(i)+ 2 * K * sx;
		fx[ny(i)] = 2 * y(i)+ 2 * K * sy;
		fx[nz(i)] = 2 * z(i)+ 2 * K * sz;
	}

	for (j = 0; j < polyhedron->numFacets; ++j)
	{
		aj = polyhedron->facets[j].plane.norm.x;
		bj = polyhedron->facets[j].plane.norm.y;
		cj = polyhedron->facets[j].plane.norm.z;
		dj = polyhedron->facets[j].plane.dist;
		sa = 0;
		sb = 0;
		sc = 0;
		sd = 0;
		for (i = 0; i < polyhedron->numVertices; ++i)
		{
			if (khi(i, j))
			{
				xi = polyhedron->vertices[i].x;
				yi = polyhedron->vertices[i].y;
				zi = polyhedron->vertices[i].z;
				if (i == id)
				{
					coeff = (aj + a(j)) * xi
					+ (bj + b(j)) * yi
					+ (cj + c(j)) * zi
					+ dj + d(j);
					sa += coeff * xi;
					sb += coeff * yi;
					sc += coeff * zi;
					sd += coeff;
				}
				else
				{
					coeff = (aj + a(j)) * (xi + x(i))
					+ (bj + b(j)) * (yi + y(i))
					+ (cj + c(j)) * (zi + z(i))
					+ dj + d(j);
					sa += coeff * (xi + x(i));
					sb += coeff * (yi + y(i));
					sc += coeff * (zi + z(i));
					sd += coeff;
				}
			}
		}
		fx[na(j)] = 2 * a(j)+ 2 * K * sa;
		fx[nb(j)] = 2 * b(j)+ 2 * K * sb;
		fx[nc(j)] = 2 * c(j)+ 2 * K * sc;
		fx[nd(j)] = 2 * d(j)+ 2 * K * sd;
	}
	DEBUG_END;
}

void PointShifterWeighted::calculateFuncitonalDerivative()
{
	DEBUG_START;
	int i, j;
	double ieps = 0.5 / EPS_DERIVATE;

	for (j = 0; j < n; ++j)
	{
		for (i = 0; i < n; ++i)
		{
			tmp0[i] = x[i];
			tmp1[i] = x[i];
		}
		tmp0[j] += EPS_DERIVATE;
		tmp1[j] -= EPS_DERIVATE;
		x = tmp0;
		fx = tmp2;
		calculateFunctional();
		x = tmp1;
		fx = tmp3;
		calculateFunctional();
		for (i = 0; i < n; ++i)
			A[i * n + j] = (tmp2[i] - tmp3[i]) * ieps;
	}
	DEBUG_END;
}

