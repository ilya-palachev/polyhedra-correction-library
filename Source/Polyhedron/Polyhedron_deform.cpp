#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "PolyhedraCorrectionLibrary.h"

#define eps 1.
#define EPSILON 1e-11
#define EPS_DERIVATE 1e-8

#define na(i) (4 * (i))
#define nb(i) (4 * (i) + 1)
#define nc(i) (4 * (i) + 2)
#define nd(i) (4 * (i) + 3)

#define nx(i) (4 * numFacets + 3 * (i - (i > id)))
#define ny(i) (4 * numFacets + 3 * (i - (i > id)) + 1)
#define nz(i) (4 * numFacets + 3 * (i - (i > id)) + 2)

#define nl(i) (4 * numFacets + 3  * (numVertices - 1) + (i))

#define a(i) X[na(i)]
#define b(i) X[nb(i)]
#define c(i) X[nc(i)]
#define d(i) X[nd(i)]
#define x(i) X[nx(i)]
#define y(i) X[ny(i)]
#define z(i) X[nz(i)]
#define l(i) X[nl(i)]

//#define a(i) X[4 * (i)]
//#define b(i) X[4 * (i) + 1]
//#define c(i) X[4 * (i) + 2]
//#define d(i) X[4 * (i) + 3]
//
//#define x(i) X[4 * numf + 3 * (i) - ((i) > id)]
//#define y(i) X[4 * numf + 3 * (i) + 1 - ((i) > id)]
//#define z(i) X[4 * numf + 3 * (i) + 2 - ((i) > id)]
//
//#define l(i) X[4 * numf + 3  * (numv - 1) + (i)]
//
//#define na(i) (4 * (i))
//#define nb(i) (4 * (i) + 1)
//#define nc(i) (4 * (i) + 2)
//#define nd(i) (4 * (i) + 3)
//
//#define nx(i) (4 * numf + 3 * (i) - ((i) > id))
//#define ny(i) (4 * numf + 3 * (i) + 1 - ((i) > id))
//#define nz(i) (4 * numf + 3 * (i) + 2 - ((i) > id))
//
//#define nl(i) (4 * numf + 3  * (numv - 1) + (i))

double norm_vector(
		int n,
		double* a) {
	int i;
	double max, tmp;
	max = 0.;
	for (i = 0; i < n; ++i) {
		tmp = fabs(a[i]);
		if (tmp > max)
			max = tmp;
	}
	return max;
//    int i;
//    double sum = 0.;
//    for (i = 0; i < n; ++i)
//        sum += a[i] * a[i];
//    return sqrt(sum);
}

void print_matrix(
		FILE* file,
		int n,
		int m,
		double* A) {
	int i, j;

	fprintf(file, "\n");
	for (i = 0; i < m; ++i)
		fprintf(file, "--");
	fprintf(file, "\n");

	for (i = 0; i < n; ++i) {
		for (j = 0; j < m; ++j) {
			if (fabs(A[i * n + j]) >= 1e-16) {
				if (fabs(A[i * n + j]) < 9.5)
					fprintf(file, "%1.0lf ", fabs(A[i * n + j]));
				else
					fprintf(file, "# ");
			} else
				fprintf(file, "  ");
		}
		fprintf(file, "|\n");
	}
	for (i = 0; i < m; ++i)
		fprintf(file, "--");
	fprintf(file, "\n");
}

void print_matrix_bool(
		FILE* file,
		int n,
		int m,
		bool* A) {
	int i, j;

	fprintf(file, "\n");
	for (i = 0; i < m; ++i)
		fprintf(file, "--");
	fprintf(file, "\n");

	for (i = 0; i < n; ++i) {
		for (j = 0; j < m; ++j) {
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

void print_vector(
		int n,
		double* v) {
	int i;
	for (i = 0; i < n; ++i)
		printf("\t%lf\n", v[i]);
}

void Polyhedron::deform(
		int id,
		Vector3d delta) {
	int i, step;
	double *X, *X1, *fx, *A, err, err_new, gamma;
	double *tmp0, *tmp1, *tmp2, *tmp3;
	bool success;

	int auto_step;

	int n; // Число переменных = число условий = размерность вектора
	int *sum;

	double xx, yy, zz;
	double a, b, c, d;
	int nv, *index;
	for (int j = 0; j < numFacets; ++j) {
		a = facets[j].plane.norm.x;
		b = facets[j].plane.norm.y;
		c = facets[j].plane.norm.z;
		d = facets[j].plane.dist;
		index = facets[j].indVertices;
		nv = facets[j].numVertices;
		for (i = 0; i < nv; ++i) {
			xx = vertices[index[i]].x;
			yy = vertices[index[i]].y;
			zz = vertices[index[i]].z;
			printf("v%d -> f%d : %lf\n", index[i], j, a * xx + b * yy + c * zz + d);
		}
	}

	vertices[id] = vertices[id] + delta;

	n = 0;
	for (i = 0; i < numFacets; ++i) {
		n += facets[i].numVertices;
	}
	n += 4 * numFacets + 3 * (numVertices - 1);

//    n = 3;

	sum = new int[numFacets];
	sum[0] = 0;
	for (i = 0; i < numFacets - 1; ++i) {
		sum[i + 1] = sum[i] + facets[i].numVertices;
	}

	for (i = 0; i < numFacets; ++i) {
		printf("sum[%d] = %d\n", i, sum[i]);
	}

	X = new double[n];
	tmp0 = new double[n];
	tmp1 = new double[n];
	tmp2 = new double[n];
	tmp3 = new double[n];
	X1 = new double[n];
	fx = new double[n];
	A = new double[n * n];

	for (i = 0; i < n; ++i) {
		X[i] = 0.;
	}
//    X[0] = 1.01;
//    X[1] = 0.01;
//    X[2] = 0.01;
	f(n, X, fx, id, sum);
	err = norm_vector(n, fx);

	step = 0;
	while (err > EPSILON) {
//        derf(n, X, A, id, sum);
		derf2(n, X, A, id, sum, tmp0, tmp1, tmp2, tmp3);
		f(n, X, fx, id, sum);
		err = norm_vector(n, fx);

		printf("step %d\terr = %le\n", step++, err);
		print_vector(n, fx);
		if (err > 100) {
			printf("Ошибка. Метод Ньютона разошелся\n");
			break;
		}
		print_matrix(stdout, n, n, A);

		success = Gauss_string(n, A, fx);
		if (!success) {
			printf("Stoping deform...\n");
			return;
		}
		for (i = 0; i < n; ++i) {
			X1[i] = fx[i];
		}
		gamma = 2.;
		auto_step = 0;
		printf("\tauto_step : ");
		do {
			if (auto_step > 100)
				break;

			gamma *= 0.5;
			++auto_step;
			printf("%d ", auto_step);

			for (i = 0; i < n; ++i)
				X[i] += gamma * X1[i];
			f(n, X, fx, id, sum);
			err_new = norm_vector(n, fx);
		} while (err_new > err);
		printf("\n");
	}

	for (i = 0; i < numVertices; ++i) {
		if (i == id)
			continue;
		vertices[i].x += x(i);
		vertices[i].y += y(i);
		vertices[i].z += z(i);
	}
	for (i = 0; i < numFacets; ++i) {
		facets[i].plane.norm.x += a(i);
		facets[i].plane.norm.y += b(i);
		facets[i].plane.norm.z += c(i);
		facets[i].plane.dist += d(i);
	}

	for (int j = 0; j < numFacets; ++j) {
		a = facets[j].plane.norm.x;
		b = facets[j].plane.norm.y;
		c = facets[j].plane.norm.z;
		d = facets[j].plane.dist;
		index = facets[j].indVertices;
		nv = facets[j].numVertices;
		for (i = 0; i < nv; ++i) {
			xx = vertices[index[i]].x;
			yy = vertices[index[i]].y;
			zz = vertices[index[i]].z;
			printf("v%d -> f%d : %lf\n", index[i], j, a * xx + b * yy + c * zz + d);
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

}

void Polyhedron::f(
		int n,
		double* X,
		double* fx,
		int id,
		int* sum) {
	int i, j, k, p, m;
	int *index, nv, nf;
	Plane plane;

	k = 0;

//    printf("test : ");
//    for (i = 0; i < numf; ++i) {
//        printf("%d %d %d %d ", na(i), nb(i), nc(i), nd(i));
//    }
//    for (i = 0; i < numv; ++i) {
//        if (i != id)
//            printf("%d %d %d ", nx(i), ny(i), nz(i));
//    }
//    for (j = 0; j < numf; ++j) {
//        nv = facet[j].nv;
//        for (i = 0; i < nv; ++i)
//            printf("%d ", nl(sum[j] + i));
//    }
//    printf("\n\n");

	for (j = 0; j < numFacets; ++j) {
		nv = facets[j].numVertices;
		index = facets[j].indVertices;
		plane = facets[j].plane;
		for (i = 0; i < nv; ++i) {
//            printf("fx[%d] = ", k);
			if (index[i] == id) {
				fx[k] = (vertices[index[i]].x) * (plane.norm.x + a(j));
				fx[k] += (vertices[index[i]].y) * (plane.norm.y + b(j));
				fx[k] += (vertices[index[i]].z) * (plane.norm.z + c(j));
				fx[k] += plane.dist + d(j);
//                printf("X%d*(a0%d + a%d) + Y%d*(b0%d + b%d) + Z%d*(c0%d + c%d) + d0%d + d%d\n",
//                        id, j, j, id, j, j, id, j, j, j, j);
					} else {
//                printf("(%.2lf + %.2lf) * (%.2lf + %.2lf) + \n", 
//                        vertex[index[i]].x, x(index[i]), plane.norm.x, a(j));
//                printf("(%.2lf + %.2lf) * (%.2lf + %.2lf) + \n",
//                        vertex[index[i]].y, y(index[i]), plane.norm.y, b(j));
//                printf("(%.2lf + %.2lf) * (%.2lf + %.2lf) + %.2lf + %.2lf = 0  ???\n\n",
//                        vertex[index[i]].z, z(index[i]), plane.norm.z, c(j),
//                        plane.dist, d(j));
						fx[k] = (vertices[index[i]].x + x(index[i])) * (plane.norm.x + a(j));                
                fx[k] += (vertices[index[i]].y + y(index[i])) * (plane.norm.y + b(j));
                fx[k] += (vertices[index[i]].z + z(index[i])) * (plane.norm.z + c(j));
                fx[k] += plane.dist + d(j);
//                printf("(x0%d + x%d)*(a0%d + a%d) + (y0%d + y%d)*(b0%d + b%d) + (z0%d + z%d)*(c0%d + c%d) + d0%d + d%d\n",
//                    index[i], index[i], j, j, index[i], index[i], j, j, index[i], index[i], j, j, j, j);

            }
				++k;
			}
		}

	for (i = 0; i < numVertices; ++i) {
		if (i == id) {
			continue;
		}
		fx[k] = 2 * eps * x(i);
		fx[k + 1] = 2 * eps * y(i);
		fx[k + 2] = 2 * eps * z(i);

//        printf("fx[%d] = 2eps x%d", k, i);

		nf = vertexInfos[i].numFacets;
		;
		index = vertexInfos[i].indFacets;
		for (j = 0; j < nf; ++j) {
			p = index[j];
			m = facets[p].find_vertex(i);
//            m = index[2 * j + 1];
			fx[k] -= l(m + sum[p])* (facets[p].plane.norm.x + a(p));
			fx[k + 1] -= l(m + sum[p])* (facets[p].plane.norm.y + b(p));
			fx[k + 2] -= l(m + sum[p])* (facets[p].plane.norm.z + c(p));
//            printf(" + l%d%d (a0%d + a%d)", p, m, p, p);
		}
//        printf("\n");
//        printf("fx[%d] = ...\n", k + 1);        
//        printf("fx[%d] = ...\n", k + 2);        
		k += 3;
	}

	for (j = 0; j < numFacets; ++j) {
		fx[k] = 2 * a(j);
		fx[k + 1] = 2 * b(j);
		fx[k + 2] = 2 * c(j);
		fx[k + 3] = 2 * d(j);
//        printf("fx[%d] = 2a%d", k, j);

		nv = facets[j].numVertices;
		index = facets[j].indVertices;
		plane = facets[j].plane;

		for (i = 0; i < nv; ++i) {
			if (index[i] == id) {
				fx[k ] -= l(sum[j] + i) * vertices[index[i]].x;
				fx[k + 1] -= l(sum[j] + i) * vertices[index[i]].y;
				fx[k + 2] -= l(sum[j] + i) * vertices[index[i]].z;
				fx[k + 3] -= l(sum[j] + i);
//                printf(" + l%d%d X%d", j, i, index[i]);
			} else {
				fx[k ] -= l(sum[j] + i) * (vertices[index[i]].x + x(index[i]));
				fx[k + 1] -= l(sum[j] + i) * (vertices[index[i]].y + y(index[i]));
				fx[k + 2] -= l(sum[j] + i) * (vertices[index[i]].z + z(index[i]));
				fx[k + 3] -= l(sum[j] + i);
//                printf(" + l%d%d (x0%d + x%d)", j, i, index[i]);
			}
		}
//        printf("\n");
//        printf("fx[%d] = 2b%d + ...\n", k + 1, j);
//        printf("fx[%d] = 2c%d + ...\n", k + 2, j);
//        printf("fx[%d] = 2d%d + ...\n", k + 3, j);
		k += 4;
	}
}

void Polyhedron::derf(
		int n,
		double* X,
		double* A,
		int id,
		int* sum) {
	int i, j, k, p, m;
	int *index, nv, nf;
	Plane plane;

	k = 0;

	for (i = 0; i < n * n; ++i) {
		A[i] = 0.;
	}

	for (j = 0; j < numFacets; ++j) {
		nv = facets[j].numVertices;
		index = facets[j].indVertices;
		plane = facets[j].plane;
		for (i = 0; i < nv; ++i) {
			if (index[i] == id) {
				A[k * n + na(j)] = vertices[index[i]].x;
				A[k * n + nb(j)] = vertices[index[i]].y;
				A[k * n + nc(j)] = vertices[index[i]].z;
				A[k * n + nd(j)] = 1.;
				A[k * n + nx(index[i])] = plane.norm.x + a(j);
				A[k * n + ny(index[i])] = plane.norm.y + b(j);
				A[k * n + nz(index[i])] = plane.norm.z + c(j);
			} else {
				A[k * n + na(j)] = vertices[index[i]].x + x(index[i]);
				A[k * n + nb(j)] = vertices[index[i]].y + y(index[i]);
				A[k * n + nc(j)] = vertices[index[i]].z + z(index[i]);
				A[k * n + nd(j)] = 1.;
				A[k * n + nx(index[i])] = plane.norm.x + a(j);
				A[k * n + ny(index[i])] = plane.norm.y + b(j);
				A[k * n + nz(index[i])] = plane.norm.z + c(j);
			}
			++k;
		}
	}

	for (i = 0; i < numVertices; ++i) {
		if (i == id) {
			continue;
		}
		A[k * n + nx(i)] = 2 * eps;
		A[(k + 1) * n + ny(i)] = 2 * eps;
		A[(k + 2) * n + nz(i)] = 2 * eps;

		nf = vertexInfos[i].numFacets;
		;
		index = vertexInfos[i].indFacets;
		for (j = 0; j < nf; ++j) {
			p = index[j];
			m = facets[p].find_vertex(i) + sum[p];
//            m = index[2 * j + 1] + sum[p];
			A[k * n + na(p)] = -l(m);
			A[k * n + nl(m)] = -facets[p].plane.norm.x - a(p);
			A[(k + 1) * n + nb(p)] = -l(m);
			A[(k + 1) * n + nl(m)] = -facets[p].plane.norm.y - b(p);
			A[(k + 2) * n + nc(p)] = -l(m);
			A[(k + 2) * n + nl(m)] = -facets[p].plane.norm.z - c(p);
		}
		k += 3;
	}

	for (j = 0; j < numFacets; ++j) {
		A[k * n + na(j)] = 2.;
		A[(k + 1) * n + nb(j)] = 2.;
		A[(k + 2) * n + nc(j)] = 2.;
		A[(k + 3) * n + nd(j)] = 2.;

		nv = facets[j].numVertices;
		index = facets[j].indVertices;
		plane = facets[j].plane;

		for (i = 0; i < nv; ++i) {
			if (index[i] == id) {
				A[k * n + nl(sum[j] + i)] = -vertices[index[i]].x;
				A[(k + 1) * n + nl(sum[j] + i)] = -vertices[index[i]].y;
				A[(k + 2) * n + nl(sum[j] + i)] = -vertices[index[i]].z;
				A[(k + 3) * n + nl(sum[j] + i)] = -1.;
			} else {
				A[k * n + nx(index[i])] = -l(sum[j] + i);
				A[k * n + nl(sum[j] + i)] = - (vertices[index[i]].x + x(index[i]));
				A[(k + 1) * n + ny(index[i])] = - l(sum[j] + i);
				A[(k + 1) * n + nl(sum[j] + i)] = - (vertices[index[i]].y + y(index[i]));
				A[(k + 2) * n + nz(index[i])] = - l(sum[j] + i);
				A[(k + 2) * n + nl(sum[j] + i)] = - (vertices[index[i]].z + z(index[i]));
				A[(k + 3) * n + nl(sum[j] + i)] = -1.;
			}
		}
		k += 4;
	}
//    //Транспонирование
//    double tmp;
//    for (i = 0; i < n; ++i)
//        for (j = i + 1; j < n; ++j) {
//            tmp = A[i * n + j];
//            A[i * n + j] = A[j * n + i];
//            A[j * n + i] = tmp;
//        }
}

//void Polyhedron::f(int n, double* x, double* fx, int id, int* sum) {
////    fx[0] = x[0] * x[0] + x[1] * x[1] - 1.;
////    fx[1] = (x[0] - 2.) * (x[0] - 2.) + x[1] * x[1] - 1.;
//
////    fx[0] = x[3] - x[0] * x[0] - x[1] * x[1];
////    fx[1] = x[0] * x[0] + x[1] * x[1] + x[2] * x[2] - 1.;
////    fx[2] = (x[0] - 1.) * (x[0] - 1.) + (x[1] - 1.) * (x[1] - 1.) - 2.;
//    
//    fx[0] = (x[0] - 2.) * (x[0]- 2.) + x[1] * x[1] + x[2] * x[2] - 1.;
//    fx[1] = (x[0] - 1.) * (x[0] - 1.) + (x[1] - 1.) * (x[1] - 1.) + x[2] * x[2] - 1.;
////    fx[2] = (x[0] - 3.) * (x[0]- 3.) + x[1] * x[1] + x[2] * x[2] - 1.;
//    fx[2] = x[0] * x[0] + x[1] * x[1] + x[2] * x[2] - 1.;
//    
//    n = n;
//    id = id;
//    sum = sum;
//}
//
//void Polyhedron::derf(int n, double* x, double* A, int id, int* sum) {
////    A[0] = 2. * x[0];
////    A[1] = 2. * x[1];
////    A[2] = 2. * x[0] - 4.;
////    A[3] = 2. * x[1];
//    
////    A[0] = 1.;
////    A[1] = -2. * x[0];
////    A[2] = 2. * x[1];
////    A[3] = 2. * x[0];
////    A[4] = 2. * x[1];
////    A[5] = 2. * x[2];
////    A[6] = 2. * x[0] - 2.;
////    A[7] = 2. * x[1] - 2.;
////    A[8] = 0.;
//    
//    A[0] = 2 * x[0] - 4;
//    A[1] = 2 * x[1];
//    A[2] = 2 * x[2];
//    A[3] = 2 * x[0] - 2;
//    A[4] = 2 * x[1] - 2;
//    A[5] = 2 * x[2];
//    A[6] = 2 * x[0];
//    A[7] = 2 * x[1];
//    A[8] = 2 * x[2];
//    
//    n = n;
//    id = id;
//    sum = sum;
//    
//}

void Polyhedron::derf2(
		int n,
		double* x,
		double* A,
		int id,
		int* sum,
		double* tmp0,
		double* tmp1,
		double* tmp2,
		double* tmp3) {
	int i, j;
	double ieps = 0.5 / EPS_DERIVATE;

	for (j = 0; j < n; ++j) {
		for (i = 0; i < n; ++i) {
			tmp0[i] = x[i];
			tmp1[i] = x[i];
		}
		tmp0[j] += EPS_DERIVATE;
		tmp1[j] -= EPS_DERIVATE;
		f(n, tmp0, tmp2, id, sum);
		f(n, tmp1, tmp3, id, sum);
		for (i = 0; i < n; ++i)
			A[i * n + j] = (tmp2[i] - tmp3[i]) * ieps;
	}
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

void Polyhedron::deform_w(
		int id,
		Vector3d delta) {
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
	for (int j = 0; j < numFacets; ++j) {
		a = facets[j].plane.norm.x;
		b = facets[j].plane.norm.y;
		c = facets[j].plane.norm.z;
		d = facets[j].plane.dist;
		index = facets[j].indVertices;
		nv = facets[j].numVertices;
		for (i = 0; i < nv; ++i) {
			xx = vertices[index[i]].x;
			yy = vertices[index[i]].y;
			zz = vertices[index[i]].z;
			printf("v%d -> f%d : %lf\n", index[i], j, a * xx + b * yy + c * zz + d);
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

	for (i = 0; i < n; ++i) {
		X[i] = 0.;
	}
	K = 1;
	f_w(n, X, fx, id, khi, K);
	err = norm_vector(n, fx);

	step = 0;
	while (err > EPSILON) {
//        derf(n, X, A, id, sum);
		derf_w(n, X, A, id, khi, K, tmp0, tmp1, tmp2, tmp3);
		f_w(n, X, fx, id, khi, K);
		err = norm_vector(n, fx);

		printf("step %d\terr = %le\n", step++, err);
//        print_vector(n, fx);
		if (err > 100) {
			printf("Ошибка. Метод Ньютона разошелся\n");
			break;
		}
		print_matrix(stdout, n, n, A);

		success = Gauss_string(n, A, fx);
		if (!success) {
			printf("Stoping deform...\n");
			return;
		}
		for (i = 0; i < n; ++i) {
			X1[i] = fx[i];
		}
		gamma = 2.;
		auto_step = 0;
		printf("\tauto_step : ");
		do {
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

	for (i = 0; i < numVertices; ++i) {
		if (i == id)
			continue;
		vertices[i].x += x(i);
		vertices[i].y += y(i);
		vertices[i].z += z(i);
	}
	for (i = 0; i < numFacets; ++i) {
		facets[i].plane.norm.x += a(i);
		facets[i].plane.norm.y += b(i);
		facets[i].plane.norm.z += c(i);
		facets[i].plane.dist += d(i);
	}

	for (int j = 0; j < numFacets; ++j) {
		a = facets[j].plane.norm.x;
		b = facets[j].plane.norm.y;
		c = facets[j].plane.norm.z;
		d = facets[j].plane.dist;
		index = facets[j].indVertices;
		nv = facets[j].numVertices;
		for (i = 0; i < nv; ++i) {
			xx = vertices[index[i]].x;
			yy = vertices[index[i]].y;
			zz = vertices[index[i]].z;
			printf("v%d -> f%d : %lf\n", index[i], j, a * xx + b * yy + c * zz + d);
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

void Polyhedron::f_w(
		int n,
		double* X,
		double* fx,
		int id,
		bool* khi,
		int K) {
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

	for (i = 0; i < numVertices; ++i) {
		if (i == id)
			continue;
		xi = vertices[i].x;
		yi = vertices[i].y;
		zi = vertices[i].z;
		sx = 0;
		sy = 0;
		sz = 0;
		for (j = 0; j < numFacets; ++j) {
			if (khi(i, j)) {
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

	for (j = 0; j < numFacets; ++j) {
		aj = facets[j].plane.norm.x;
		bj = facets[j].plane.norm.y;
		cj = facets[j].plane.norm.z;
		dj = facets[j].plane.dist;
		sa = 0;
		sb = 0;
		sc = 0;
		sd = 0;
		for (i = 0; i < numVertices; ++i) {
			if (khi(i, j)) {
				xi = vertices[i].x;
				yi = vertices[i].y;
				zi = vertices[i].z;
				if (i == id) {
					coeff = (aj + a(j)) * xi
					+ (bj + b(j)) * yi
					+ (cj + c(j)) * zi
					+ dj + d(j);
					sa += coeff * xi;
					sb += coeff * yi;
					sc += coeff * zi;
					sd += coeff;
				} else {
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

void Polyhedron::derf_w(
		int n,
		double* x,
		double* A,
		int id,
		bool* khi,
		int K,
		double* tmp0,
		double* tmp1,
		double* tmp2,
		double* tmp3) {
	int i, j;
	double ieps = 0.5 / EPS_DERIVATE;

	for (j = 0; j < n; ++j) {
		for (i = 0; i < n; ++i) {
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

