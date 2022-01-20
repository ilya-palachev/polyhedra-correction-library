/*
 * Copyright (c) 2009-2016 Ilya Palachev <iliyapalachev@gmail.com>
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
#include "LeastSquaresMethod.h"
#include "Correctors/PointShifterLinear/PointShifterLinear.h"
#include "Polyhedron/Facet/Facet.h"
#include "Polyhedron/VertexInfo/VertexInfo.h"

#define EPSILON 1e-7
//#define DEFORM_SCALE //Этот макрос определяет, каким методом производить
//деформацию:
// экспоненциальным ростом штрафа или масштабированием
// (если он определен - то масштабированием)

#define MAX_STEPS 1e4

PointShifterLinear::PointShifterLinear() : PCorrector(), id(), x(), y(), z(), xold(), yold(), zold(), A(), B(), K()
{
	DEBUG_START;
	DEBUG_END;
}

PointShifterLinear::PointShifterLinear(PolyhedronPtr p) :
	PCorrector(p),
	id(),
	x(new double[polyhedron->numVertices]),
	y(new double[polyhedron->numVertices]),
	z(new double[polyhedron->numVertices]),
	xold(new double[polyhedron->numVertices]),
	yold(new double[polyhedron->numVertices]),
	zold(new double[polyhedron->numVertices]),
	A(new double[9]),
	B(new double[3]),
	K()
{
	DEBUG_START;
	DEBUG_END;
}

PointShifterLinear::PointShifterLinear(Polyhedron *p) :
	PCorrector(p),
	id(),
	x(new double[polyhedron->numVertices]),
	y(new double[polyhedron->numVertices]),
	z(new double[polyhedron->numVertices]),
	xold(new double[polyhedron->numVertices]),
	yold(new double[polyhedron->numVertices]),
	zold(new double[polyhedron->numVertices]),
	A(new double[9]),
	B(new double[3]),
	K()
{
	DEBUG_START;
	DEBUG_END;
}

PointShifterLinear::~PointShifterLinear()
{
	DEBUG_START;
	if (x != NULL)
	{
		delete[] x;
		x = NULL;
	}
	if (y != NULL)
	{
		delete[] y;
		y = NULL;
	}
	if (z != NULL)
	{
		delete[] z;
		z = NULL;
	}
	if (x != NULL)
	{
		delete[] xold;
		xold = NULL;
	}
	if (y != NULL)
	{
		delete[] yold;
		yold = NULL;
	}
	if (z != NULL)
	{
		delete[] zold;
		zold = NULL;
	}
	if (A != NULL)
	{
		delete[] A;
		A = NULL;
	}
	if (B != NULL)
	{
		delete[] B;
		B = NULL;
	}
	DEBUG_END;
}

void PointShifterLinear::runGlobal(int id, Vector3d delta)
{
	DEBUG_START;
	int step = 0, i = 0;
	double err = 0., err_eps = 0.;
	DEBUG_VARIABLE double norm = 0.;

	polyhedron->vertices[id] += delta;

	for (i = 0; i < polyhedron->numVertices; ++i)
	{
		xold[i] = polyhedron->vertices[i].x;
		yold[i] = polyhedron->vertices[i].y;
		zold[i] = polyhedron->vertices[i].z;
	}

#ifndef DEFORM_SCALE
	step = 0;
	K = 100;
	err = calculateError();
	//    K = sqrt(qmod(delta)) / err;
	err_eps = EPSILON;
	for (i = 0; err > 1e-10; ++i)
	{
		do
		{

			DEBUG_PRINT("\\hline\n %d & %d & ", i, step);
			//                    vertex[id].x = xold[id];
			//                    vertex[id].y = yold[id];
			//                    vertex[id].z = zold[id];
			//            vertex[id] = delta;
			moveFacets();
			polyhedron->countConsections(true);
			//            join_points();
			moveVerticesGlobal();
			err = calculateError();
			norm = calculateDeform();
			DEBUG_PRINT("%le & %le & %lf\\\\\n", err, norm, K);
			++step;
			if (step > MAX_STEPS)
			{
				DEBUG_PRINT("Too much steps...");
				DEBUG_END;
				return;
			}
			//            } while (err > err_eps);
		} while (0);

		//            K += K;
		//        K *= 1.005;
		K *= 2;
		//            K = norm / err;
		//            if (K < 100.)
		//                K = 100;
		//            K += 1.;
		err_eps *= 0.5;
	}
#endif
#ifdef DEFORM_SCALE
	step = 0;
	err = deform_linear_calculate_error();
	//        K = sqrt(qmod(delta)) / err;
	K = 1. / err;
	do
	{
		moveFacets();
		polyhedron->countConsections();
		moveVerticesGlobal();
		err = calculateError();
		norm = calculateDeform();
		if (step % 100 == 0 || step >= 2059)
		{
			DEBUG_PRINT("\\hline\n %d & %le & %le & %lf\\\\\n", step, err, norm, K);
			DEBUG_PRINT("%d\t%d\terr = %le\tnorm = %le\tK = %lf\n", i, step++, err, norm, K);
		}
		++step;
		if (step > MAX_STEPS)
		{
			DEBUG_PRINT("Too much steps...");
			DEBUG_END;
			return;
		}
		K = norm / err;
	} while (err > 1e-10);
#endif
	DEBUG_END;
}

double PointShifterLinear::calculateError()
{
	DEBUG_START;
	int i, j, nv, *index;
	double err, a, b, c, d, x, y, z, locerr;
	err = 0.;

	for (j = 0; j < polyhedron->numFacets; ++j)
	{
		nv = polyhedron->facets[j].numVertices;
		index = polyhedron->facets[j].indVertices;
		a = polyhedron->facets[j].plane.norm.x;
		b = polyhedron->facets[j].plane.norm.y;
		c = polyhedron->facets[j].plane.norm.z;
		d = polyhedron->facets[j].plane.dist;
		if (nv < 3)
			continue;
		locerr = 0.;
		for (i = 0; i < nv; ++i)
		{
			x = polyhedron->vertices[index[i]].x;
			y = polyhedron->vertices[index[i]].y;
			z = polyhedron->vertices[index[i]].z;
			locerr += fabs(a * x + b * y + c * z + d);
		}
		err += locerr;
		DEBUG_PRINT("error[%d] = %lf\n", j, locerr);
	}

	DEBUG_END;
	return err;
}

double PointShifterLinear::calculateDeform()
{
	DEBUG_START;
	int i;
	double deform, xx, yy, zz;
	deform = 0.;
	for (i = 0; i < polyhedron->numVertices; ++i)
	{
		xx = polyhedron->vertices[i].x;
		yy = polyhedron->vertices[i].y;
		zz = polyhedron->vertices[i].z;
		deform += (xx - xold[i]) * (xx - xold[i]);
		deform += (yy - yold[i]) * (yy - yold[i]);
		deform += (zz - zold[i]) * (zz - zold[i]);
	}
	DEBUG_END;
	return deform;
}

void PointShifterLinear::moveFacets()
{
	DEBUG_START;
	int i, j, nv, *index;
	double a0, b0, c0, d0;
	double a, b, c, d;
	double lambda;

	for (j = 0; j < polyhedron->numFacets; ++j)
	{
		nv = polyhedron->facets[j].numVertices;
		index = polyhedron->facets[j].indVertices;
		for (i = 0; i < nv; ++i)
		{
			x[i] = polyhedron->vertices[index[i]].x;
			y[i] = polyhedron->vertices[index[i]].y;
			z[i] = polyhedron->vertices[index[i]].z;
		}
		runListSquaresMethod(nv, x, y, z, a, b, c, d);
		a0 = polyhedron->facets[j].plane.norm.x;
		b0 = polyhedron->facets[j].plane.norm.y;
		c0 = polyhedron->facets[j].plane.norm.z;
		d0 = polyhedron->facets[j].plane.dist;
		// Чтобы минимизировать разницу норм плоскостей:
		lambda = a * a0 + b * b0 + c * c0 + d * d0;
		lambda /= a * a + b * b + c * c + d * d;
		a *= lambda;
		b *= lambda;
		c *= lambda;
		d *= lambda;
		polyhedron->facets[j].plane.norm.x = a;
		polyhedron->facets[j].plane.norm.y = b;
		polyhedron->facets[j].plane.norm.z = c;
		polyhedron->facets[j].plane.dist = d;
	}
	DEBUG_END;
}

void PointShifterLinear::moveVerticesGlobal()
{
	DEBUG_START;
	int i, j, nf, *index;
	double a, b, c, d;
	double Maa, Mab, Mac, Mad, Mbb, Mbc, Mbd, Mcc, Mcd;

	Plane pl;

	for (i = 0; i < polyhedron->numVertices; ++i)
	{
		if (i == id)
			continue;
		Maa = 0.;
		Mab = 0.;
		Mac = 0.;
		Mad = 0.;
		Mbb = 0.;
		Mbc = 0.;
		Mbd = 0.;
		Mcc = 0.;
		Mcd = 0.;
		nf = polyhedron->vertexInfos[i].numFacets;
		index = polyhedron->vertexInfos[i].indFacets;
		for (j = 0; j < nf; ++j)
		{
			pl = polyhedron->facets[index[j]].plane;
			a = pl.norm.x;
			b = pl.norm.y;
			c = pl.norm.z;
			d = pl.dist;
			Maa += a * a;
			Mab += a * b;
			Mac += a * c;
			Mad += a * d;
			Mbb += b * b;
			Mbc += b * c;
			Mbd += b * d;
			Mcc += c * c;
			Mcd += c * d;
		}
		A[0] = 1. + K * Maa;
		A[1] = K * Mab;
		A[2] = K * Mac;
		A[3] = A[1];
		A[4] = 1. + K * Mbb;
		A[5] = K * Mbc;
		A[6] = A[2];
		A[7] = A[5];
		A[8] = 1. + K * Mcc;
		B[0] = -K * Mad + xold[i];
		B[1] = -K * Mbd + yold[i];
		B[2] = -K * Mcd + zold[i];
		Gauss_string(3, A, B);
		polyhedron->vertices[i].x = B[0];
		polyhedron->vertices[i].y = B[1];
		polyhedron->vertices[i].z = B[2];
	}
	DEBUG_END;
}

void PointShifterLinear::runLocal(int id, Vector3d delta)
{
	DEBUG_START;
	int step = 0, i = 0;
	double err = 0., err_eps = 0.;
	DEBUG_VARIABLE double norm = 0.;
	int ncons_curr, ncons_prev;

	//    delta += vertex[id]; //Сохраняем положение деформированной точки
	polyhedron->vertices[id] += delta;

	for (i = 0; i < polyhedron->numVertices; ++i)
	{
		xold[i] = polyhedron->vertices[i].x;
		yold[i] = polyhedron->vertices[i].y;
		zold[i] = polyhedron->vertices[i].z;
	}

#ifndef DEFORM_SCALE
	step = 0;
	K = 100;
	//    err = deform_linear_calculate_error();
	//    K = sqrt(qmod(delta)) / err;
	err_eps = EPSILON;
	ncons_prev = polyhedron->countConsections(false);
	for (i = 0; i < 10; ++i)
	{
		do
		{
			polyhedron->vertices[id].x = xold[id];
			polyhedron->vertices[id].y = yold[id];
			polyhedron->vertices[id].z = zold[id];
			//            vertex[id] = delta;
			moveFacets();
			//                join_points();
			moveVerticesLocal();
			err = calculateError();
			norm = calculateDeform();

			ncons_curr = polyhedron->countConsections(false);
			if (ncons_curr != ncons_prev)
			{
				ncons_prev = ncons_curr;
			}

			DEBUG_PRINT("\\hline\n %d & %d & ", i, step);
			DEBUG_PRINT("%le & %le & %lf & %d\\\\\n", err, norm, K, ncons_curr);
			polyhedron->countConsections(true);
			polyhedron->groupVertices(id);

			++step;
			if (step > MAX_STEPS)
			{
				DEBUG_PRINT("Too much steps...");
				DEBUG_END;
				return;
			}
		} while (err > err_eps);

		K *= 1.005;
		//        K = norm / err;
		//        K += 1.;
		err_eps *= 0.5;
	}

#endif
#ifdef DEFORM_SCALE
	char *fname;
	fname = new char[255];

	step = 0;
	err = calculateError();
	K = sqrt(qmod(delta)) / err;

	ncons_prev = polyhedron->countConsections();
	ifPrint = false;

	//    K = 1. / err;
	do
	{
		polyhedron->vertices[id].x = xold[id];
		polyhedron->vertices[id].y = yold[id];
		polyhedron->vertices[id].z = zold[id];
		moveFacets();

		ncons_curr = polyhedron->countConsections();
		//        if (ncons_curr != ncons_prev) {
		if (1)
		{
			ifPrint = true;
			ncons_prev = ncons_curr;
		}

		moveVerticesLocal();
		err = calculateError();
		norm = calculateDeform();
		//        if (step % 100 == 0 || step >= 2059) {
		if (ifPrint)
		{
			DEBUG_PRINT("\\hline\n %d & %le & %le & %lf & %d\\\\\n", step, err, norm, K, ncons_curr);
			sprintf(fname, "../poly-data-out/Consections - %d.ply", ncons_curr);
			fprint_ply_scale(1000., fname, "generated-in-deform-linear");
			ifPrint = false;

			DEBUG_PRINT("%d\t%d\terr = %le\tnorm = %le\tK = %lf\n", i, step++, err, norm, K);
		}
		++step;
		if (step > MAX_STEPS)
		{
			DEBUG_PRINT("Too much steps...");
			DEBUG_END;
			return;
		}
		K = norm / err;
	} while (err > 1e-10);
#endif

	polyhedron->delete_empty_facets();
	DEBUG_END;
}

void PointShifterLinear::moveVerticesLocal()
{
	DEBUG_START;
	int i, j, nf, *index;
	double a, b, c, d;
	double Maa, Mab, Mac, Mad, Mbb, Mbc, Mbd, Mcc, Mcd;
	double norm;

	Plane pl;

	norm = 0.;
	for (i = 0; i < polyhedron->numVertices; ++i)
	{
		Maa = 0.;
		Mab = 0.;
		Mac = 0.;
		Mad = 0.;
		Mbb = 0.;
		Mbc = 0.;
		Mbd = 0.;
		Mcc = 0.;
		Mcd = 0.;
		nf = polyhedron->vertexInfos[i].numFacets;
		if (nf == 0)
		{
			DEBUG_PRINT("Vertex %d cannot be found in any facet...\n", i);
			continue;
		}
		index = polyhedron->vertexInfos[i].indFacets;
		for (j = 0; j < nf; ++j)
		{
			pl = polyhedron->facets[index[j]].plane;
			a = pl.norm.x;
			b = pl.norm.y;
			c = pl.norm.z;
			d = pl.dist;
			Maa += a * a;
			Mab += a * b;
			Mac += a * c;
			Mad += a * d;
			Mbb += b * b;
			Mbc += b * c;
			Mbd += b * d;
			Mcc += c * c;
			Mcd += c * d;
		}
		A[0] = 1. + K * Maa;
		A[1] = K * Mab;
		A[2] = K * Mac;
		A[3] = A[1];
		A[4] = 1. + K * Mbb;
		A[5] = K * Mbc;
		A[6] = A[2];
		A[7] = A[5];
		A[8] = 1. + K * Mcc;
		B[0] = -K * Mad + polyhedron->vertices[i].x;
		B[1] = -K * Mbd + polyhedron->vertices[i].y;
		B[2] = -K * Mcd + polyhedron->vertices[i].z;
		Gauss_string(3, A, B);
		norm += (B[0] - polyhedron->vertices[i].x) * (B[0] - polyhedron->vertices[i].x);
		norm += (B[1] - polyhedron->vertices[i].y) * (B[1] - polyhedron->vertices[i].y);
		norm += (B[2] - polyhedron->vertices[i].z) * (B[2] - polyhedron->vertices[i].z);
		polyhedron->vertices[i].x = B[0];
		polyhedron->vertices[i].y = B[1];
		polyhedron->vertices[i].z = B[2];
	}

	DEBUG_END;
}

void PointShifterLinear::runTest(int id, Vector3d delta, int mode, int &num_steps, double &norm_sum)
{
	DEBUG_START;

	int step, i;
	double err, err_eps;
	double norm;

	polyhedron->vertices[id] += delta;

	DEBUG_PRINT("vertex[%d] = (%lf, %lf, %lf)\n", id, polyhedron->vertices[id].x, polyhedron->vertices[id].y,
				polyhedron->vertices[id].z);

	for (i = 0; i < polyhedron->numVertices; ++i)
	{
		xold[i] = polyhedron->vertices[i].x;
		yold[i] = polyhedron->vertices[i].y;
		zold[i] = polyhedron->vertices[i].z;
	}

	switch (mode)
	{
	case 0:
		step = 0;
		K = 100;
		//    err = deform_linear_calculate_error();
		//    K = sqrt(qmod(delta)) / err;
		err_eps = EPSILON;
		for (i = 0; i < 10; ++i)
		{
			do
			{
				moveFacets();
				moveVerticesGlobal();
				err = calculateError();
				norm = calculateDeform();
				++step;
				if (step > MAX_STEPS)
				{
					DEBUG_PRINT("Too much steps...\n");
					DEBUG_END;
					return;
				}
			} while (err > err_eps);

			K *= 2.;
			//        K = norm / err;
			//        K += 1.;
			err_eps *= 0.5;
		}
		num_steps = step;
		norm_sum = norm;
		break;
	case 1:
		step = 0;
		err = calculateError();
		K = sqrt(qmod(delta)) / err;
		//    K = 1. / err;
		do
		{
			moveFacets();
			moveVerticesGlobal();
			err = calculateError();
			norm = calculateDeform();
			++step;
			if (step > MAX_STEPS)
			{
				DEBUG_PRINT("Too much steps...\n");
				DEBUG_END;
				return;
			}
			K = norm / err;
		} while (err > 1e-10);
		num_steps = step;
		norm_sum = norm;
		break;
	default:
		break;
	}

	DEBUG_PRINT("vertex[%d] = (%lf, %lf, %lf)\n", id, polyhedron->vertices[id].x, polyhedron->vertices[id].y,
				polyhedron->vertices[id].z);
	DEBUG_END;
}

void PointShifterLinear::runPartial(int id, Vector3d delta, int num)
{
	DEBUG_START;
	int i;
	double coeff;
	Vector3d delta_step;

	if (num == 1)
	{
		runGlobal(id, delta);
	}
	else
	{
		coeff = 1. / num;
		delta_step = delta * coeff;
		for (i = 0; i < num; ++i)
		{
			runGlobal(id, delta_step);
		}
	}
	DEBUG_END;
}
