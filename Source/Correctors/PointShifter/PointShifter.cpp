/*
 * PointShifter.cpp
 *
 *  Created on: 20.06.2013
 *      Author: ilya
 */

#include "PolyhedraCorrectionLibrary.h"

PointShifter::PointShifter() :
		PCorrector(),
		x(),
		x1(),
		fx(),
		A(),
		sum(),
		tmp0(),
		tmp1(),
		tmp2(),
		tmp3()
{
}

PointShifter::PointShifter(Polyhedron* p) :
		PCorrector(p),
		x(),
		x1(),
		fx(),
		A(),
		sum(),
		tmp0(),
		tmp1(),
		tmp2(),
		tmp3()
{
	n = 0;
	for (int i = 0; i < polyhedron->numFacets; ++i)
	{
		n += polyhedron->facets[i].numVertices;
	}
	n += 4 * polyhedron->numFacets + 3 * (polyhedron->numVertices - 1);

	x = new double[n];
	x1 = new double[n];
	fx = new double[n];
	A = new double[n * n];
	tmp0 = new double[n];
	tmp1 = new double[n];
	tmp2 = new double[n];
	tmp3 = new double[n];

	sum = new int[polyhedron->numFacets];
	sum[0] = 0;
	for (int i = 0; i < polyhedron->numFacets - 1; ++i)
	{
		sum[i + 1] = sum[i] + polyhedron->facets[i].numVertices;
	}

	for (int i = 0; i < polyhedron->numFacets; ++i)
	{
		DEBUG_PRINT("sum[%d] = %d", i, sum[i]);
	}
}

PointShifter::~PointShifter()
{
	if (x != NULL)
	{
		delete[] x;
		x = NULL;
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
	if (sum != NULL)
	{
		delete[] sum;
		sum = NULL;
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
}

#define eps 1.
#define EPSILON 1e-11
#define EPS_DERIVATE 1e-8

#define na(i) (4 * (i))
#define nb(i) (4 * (i) + 1)
#define nc(i) (4 * (i) + 2)
#define nd(i) (4 * (i) + 3)

#define nx(i) (4 * polyhedron->numFacets + 3 * (i - (i > id)))
#define ny(i) (4 * polyhedron->numFacets + 3 * (i - (i > id)) + 1)
#define nz(i) (4 * polyhedron->numFacets + 3 * (i - (i > id)) + 2)

#define nl(i) (4 * polyhedron->numFacets + 3  * (polyhedron->numVertices - 1) + (i))

#define a(i) x[na(i)]
#define b(i) x[nb(i)]
#define c(i) x[nc(i)]
#define d(i) x[nd(i)]
#define x(i) x[nx(i)]
#define y(i) x[ny(i)]
#define z(i) x[nz(i)]
#define l(i) x[nl(i)]

double norm_vector(int n, double* a);

void print_matrix(FILE* file, int n, int m, double* A);

void print_matrix_bool(FILE* file, int n, int m, bool* A);

void print_vector(int n, double* v);

void PointShifter::run(int id, Vector3d delta)
{
	int i, step;
	double err, err_new, gamma;
	bool success;

	int auto_step;

	double xx, yy, zz;
	double a, b, c, d;
	int nv, *index;
	for (int j = 0; j < polyhedron->numFacets; ++j)
	{
		a = polyhedron->facets[j].plane.norm.x;
		b = polyhedron->facets[j].plane.norm.y;
		c = polyhedron->facets[j].plane.norm.z;
		d = polyhedron->facets[j].plane.dist;
		index = polyhedron->facets[j].indVertices;
		nv = polyhedron->facets[j].numVertices;
		for (i = 0; i < nv; ++i)
		{
			xx = polyhedron->vertices[index[i]].x;
			yy = polyhedron->vertices[index[i]].y;
			zz = polyhedron->vertices[index[i]].z;
			printf("v%d -> f%d : %lf\n", index[i], j,
					a * xx + b * yy + c * zz + d);
		}
	}

	polyhedron->vertices[id] = polyhedron->vertices[id] + delta;

	for (i = 0; i < n; ++i)
	{
		x[i] = 0.;
	}
	calculateFunctional();
	err = norm_vector(n, fx);

	step = 0;
	while (err > EPSILON)
	{
		calculateFunctionalDerivative2();
		calculateFunctional();
		err = norm_vector(n, fx);

		printf("step %d\terr = %le\n", step++, err);
		print_vector(n, fx);
		if (err > 100)
		{
			printf("Ошибка. Метод Ньютона разошелся\n");
			break;
		}
		print_matrix(stdout, n, n, A);

		success = Gauss_string(n, A, fx);
		if (!success)
		{
			printf("Stoping deform...\n");
			return;
		}
		for (i = 0; i < n; ++i)
		{
			x1[i] = fx[i];
		}
		gamma = 2.;
		auto_step = 0;
		printf("\tauto_step : ");
		do
		{
			if (auto_step > 100)
				break;

			gamma *= 0.5;
			++auto_step;
			printf("%d ", auto_step);

			for (i = 0; i < n; ++i)
				x[i] += gamma * x1[i];
			calculateFunctional();
			err_new = norm_vector(n, fx);
		} while (err_new > err);
		printf("\n");
	}

	for (i = 0; i < polyhedron->numVertices; ++i)
	{
		if (i == id)
			continue;
		polyhedron->vertices[i].x += x(i);
		polyhedron->vertices[i].y += y(i);
		polyhedron->vertices[i].z += z(i);
	}
	for (i = 0; i < polyhedron->numFacets; ++i)
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
		for (i = 0; i < nv; ++i)
		{
			xx = polyhedron->vertices[index[i]].x;
			yy = polyhedron->vertices[index[i]].y;
			zz = polyhedron->vertices[index[i]].z;
			printf("v%d -> f%d : %lf\n", index[i], j,
					a * xx + b * yy + c * zz + d);
		}
	}
}

double norm_vector(int n, double* a)
{
	int i;
	double max, tmp;
	max = 0.;
	for (i = 0; i < n; ++i)
	{
		tmp = fabs(a[i]);
		if (tmp > max)
			max = tmp;
	}
	return max;
}

void print_matrix(FILE* file, int n, int m, double* A)
{
	int i, j;

	fprintf(file, "\n");
	for (i = 0; i < m; ++i)
		fprintf(file, "--");
	fprintf(file, "\n");

	for (i = 0; i < n; ++i)
	{
		for (j = 0; j < m; ++j)
		{
			if (fabs(A[i * n + j]) >= 1e-16)
			{
				if (fabs(A[i * n + j]) < 9.5)
					fprintf(file, "%1.0lf ", fabs(A[i * n + j]));
				else
					fprintf(file, "# ");
			}
			else
				fprintf(file, "  ");
		}
		fprintf(file, "|\n");
	}
	for (i = 0; i < m; ++i)
		fprintf(file, "--");
	fprintf(file, "\n");
}

void print_matrix_bool(FILE* file, int n, int m, bool* A)
{
	int i, j;

	fprintf(file, "\n");
	for (i = 0; i < m; ++i)
		fprintf(file, "--");
	fprintf(file, "\n");

	for (i = 0; i < n; ++i)
	{
		for (j = 0; j < m; ++j)
		{
			if (A[i * m + j])
				fprintf(file, "1 ");
			else
				fprintf(file, "  ");
		}
		fprintf(file, "|\n");
	}
	for (i = 0; i < m; ++i)
		fprintf(file, "--");
	fprintf(file, "\n");
}

void print_vector(int n, double* v)
{
	int i;
	for (i = 0; i < n; ++i)
		printf("\t%lf\n", v[i]);
}


void PointShifter::calculateFunctional()
{
	int i, j, k, p, m;
	int *index, nv, nf;
	Plane plane;

	k = 0;

	for (j = 0; j < polyhedron->numFacets; ++j)
	{
		nv = polyhedron->facets[j].numVertices;
		index = polyhedron->facets[j].indVertices;
		plane = polyhedron->facets[j].plane;
		for (i = 0; i < nv; ++i)
		{
			if (index[i] == id)
			{
				fx[k] = (polyhedron->vertices[index[i]].x) * (plane.norm.x + a(j));
				fx[k] += (polyhedron->vertices[index[i]].y) * (plane.norm.y + b(j));
				fx[k] += (polyhedron->vertices[index[i]].z) * (plane.norm.z + c(j));
				fx[k] += plane.dist + d(j);
					}
					else
					{
						fx[k] = (polyhedron->vertices[index[i]].x + x(index[i])) * (plane.norm.x + a(j));
                fx[k] += (polyhedron->vertices[index[i]].y + y(index[i])) * (plane.norm.y + b(j));
                fx[k] += (polyhedron->vertices[index[i]].z + z(index[i])) * (plane.norm.z + c(j));
                fx[k] += plane.dist + d(j);
            }
				++k;
			}
		}

	for (i = 0; i < polyhedron->numVertices; ++i)
	{
		if (i == id)
		{
			continue;
		}
		fx[k] = 2 * eps * x(i);
		fx[k + 1] = 2 * eps * y(i);
		fx[k + 2] = 2 * eps * z(i);

		nf = polyhedron->vertexInfos[i].numFacets;
		;
		index = polyhedron->vertexInfos[i].indFacets;
		for (j = 0; j < nf; ++j)
		{
			p = index[j];
			m = polyhedron->facets[p].find_vertex(i);
			fx[k] -= l(m + sum[p])* (polyhedron->facets[p].plane.norm.x + a(p));
			fx[k + 1] -= l(m + sum[p])* (polyhedron->facets[p].plane.norm.y + b(p));
			fx[k + 2] -= l(m + sum[p])* (polyhedron->facets[p].plane.norm.z + c(p));
		}
		k += 3;
	}

	for (j = 0; j < polyhedron->numFacets; ++j)
	{
		fx[k] = 2 * a(j);
		fx[k + 1] = 2 * b(j);
		fx[k + 2] = 2 * c(j);
		fx[k + 3] = 2 * d(j);

		nv = polyhedron->facets[j].numVertices;
		index = polyhedron->facets[j].indVertices;
		plane = polyhedron->facets[j].plane;

		for (i = 0; i < nv; ++i)
		{
			if (index[i] == id)
			{
				fx[k ] -= l(sum[j] + i) * polyhedron->vertices[index[i]].x;
				fx[k + 1] -= l(sum[j] + i) * polyhedron->vertices[index[i]].y;
				fx[k + 2] -= l(sum[j] + i) * polyhedron->vertices[index[i]].z;
				fx[k + 3] -= l(sum[j] + i);
			}
			else
			{
				fx[k ] -= l(sum[j] + i) * (polyhedron->vertices[index[i]].x + x(index[i]));
				fx[k + 1] -= l(sum[j] + i) * (polyhedron->vertices[index[i]].y + y(index[i]));
				fx[k + 2] -= l(sum[j] + i) * (polyhedron->vertices[index[i]].z + z(index[i]));
				fx[k + 3] -= l(sum[j] + i);
			}
		}
		k += 4;
	}
}

void PointShifter::calculateFunctionalDerivative()
{
	int i, j, k, p, m;
	int *index, nv, nf;
	Plane plane;

	k = 0;

	for (i = 0; i < n * n; ++i)
	{
		A[i] = 0.;
	}

	for (j = 0; j < polyhedron->numFacets; ++j)
	{
		nv = polyhedron->facets[j].numVertices;
		index = polyhedron->facets[j].indVertices;
		plane = polyhedron->facets[j].plane;
		for (i = 0; i < nv; ++i)
		{
			if (index[i] == id)
			{
				A[k * n + na(j)] = polyhedron->vertices[index[i]].x;
				A[k * n + nb(j)] = polyhedron->vertices[index[i]].y;
				A[k * n + nc(j)] = polyhedron->vertices[index[i]].z;
				A[k * n + nd(j)] = 1.;
				A[k * n + nx(index[i])] = plane.norm.x + a(j);
				A[k * n + ny(index[i])] = plane.norm.y + b(j);
				A[k * n + nz(index[i])] = plane.norm.z + c(j);
			}
			else
			{
				A[k * n + na(j)] = polyhedron->vertices[index[i]].x + x(index[i]);
				A[k * n + nb(j)] = polyhedron->vertices[index[i]].y + y(index[i]);
				A[k * n + nc(j)] = polyhedron->vertices[index[i]].z + z(index[i]);
				A[k * n + nd(j)] = 1.;
				A[k * n + nx(index[i])] = plane.norm.x + a(j);
				A[k * n + ny(index[i])] = plane.norm.y + b(j);
				A[k * n + nz(index[i])] = plane.norm.z + c(j);
			}
			++k;
		}
	}

	for (i = 0; i < polyhedron->numVertices; ++i)
	{
		if (i == id)
		{
			continue;
		}
		A[k * n + nx(i)] = 2 * eps;
		A[(k + 1) * n + ny(i)] = 2 * eps;
		A[(k + 2) * n + nz(i)] = 2 * eps;

		nf = polyhedron->vertexInfos[i].numFacets;
		;
		index = polyhedron->vertexInfos[i].indFacets;
		for (j = 0; j < nf; ++j)
		{
			p = index[j];
			m = polyhedron->facets[p].find_vertex(i) + sum[p];
//            m = index[2 * j + 1] + sum[p];
			A[k * n + na(p)] = -l(m);
			A[k * n + nl(m)] = -polyhedron->facets[p].plane.norm.x - a(p);
			A[(k + 1) * n + nb(p)] = -l(m);
			A[(k + 1) * n + nl(m)] = -polyhedron->facets[p].plane.norm.y - b(p);
			A[(k + 2) * n + nc(p)] = -l(m);
			A[(k + 2) * n + nl(m)] = -polyhedron->facets[p].plane.norm.z - c(p);
		}
		k += 3;
	}

	for (j = 0; j < polyhedron->numFacets; ++j)
	{
		A[k * n + na(j)] = 2.;
		A[(k + 1) * n + nb(j)] = 2.;
		A[(k + 2) * n + nc(j)] = 2.;
		A[(k + 3) * n + nd(j)] = 2.;

		nv = polyhedron->facets[j].numVertices;
		index = polyhedron->facets[j].indVertices;
		plane = polyhedron->facets[j].plane;

		for (i = 0; i < nv; ++i)
		{
			if (index[i] == id)
			{
				A[k * n + nl(sum[j] + i)] = -polyhedron->vertices[index[i]].x;
				A[(k + 1) * n + nl(sum[j] + i)] = -polyhedron->vertices[index[i]].y;
				A[(k + 2) * n + nl(sum[j] + i)] = -polyhedron->vertices[index[i]].z;
				A[(k + 3) * n + nl(sum[j] + i)] = -1.;
			}
			else
			{
				A[k * n + nx(index[i])] = -l(sum[j] + i);
				A[k * n + nl(sum[j] + i)] = - (polyhedron->vertices[index[i]].x + x(index[i]));
				A[(k + 1) * n + ny(index[i])] = - l(sum[j] + i);
				A[(k + 1) * n + nl(sum[j] + i)] = - (polyhedron->vertices[index[i]].y + y(index[i]));
				A[(k + 2) * n + nz(index[i])] = - l(sum[j] + i);
				A[(k + 2) * n + nl(sum[j] + i)] = - (polyhedron->vertices[index[i]].z + z(index[i]));
				A[(k + 3) * n + nl(sum[j] + i)] = -1.;
			}
		}
		k += 4;
	}
}

void PointShifter::calculateFunctionalDerivative2()
{
	int i, j;
	double ieps = 0.5 / EPS_DERIVATE;
	double *swap0, *swap1;

	for (j = 0; j < n; ++j)
	{
		for (i = 0; i < n; ++i)
		{
			tmp0[i] = x[i];
			tmp1[i] = x[i];
		}
		tmp0[j] += EPS_DERIVATE;
		tmp1[j] -= EPS_DERIVATE;
		swap0 = x;
		swap1 = fx;
		x = tmp0;
		fx = tmp2;
		calculateFunctional();
		x = tmp1;
		fx = tmp3;
		calculateFunctional();
		for (i = 0; i < n; ++i)
			A[i * n + j] = (tmp2[i] - tmp3[i]) * ieps;
		x = swap0;
		fx = swap1;
	}
}
