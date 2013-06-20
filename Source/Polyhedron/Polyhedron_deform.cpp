#include "PolyhedraCorrectionLibrary.h"

double norm_vector(int n, double* a);

void print_matrix(FILE* file, int n, int m, double* A);

void print_matrix_bool(FILE* file, int n, int m, bool* A);


#define EPSILON 1e-11
#define EPS_DERIVATE 1e-8

void Polyhedron::deform(int id, Vector3d delta)
{
	PointShifter* pShifter = new PointShifter(this);
	pShifter->run(id, delta);
	delete pShifter;
}

#undef a
#undef b
#undef c
#undef d

#undef x
#undef y
#undef z

#undef l

#undef na
#undef nb
#undef nc
#undef nd

#undef nx
#undef ny
#undef nz

#undef nl

#define nx(i) (3 * ((i) - ((i) > id)))
#define ny(i) (3 * ((i) - ((i) > id)) + 1)
#define nz(i) (3 * ((i) - ((i) > id)) + 2)
#define na(j) (3 * (numVertices - 1) + 4 * (j))
#define nb(j) (3 * (numVertices - 1) + 4 * (j) + 1)
#define nc(j) (3 * (numVertices - 1) + 4 * (j) + 2)
#define nd(j) (3 * (numVertices - 1) + 4 * (j) + 3)

#define x(i) X[nx(i)]
#define y(i) X[ny(i)]
#define z(i) X[nz(i)]
#define a(i) X[na(i)]
#define b(i) X[nb(i)]
#define c(i) X[nc(i)]
#define d(i) X[nd(i)]

#define khi(i, j) khi[(i) * numVertices + (j)]

void Polyhedron::deform_w(int id, Vector3d delta)
{
	int i, j, step;
	double *X, *X1, *fx, *A, err, err_new, gamma;
	double *tmp0, *tmp1, *tmp2, *tmp3;
	bool success;

	bool *khi;
	double K;

	int auto_step;

	int n; // Число переменных = число условий = размерность вектора

	double xx, yy, zz;
	double a, b, c, d;
	int nv, *index;
	for (int j = 0; j < numFacets; ++j)
	{
		a = facets[j].plane.norm.x;
		b = facets[j].plane.norm.y;
		c = facets[j].plane.norm.z;
		d = facets[j].plane.dist;
		index = facets[j].indVertices;
		nv = facets[j].numVertices;
		for (i = 0; i < nv; ++i)
		{
			xx = vertices[index[i]].x;
			yy = vertices[index[i]].y;
			zz = vertices[index[i]].z;
			printf("v%d -> f%d : %lf\n", index[i], j,
					a * xx + b * yy + c * zz + d);
		}
	}

	vertices[id] = vertices[id] + delta;
	n = 4 * numFacets + 3 * (numVertices - 1);

	X = new double[n];
	tmp0 = new double[n];
	tmp1 = new double[n];
	tmp2 = new double[n];
	tmp3 = new double[n];
	X1 = new double[n];
	fx = new double[n];
	A = new double[n * n];
	khi = new bool[numFacets * numVertices];

	for (i = 0; i < numVertices; ++i)
		for (j = 0; j < numFacets; ++j)
			khi[i * numFacets + j] = (facets[j].find_vertex(i) != -1);
	print_matrix_bool(stdout, numVertices, numFacets, khi);

	for (i = 0; i < n; ++i)
	{
		X[i] = 0.;
	}
	K = 1;
	f_w(n, X, fx, id, khi, K);
	err = norm_vector(n, fx);

	step = 0;
	while (err > EPSILON)
	{
//        derf(n, X, A, id, sum);
		derf_w(n, X, A, id, khi, K, tmp0, tmp1, tmp2, tmp3);
		f_w(n, X, fx, id, khi, K);
		err = norm_vector(n, fx);

		printf("step %d\terr = %le\n", step++, err);
//        print_vector(n, fx);
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
			X1[i] = fx[i];
		}
		gamma = 2.;
		auto_step = 0;
		printf("\tauto_step : ");
		do
		{
			if (auto_step > 0)
				break;

			gamma *= 0.5;
			++auto_step;
			printf("%d ", auto_step);

			for (i = 0; i < n; ++i)
				X[i] += gamma * X1[i];
			f_w(n, X, fx, id, khi, K);
			err_new = norm_vector(n, fx);
		} while (err_new > err);
		printf("\n");
	}

	for (i = 0; i < numVertices; ++i)
	{
		if (i == id)
			continue;
		vertices[i].x += x(i);
		vertices[i].y += y(i);
		vertices[i].z += z(i);
	}
	for (i = 0; i < numFacets; ++i)
	{
		facets[i].plane.norm.x += a(i);
		facets[i].plane.norm.y += b(i);
		facets[i].plane.norm.z += c(i);
		facets[i].plane.dist += d(i);
	}

	for (int j = 0; j < numFacets; ++j)
	{
		a = facets[j].plane.norm.x;
		b = facets[j].plane.norm.y;
		c = facets[j].plane.norm.z;
		d = facets[j].plane.dist;
		index = facets[j].indVertices;
		nv = facets[j].numVertices;
		for (i = 0; i < nv; ++i)
		{
			xx = vertices[index[i]].x;
			yy = vertices[index[i]].y;
			zz = vertices[index[i]].z;
			printf("v%d -> f%d : %lf\n", index[i], j,
					a * xx + b * yy + c * zz + d);
		}
	}

	if (X != NULL)
		delete[] X;
	if (tmp0 != NULL)
		delete[] tmp0;
	if (tmp1 != NULL)
		delete[] tmp1;
	if (tmp2 != NULL)
		delete[] tmp2;
	if (tmp3 != NULL)
		delete[] tmp3;
	if (X1 != NULL)
		delete[] X1;
	if (fx != NULL)
		delete[] fx;
	if (A != NULL)
		delete[] A;
	if (khi != NULL)
		delete[] khi;
	printf("End of deform_w\n");
}

void Polyhedron::f_w(int n, double* X, double* fx, int id, bool* khi, int K)
{
	int i, j;
	double sx, sy, sz;
	double sa, sb, sc, sd;
	double aj, bj, cj, dj;
	double xi, yi, zi;
	double coeff;

//    printf("test : ");
//    for (i = 0; i < numv; ++i) {
//        if (i != id)
//            printf("%d %d %d ", nx(i), ny(i), nz(i));
//    }
//    for (i = 0; i < numf; ++i) {
//        printf("%d %d %d %d ", na(i), nb(i), nc(i), nd(i));
//    }
//    printf("\n");

	for (i = 0; i < numVertices; ++i)
	{
		if (i == id)
			continue;
		xi = vertices[i].x;
		yi = vertices[i].y;
		zi = vertices[i].z;
		sx = 0;
		sy = 0;
		sz = 0;
		for (j = 0; j < numFacets; ++j)
		{
			if (khi(i, j))
			{
				aj = facets[j].plane.norm.x;
				bj = facets[j].plane.norm.y;
				cj = facets[j].plane.norm.z;
				dj = facets[j].plane.dist;
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

	for (j = 0; j < numFacets; ++j)
	{
		aj = facets[j].plane.norm.x;
		bj = facets[j].plane.norm.y;
		cj = facets[j].plane.norm.z;
		dj = facets[j].plane.dist;
		sa = 0;
		sb = 0;
		sc = 0;
		sd = 0;
		for (i = 0; i < numVertices; ++i)
		{
			if (khi(i, j))
			{
				xi = vertices[i].x;
				yi = vertices[i].y;
				zi = vertices[i].z;
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
}

void Polyhedron::derf_w(int n, double* x, double* A, int id, bool* khi, int K,
		double* tmp0, double* tmp1, double* tmp2, double* tmp3)
{
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
		f_w(n, tmp0, tmp2, id, khi, K);
		f_w(n, tmp1, tmp3, id, khi, K);
		for (i = 0; i < n; ++i)
			A[i * n + j] = (tmp2[i] - tmp3[i]) * ieps;
	}
}

