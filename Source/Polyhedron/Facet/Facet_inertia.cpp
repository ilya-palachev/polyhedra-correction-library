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

#include <cmath>

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "Polyhedron/Facet/Facet.h"

bool Facet::consect_x(double y, double z, double& x)
{
	DEBUG_START;

	int i;
	double u, delta, alpha, sum;
	Vector3d A, A0, A1, normal;
	double a, b, c, d;
	
	Vector3d* vertices = NULL;
	if (auto polyhedron = parentPolyhedron.lock())
	{
		vertices = polyhedron->vertices;
	}
	else
	{
		ERROR_PRINT("parentPolyhedron expired");
		ASSERT(0 && "parentPolyhedron expired");
		DEBUG_END;
		return false;
	}

	normal = plane.norm;
	a = normal.x;
	b = normal.y;
	c = normal.z;
	d = plane.dist;

	if (fabs(a) < 1e-16)
		return false;

	u = -(b * y + c * z + d) / a;
	A = Vector3d(u, y, z);

	normal.norm(1.);
    DEBUG_PRINT("\t\t|n| = %lf,  ", sqrt(qmod(normal)));

	sum = 0.;
	for (i = 0; i < numVertices; ++i)
	{
		A0 = vertices[indVertices[i % numVertices]] - A;
		A1 = vertices[indVertices[(i + 1) % numVertices]] - A;
		delta = (A0 % A1) * normal;
		delta /= sqrt(qmod(A0) * qmod(A1));
		alpha = asin(delta);
        DEBUG_PRINT(" %lf ", alpha / M_PI * 180);
		sum += alpha;
	}

	DEBUG_PRINT(" = sum = %lf*\n", sum / M_PI * 180);
	DEBUG_PRINT("facet[%d].consect_x(%lf, %lf) : sum = %lf\n", id, y, z, sum);

	if (fabs(sum) < 2 * M_PI)
	{
		return false;
	}
	else
	{
		x = u;
		return true;
	}

	DEBUG_END;
}

