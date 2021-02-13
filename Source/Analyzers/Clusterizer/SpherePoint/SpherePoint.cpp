/*
 * Copyright (c) 2009-2016 Ilya Palachev <iliyapalachev@gmail.com>
 * Copyright (c) 2009-2012 Nikolai Kaligin <nkaligin@yandex.ru>
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

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "Analyzers/Clusterizer/SpherePoint/SpherePoint.h"
#include "Polyhedron/Facet/Facet.h"

SpherePoint::SpherePoint() : vector(Vector3d(0., 0., 0.)), phi(0.), psi(0.)
{
	DEBUG_START;
	DEBUG_END;
}

SpherePoint::SpherePoint(const Vector3d &vector_orig)
{
	DEBUG_START;
	double x = vector_orig.x;
	double y = vector_orig.y;
	double z = vector_orig.z;

	vector = vector_orig;
	phi = atan2(y, x);
	psi = atan2(z, sqrt(x * x + y * y));
	DEBUG_END;
}

SpherePoint::SpherePoint(const Vector3d &vector_orig, const double phi_orig,
						 const double psi_orig) :
	vector(vector_orig), phi(phi_orig), psi(psi_orig)
{
	DEBUG_START;
	DEBUG_END;
}

SpherePoint::SpherePoint(const double phi_orig, const double psi_orig)
{
	DEBUG_START;
	phi = phi_orig;
	psi = psi_orig;
	double cosinus = cos(psi);
	vector = Vector3d(cos(phi) * cosinus, sin(phi) * cosinus, sin(psi));
	DEBUG_END;
}

SpherePoint::SpherePoint(const SpherePoint &orig) :
	vector(orig.vector), phi(orig.phi), psi(orig.psi)
{
	DEBUG_START;
	DEBUG_END;
}

SpherePoint::~SpherePoint()
{
	DEBUG_START;
	DEBUG_END;
}

SpherePoint &SpherePoint::operator=(const SpherePoint &orig)
{
	DEBUG_START;
	vector = orig.vector;
	phi = orig.phi;
	psi = orig.psi;
	DEBUG_END;
	return *this;
}

double distSpherePoint(SpherePoint &point0, SpherePoint &point1)
{
	DEBUG_START;
	double sinus = sqrt(qmod(point0.vector % point1.vector));
	double cosinus = (point0.vector * point1.vector) /
					 (sqrt(qmod(point0.vector)) * sqrt(qmod(point1.vector)));
	DEBUG_END;
	return atan2(sinus, cosinus);
}

SpherePoint &MassCentre(int n, SpherePoint *points, SpherePoint &newSpherePoint)
{
	DEBUG_START;
	double sum_phi = 0.;
	double sum_psi = 0.;

	for (int i = 0; i < n; ++i)
	{
		sum_phi += points[i].phi;
		sum_psi += points[i].psi;
	}
	sum_phi /= n;
	sum_psi /= n;
	newSpherePoint = SpherePoint(sum_phi, sum_psi);
	DEBUG_END;
	return newSpherePoint;
}

SpherePoint &MassCentre(int n, int *indexFacet, PolyhedronPtr poly)
{
	DEBUG_START;
	SpherePoint newSpherePoint;
	double sum_phi = 0.;
	double sum_psi = 0.;
	double areaOne;
	double areaTotal;

	Vector3d normal;

	areaTotal = 0;
	for (int i = 0; i < n; ++i)
	{
		normal = poly->facets[indexFacet[i]].plane.norm;

		newSpherePoint = SpherePoint(normal);

		areaOne = poly->areaOfFacet(indexFacet[i]);
		areaTotal += areaOne;

		sum_phi += areaOne * newSpherePoint.phi;
		sum_psi += areaOne * newSpherePoint.psi;
	}
	sum_phi /= areaTotal;
	sum_psi /= areaTotal;
	SpherePoint *newSpherePoint_returned = new SpherePoint(sum_phi, sum_psi);
	DEBUG_END;
	return *newSpherePoint_returned;
}
