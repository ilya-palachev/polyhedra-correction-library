/*
 * Copyright (c) 2009-2013 Ilya Palachev <iliyapalachev@gmail.com>
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

#ifndef SPHEREPOINT_H
#define SPHEREPOINT_H

#include <memory>

#include "Vector3d.h"
#include "Polyhedron/Polyhedron.h"

class SpherePoint
{
public:
	Vector3d vector;
	double phi;
	double psi;

	SpherePoint();
	SpherePoint(const Vector3d& vector_orig);
	SpherePoint(const Vector3d& vector_orig, const double phi_orig,
			const double psi_orig);
	SpherePoint(const double phi_orig, const double psi_orig);
	SpherePoint(const SpherePoint& orig);
	~SpherePoint();
	SpherePoint& operator =(const SpherePoint& orig);
};

double distSpherePoint(SpherePoint& point0, SpherePoint& point1);

SpherePoint& MassCentre(int n, int* indexFacet, shared_ptr<Polyhedron> poly);
SpherePoint& MassCentre(int n, SpherePoint* points);

#endif /* SPHEREPOINT_H */
