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

#include <cmath>
#include <set>

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "Constants.h"
#include "LeastSquaresMethod.h"
#include "Analyzers/SizeCalculator/SizeCalculator.h"
#include "Analyzers/SizeCalculator/SortedDouble/SortedDouble.h"
#include "Polyhedron/Facet/Facet.h"

SizeCalculator::SizeCalculator() :
		PAnalyzer()
{
	DEBUG_START;
	DEBUG_END;
}

SizeCalculator::SizeCalculator(PolyhedronPtr p) :
		PAnalyzer(p)
{
	DEBUG_START;
	DEBUG_END;
}

SizeCalculator::~SizeCalculator()
{
	DEBUG_START;
	DEBUG_END;
}

double SizeCalculator::calculate_J11(int N)
{
	DEBUG_START;

	int k, l;
	double xmin, xmax, ymin, ymax, zmin, zmax;
	double h, s, z, y, integral;

	polyhedron->get_boundary(xmin, xmax, ymin, ymax, zmin, zmax);
	xmin *= 1.001;
	xmax *= 1.001;
	ymin *= 1.001;
	ymax *= 1.001;
	zmin *= 1.001;
	zmax *= 1.001;

	h = 1. / N;

	s = 0.;

	for (k = 0; k < N; ++k)
	{
		z = k * h * (zmax - zmin) + zmin;
		for (l = 0; l < N; ++l)
		{
			y = l * h * (ymax - ymin) + ymin;
			integral = consection_x(y, z);
			s += h * h * integral * (z * z + y * y);
			if (fabs(integral) > 1e-16)
				DEBUG_PRINT("integral[%d, %d] = %lf\n", k, l, integral);
		}
	}

	DEBUG_END;
	return s;
}

double SizeCalculator::consection_x(double y, double z)
{
	DEBUG_START;

	int i;
	double x;
	bool ifConsect;

	SortedDouble SD(20);

	for (i = 0; i < polyhedron->numVertices; ++i)
	{

		ifConsect = polyhedron->facets[i].consect_x(y, z, x);
		if (ifConsect == true)
		{
			DEBUG_PRINT("SD.add(%lf) y = %lf, z = %lf\n", x, y, z);
			SD.add(x);
		}
	}

	DEBUG_END;
	return SD.calclulate();
}

double SizeCalculator::volume()
{
	DEBUG_START;

	int i, j, *index, nv;
	Vector3d B, A0, A1, A2;
	double xmin, xmax, ymin, ymax, zmin, zmax;
	double sum, loc;

	polyhedron->get_boundary(xmin, xmax, ymin, ymax, zmin, zmax);
	B = Vector3d(0., 0., zmax + 1);

	sum = 0.;
	for (i = 0; i < polyhedron->numFacets; ++i)
	{

		index = polyhedron->facets[i].indVertices;
		nv = polyhedron->facets[i].numVertices;
		A0 = polyhedron->vertices[index[0]];
		A0 -= B;
		for (j = 1; j < nv - 1; ++j)
		{
			A1 = polyhedron->vertices[index[j]];
			A2 = polyhedron->vertices[index[j + 1]];
			A1 -= B;
			A2 -= B;
			loc = ((A0 % A1) * A2) * 0.1666666666666666;
			sum += loc;
		}
	}

	DEBUG_END;
	return sum;
}

double SizeCalculator::areaOfSurface()
{
	DEBUG_START;

	int i, j, *index, nv;
	Vector3d A0, A1, A2;
	double sum_poly = 0, loc, sum_facet;

	Vector3d normal;

	for (i = 0; i < polyhedron->numFacets; ++i)
	{

		index = polyhedron->facets[i].indVertices;
		nv = polyhedron->facets[i].numVertices;
		A0 = polyhedron->vertices[index[0]];
		normal = polyhedron->facets[i].plane.norm;

		sum_facet = 0.;
		for (j = 1; j < nv - 1; ++j)
		{

			A1 = polyhedron->vertices[index[j]];
			A2 = polyhedron->vertices[index[j + 1]];
			A1 -= A0;
			A2 -= A0;
			loc = (A1 % A2) * normal * 0.5;
			sum_facet += loc;
		}
		sum_poly += sum_facet;
	}

	DEBUG_END;
	return sum_poly;
}

double SizeCalculator::areaOfFacet(int iFacet)
{
	DEBUG_START;
	DEBUG_END;
	return polyhedron->facets[iFacet].area();
}

void SizeCalculator::J(double& Jxx, double& Jyy, double& Jzz, double& Jxy,
		double& Jyz, double& Jxz)
{
	DEBUG_START;
	int i, j, *index, nv;
	Vector3d B, A0, A1, A2;
	double xmin, xmax, ymin, ymax, zmin, zmax;

	double detJ;

	double Jx, Jy, Jz;
	double Jxy_loc, Jyz_loc, Jxz_loc;

	double x0, y0, z0;

	double ux, uy, uz;
	double vx, vy, vz;
	double wx, wy, wz;

	polyhedron->get_boundary(xmin, xmax, ymin, ymax, zmin, zmax);
	B = Vector3d(0., 0., zmax + 1);
	x0 = B.x;
	y0 = B.y;
	z0 = B.z;

	Jxx = 0.;
	Jyy = 0.;
	Jzz = 0.;
	Jxy = 0.;
	Jxz = 0.;
	Jyz = 0.;
	for (i = 0; i < polyhedron->numFacets; ++i)
	{

		index = polyhedron->facets[i].indVertices;
		nv = polyhedron->facets[i].numVertices;
		A0 = polyhedron->vertices[index[0]];
		A0 -= B;
		ux = A0.x;
		uy = A0.y;
		uz = A0.z;
		for (j = 1; j < nv - 1; ++j)
		{
			A1 = polyhedron->vertices[index[j]];
			A2 = polyhedron->vertices[index[j + 1]];
			A1 -= B;
			A2 -= B;

			vx = A1.x;
			vy = A1.y;
			vz = A1.z;
			wx = A2.x;
			wy = A2.y;
			wz = A2.z;

			detJ = ((A0 % A1) * A2);

			Jx = x0 * x0 / 6.;
			Jx += ux * ux / 60.;
			Jx += vx * vx / 60.;
			Jx += wx * wx / 60.;
			Jx += x0 * ux / 12.;
			Jx += x0 * vx / 12.;
			Jx += x0 * wx / 12.;
			Jx += ux * vx / 60.;
			Jx += ux * wx / 60.;
			Jx += vx * wx / 60.;
			Jx *= detJ;

			Jy = y0 * y0 / 6.;
			Jy += uy * uy / 60.;
			Jy += vy * vy / 60.;
			Jy += wy * wy / 60.;
			Jy += y0 * uy / 12.;
			Jy += y0 * vy / 12.;
			Jy += y0 * wy / 12.;
			Jy += uy * vy / 60.;
			Jy += uy * wy / 60.;
			Jy += vy * wy / 60.;
			Jy *= detJ;

			Jz = z0 * z0 / 6.;
			Jz += uz * uz / 60.;
			Jz += vz * vz / 60.;
			Jz += wz * wz / 60.;
			Jz += z0 * uz / 12.;
			Jz += z0 * vz / 12.;
			Jz += z0 * wz / 12.;
			Jz += uz * vz / 60.;
			Jz += uz * wz / 60.;
			Jz += vz * wz / 60.;
			Jz *= detJ;

			Jxx += Jy + Jz;
			Jyy += Jx + Jz;
			Jzz += Jx + Jy;

			Jxy_loc = x0 * y0 / 6.;
			Jxy_loc += (x0 * uy + y0 * ux) / 24.;
			Jxy_loc += (x0 * vy + y0 * vx) / 24.;
			Jxy_loc += (x0 * wy + y0 * wx) / 24.;
			Jxy_loc += ux * uy / 60.;
			Jxy_loc += vx * vy / 60.;
			Jxy_loc += wx * wy / 60.;
			Jxy_loc += (ux * vy + vx * uy) / 120.;
			Jxy_loc += (ux * wy + wx * uy) / 120.;
			Jxy_loc += (vx * wy + wx * vy) / 120.;
			Jxy_loc *= detJ;

			Jyz_loc = z0 * y0 / 6.;
			Jyz_loc += (z0 * uy + y0 * uz) / 24.;
			Jyz_loc += (z0 * vy + y0 * vz) / 24.;
			Jyz_loc += (z0 * wy + y0 * wz) / 24.;
			Jyz_loc += uz * uy / 60.;
			Jyz_loc += vz * vy / 60.;
			Jyz_loc += wz * wy / 60.;
			Jyz_loc += (uz * vy + vz * uy) / 120.;
			Jyz_loc += (uz * wy + wz * uy) / 120.;
			Jyz_loc += (vz * wy + wz * vy) / 120.;
			Jyz_loc *= detJ;

			Jxz_loc = z0 * x0 / 6.;
			Jxz_loc += (z0 * ux + x0 * uz) / 24.;
			Jxz_loc += (z0 * vx + x0 * vz) / 24.;
			Jxz_loc += (z0 * wx + x0 * wz) / 24.;
			Jxz_loc += uz * ux / 60.;
			Jxz_loc += vz * vx / 60.;
			Jxz_loc += wz * wx / 60.;
			Jxz_loc += (uz * vx + vz * ux) / 120.;
			Jxz_loc += (uz * wx + wz * ux) / 120.;
			Jxz_loc += (vz * wx + wz * vx) / 120.;
			Jxz_loc *= detJ;

			Jxy += Jxy_loc;
			Jyz += Jyz_loc;
			Jxz += Jxz_loc;

		}
	}
	DEBUG_END;
}

void SizeCalculator::get_center(double& xc, double& yc, double& zc)
{
	DEBUG_START;

	int i, j, *index, nv;
	Vector3d B, A0, A1, A2;
	double xmin, xmax, ymin, ymax, zmin, zmax;

	double sum, detJ;

	double xc_loc, yc_loc, zc_loc;

	double x0, y0, z0;

	double ux, uy, uz;
	double vx, vy, vz;
	double wx, wy, wz;

	polyhedron->get_boundary(xmin, xmax, ymin, ymax, zmin, zmax);
	B = Vector3d(0., 0., zmax + 1);
	x0 = B.x;
	y0 = B.y;
	z0 = B.z;

	sum = 0.;
	xc = 0.;
	yc = 0.;
	zc = 0.;
	for (i = 0; i < polyhedron->numFacets; ++i)
	{

		index = polyhedron->facets[i].indVertices;
		nv = polyhedron->facets[i].numVertices;
		A0 = polyhedron->vertices[index[0]];
		A0 -= B;
		ux = A0.x;
		uy = A0.y;
		uz = A0.z;

		for (j = 1; j < nv - 1; ++j)
		{
			A1 = polyhedron->vertices[index[j]];
			A2 = polyhedron->vertices[index[j + 1]];
			A1 -= B;
			A2 -= B;
			vx = A1.x;
			vy = A1.y;
			vz = A1.z;
			wx = A2.x;
			wy = A2.y;
			wz = A2.z;

			detJ = ((A0 % A1) * A2);
			sum += detJ / 6.;

			xc_loc = x0 / 6;
			xc_loc += (ux + vx + wx) / 24.;
			xc_loc *= detJ;

			yc_loc = y0 / 6;
			yc_loc += (uy + vy + wy) / 24.;
			yc_loc *= detJ;

			zc_loc = z0 / 6;
			zc_loc += (uz + vz + wz) / 24.;
			zc_loc *= detJ;

			xc += xc_loc;
			yc += yc_loc;
			zc += zc_loc;
		}
	}
	xc /= sum;
	yc /= sum;
	zc /= sum;

	DEBUG_END;
}

void SizeCalculator::inertia(double& l0, double& l1, double& l2, Vector3d& v0,
		Vector3d& v1, Vector3d& v2)
{
	DEBUG_START;

	double **JJ, **v, *l;

	JJ = new double*[3];
	JJ[0] = new double[3];
	JJ[1] = new double[3];
	JJ[2] = new double[3];

	v = new double*[3];
	v[0] = new double[3];
	v[1] = new double[3];
	v[2] = new double[3];

	l = new double[3];

	double Jxx, Jyy, Jzz, Jxy, Jyz, Jxz;
	J(Jxx, Jyy, Jzz, Jxy, Jyz, Jxz);

	JJ[0][0] = Jxx;
	JJ[0][1] = Jxy;
	JJ[0][2] = Jxz;
	JJ[1][0] = Jxy;
	JJ[1][1] = Jyy;
	JJ[1][2] = Jyz;
	JJ[2][0] = Jxz;
	JJ[2][1] = Jyz;
	JJ[2][2] = Jzz;

	jacobi(3, JJ, l, v);

	l0 = l[0];
	l1 = l[1];
	l2 = l[2];

	v0 = Vector3d(v[0][0], v[1][0], v[2][0]);
	v1 = Vector3d(v[0][1], v[1][1], v[2][1]);
	v2 = Vector3d(v[0][2], v[1][2], v[2][2]);

	delete[] JJ[0];
	delete[] JJ[1];
	delete[] JJ[2];
	delete[] JJ;
	delete[] v[0];
	delete[] v[1];
	delete[] v[2];
	delete[] v;
	delete[] l;

	DEBUG_END;
}

void SizeCalculator::printSortedByAreaFacets(void)
{
	DEBUG_START;

	std::list< FacetWithArea > listFacetsSorted = getSortedByAreaFacets();
	for (auto itFacet = listFacetsSorted.begin(); itFacet !=
		listFacetsSorted.end(); ++itFacet)
	{
		PRINT("area of facet #%d = %lf", itFacet->facet->get_id(),
			itFacet->area);
	}
	
	DEBUG_END;
}

std::list< FacetWithArea > SizeCalculator::getSortedByAreaFacets (void)
{
	DEBUG_START;
	auto comparer = [](struct FacetWithArea f0, struct FacetWithArea f1)
	{
		return f0.area < f1.area;
	};
	std::set<FacetWithArea, decltype(comparer)> facetsSorted(comparer);

	for (int iFacet = 0; iFacet < polyhedron->numFacets; ++iFacet)
	{
		facetsSorted.insert({&polyhedron->facets[iFacet],
			areaOfFacet(iFacet)});
	}

	std::list< FacetWithArea > listFacetsSorted;
	for (auto itFacet = facetsSorted.begin(); itFacet != facetsSorted.end();
		 ++itFacet)
	{
		listFacetsSorted.push_back(*itFacet);
	}
	
	DEBUG_END;
	return listFacetsSorted;
}

Vector3d SizeCalculator::calculateFacetCenter(int iFacet)
{
	DEBUG_START;
	Vector3d center;
	DEBUG_VARIABLE double area =
			polyhedron->facets[iFacet].calculateAreaAndCenter(center);
	DEBUG_END;
	return center;
}

Vector3d SizeCalculator::calculateSurfaceCenter(void)
{
	DEBUG_START;
	Vector3d centerSurface(0., 0., 0.);
	Vector3d centerFacet(0., 0., 0.);
	double areaSurface = 0.;
	double areaFacet = 0.;

	for (int iFacet = 0; iFacet < polyhedron->numFacets; ++iFacet)
	{
		areaFacet =
			polyhedron->facets[iFacet].calculateAreaAndCenter(centerFacet);
		areaFacet = fabs(areaFacet);
		centerSurface += areaFacet * centerFacet;
		areaSurface += areaFacet;
	}
	ASSERT(std::isfinite(areaSurface));
	ASSERT(fabs(areaSurface) > EPS_MIN_DOUBLE);
	centerSurface /= areaSurface;
	ASSERT(std::isfinite(centerSurface.x));
	ASSERT(std::isfinite(centerSurface.y));
	ASSERT(std::isfinite(centerSurface.z));

	DEBUG_END;
	return centerSurface;
}
