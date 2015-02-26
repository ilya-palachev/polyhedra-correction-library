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

#include <math.h>

#include "Vector3d.h"
#include "Gauss_string.h"

Vector3d &Vector3d::norm(double d)
{
	double t = x * x + y * y + z * z;
	if (!equal(t, 0.))
	{
		d /= sqrt(t);
		x *= d;
		y *= d;
		z *= d;
	}
	return *this;
}

Vector3d Vector3d::perpendicular() const
{
	double ax = fabs(x);
	double ay = fabs(y);
	double az = fabs(z);
	if (ax >= ay)
	{
		if (ax >= az)
		{
			if (equal(ax, 0))
				return Vector3d(1., 0., 0.);
			double d = 1. / sqrt(ax * ax + ay * ay);
			return Vector3d(-y * d, x * d, 0.);
		}
	}
	else
	{
		if (ay >= az)
		{
			double d = 1. / sqrt(ay * ay + az * az);
			return Vector3d(0., -z * d, y * d);
		}
	}
	double d = 1. / sqrt(ax * ax + az * az);
	return Vector3d(z * d, 0., -x * d);
}

double length(const Vector3d &v)
{
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

Plane::Plane(const Vector3d &x, const Vector3d &y, const Vector3d &z)
{
	const Vector3d xy = x - y;
	const double d1 = qmod(xy);
	const Vector3d yz = y - z;
	const double d2 = qmod(yz);
	const Vector3d zx = z - x;
	const double d3 = qmod(zx);
	const double d123 = d1 + d2 + d3;
	norm = xy % yz;
	if (norm * norm < 1e-22 * d123 * d123)
	{
		if (d1 >= d2)
			if (d1 >= d3)
				norm = xy.perpendicular();
			else
				norm = zx.perpendicular();
		else if (d2 >= d3)
			norm = yz.perpendicular();
		else
			norm = zx.perpendicular();
	}
	else
		norm.norm(1.);
	dist = ((x + y + z) * norm) / -3.;
}

Vector3d Plane::project(const Vector3d &v) const
{
	return v - (*this % v) * norm;
}

bool intersection(const Plane &plane1, const Plane &plane2,
		const Plane &plane3, Vector3d &point)
{
	double *A, *b;
	bool ret;
	A = new double[9];
	b = new double[3];
	A[0] = plane1.norm.x;
	A[1] = plane1.norm.y;
	A[2] = plane1.norm.z;
	A[3] = plane2.norm.x;
	A[4] = plane2.norm.y;
	A[5] = plane2.norm.z;
	A[6] = plane3.norm.x;
	A[7] = plane3.norm.y;
	A[8] = plane3.norm.z;
	b[0] = -plane1.dist;
	b[1] = -plane2.dist;
	b[2] = -plane3.dist;
	ret = Gauss_string(3, A, b);
	point.x = b[0];
	point.y = b[1];
	point.z = b[2];

	if (A != NULL)
	{
		delete[] A;
		A = NULL;
	}
	if (b != NULL)
	{
		delete[] b;
		b = NULL;
	}
	return ret;
}

bool intersection(const Plane &plane1, const Plane &plane2, Vector3d &dir,
		Vector3d &point)
{
	const Vector3d v = plane1.norm % plane2.norm;
	const double s = v * v;
	if (s < 1e-16)
	{
		return false;
	}
	dir = v;
	dir /= sqrt(s);

	const double ax = fabs(plane1.norm.x);
	const double ay = fabs(plane1.norm.y);
	const double az = fabs(plane1.norm.z);
	if (ax >= ay && ax >= az)
	{
		const double c = plane2.norm.x / plane1.norm.x;
		const double y = plane2.norm.y - c * plane1.norm.y;
		const double z = plane2.norm.z - c * plane1.norm.z;
		const double d = plane2.dist - c * plane1.dist;
		if (y * z < 0)
		{
			point.y = d / (z - y);
			point.z = -point.y;
		}
		else
		{
			point.y = point.z = -d / (z + y);
		}
		point.x = -(point.y * plane1.norm.y + point.z * plane1.norm.z
				+ plane1.dist) / plane1.norm.x;
	}
	else if (ay >= az)
	{
		const double c = plane2.norm.y / plane1.norm.y;
		const double x = plane2.norm.x - c * plane1.norm.x;
		const double z = plane2.norm.z - c * plane1.norm.z;
		const double d = plane2.dist - c * plane1.dist;
		if (x * z < 0)
		{
			point.x = d / (z - x);
			point.z = -point.x;
		}
		else
		{
			point.x = point.z = -d / (z + x);
		}
		point.y = -(point.x * plane1.norm.x + point.z * plane1.norm.z
				+ plane1.dist) / plane1.norm.y;
	}
	else
	{
		const double c = plane2.norm.z / plane1.norm.z;
		const double y = plane2.norm.y - c * plane1.norm.y;
		const double x = plane2.norm.x - c * plane1.norm.x;
		const double d = plane2.dist - c * plane1.dist;
		if (y * x < 0)
		{
			point.y = d / (x - y);
			point.x = -point.y;
		}
		else
		{
			point.y = point.x = -d / (x + y);
		}
		point.z = -(point.y * plane1.norm.y + point.x * plane1.norm.x
				+ plane1.dist) / plane1.norm.z;
	}
	return true;
}

bool intersection(const Plane & plane, const Vector3d & dir,
		const Vector3d & point, Vector3d & res)
{
	const double s = plane.norm * dir;
	if (fabs(s) < 1e-8)
	{
		return false;
	}
	res = point - dir * ((plane % point) / s);
	return true;
}

