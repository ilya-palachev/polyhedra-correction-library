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

/**
 * @file Vector3d.h
 * @brief 3D vectors and planes declaration, with their intersections.
 */

#ifndef VECTOR3D_H
#define VECTOR3D_H

#include <cmath>
#include <istream>
#include <ostream>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
typedef CGAL::Exact_predicates_inexact_constructions_kernel KernelVector3dLocal;

#include "DebugPrint.h"

/**
 * Checks the FP equality of double numbers
 *
 * @param a	1st number
 * @param b	2nd number
 * @return true, if std:fpclassify is FP_ZERO.
 */
static inline bool equal(const double a, const double b)
{
	return std::fpclassify(a - b) == FP_ZERO;
}

/** Simple 3D vector class.*/
class Vector3d
{
public:
	double x; /**< The X coordinate. */
       	double y; /**< The Y coordinate. */
        double z; /**< The Z coordinate. */

	/** Empty constructor. */
	Vector3d() :
		x(0.),
		y(0.),
		z(0.)
	{
	}

	/**
	 * Constructor by coordinates.
	 *
	 * @param a	The X coordinate.
	 * @param b	The Y coordinate.
	 * @param c	The Z coordinate.
	 */
	Vector3d(double a, double b, double c) :
		x(a),
		y(b),
		z(c)
	{
	}

	/**
	 * Converter fromCGAL Point_3 object.
	 *
	 * @param p	The CGAL point.
	 */
	static Vector3d fromCGAL(KernelVector3dLocal::Point_3 p)
	{
		return Vector3d(p.x(), p.y(), p.z());
	}

	/**
	 * Constructor by CGAL Vector_3 object.
	 *
	 * @param v	The CGAL vector.
	 */
	static Vector3d fromCGAL(KernelVector3dLocal::Vector_3 v)
	{
		return Vector3d(v.x(), v.y(), v.z());
	}

	/**
	 * Simple assignment operator
	 *
	 * @param v	The vector that should be assigned from.
	 * @return The result of the assignment.
	 */
	Vector3d &operator =(const Vector3d &v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}

	/**
	 * Compares 2 vectors in lexicographical order.
	 *
	 * @param v	Reference to 2nd vector
	 * @return	The result of the comparison.
	 */
	bool operator <(const Vector3d &v) const
	{
		if (x < v.x)
			return true;
		else if (x <= v.x)
		{
			if (y < v.y)
				return true;
			else if ((y <= v.y) &&z < v.z)
				return true;
		}
		return false;
	}

	/**
	 * Compares 2 vectors in lexicographical order.
	 *
	 * @param v	Reference to 2nd vector
	 * @return	The result of the comparison.
	 */
	bool operator >(const Vector3d &v) const
	{
		if (x > v.x)
			return true;
		else if (x >= v.x)
		{
			if (y > v.y)
				return true;
			else if ((y >= v.y) &&z > v.z)
				return true;
		}
		return false;
	}

	/**
	 * Compares 2 vectors in lexicographical order.
	 *
	 * @param v	Reference to 2nd vector
	 * @return	The result of the comparison.
	 */
	bool operator >=(const Vector3d &v) const
	{
		return !(this->operator<(v));
	}

	/**
	 * Compares 2 vectors in lexicographical order.
	 *
	 * @param v	Reference to 2nd vector
	 * @return	The result of the comparison.
	 */
	bool operator <=(const Vector3d &v) const
	{
		return !(this->operator>(v));
	}

	/* TODO: complete comments for all functions. */
	Vector3d operator -() const
	{
		return Vector3d(-x, -y, -z);
	}

	Vector3d &operator +=(const Vector3d &v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	Vector3d &operator -=(const Vector3d &v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	Vector3d &operator *=(const double d)
	{
		x *= d;
		y *= d;
		z *= d;
		return *this;
	}

    double dot(const Vector3d &other) const {
        return x * other.x + y * other.y + z * other.z;
    }

	Vector3d &operator /=(const double d)
	{
		x /= d;
		y /= d;
		z /= d;
		return *this;
	}

	Vector3d &null()
	{
		x = y = z = 0.;
		return *this;
	}

	bool operator !() const
	{
		return equal(x, 0.) && equal (y, 0.) && equal(z, 0.);
	}

	Vector3d &norm(double d);

	Vector3d perpendicular() const;

	/**
	 * Prints vector to the stream.
	 *
	 * @param stream	The output stream
	 * @param a		The vector
	 *
	 * @return The stream ready for further printing
	 */
	friend std::ostream &operator <<(std::ostream &stream,
			const Vector3d &a)
	{
		DEBUG_START;
		stream << "(" << a.x << ", " << a.y << ", " << a.z << ")";
		DEBUG_END;
		return stream;
	}

	/**
	 * Scans vector from the stream.
	 *
	 * @param stream	The input stream
	 * @param a		The vector
	 *
	 * @return The stream ready for further scanning
	 */
	friend std::istream &operator >>(std::istream &stream,
			Vector3d &a)
	{
		DEBUG_START;
		stream >> a.x;
		stream >> a.y;
		stream >> a.z;
		DEBUG_END;
		return stream;
	}
};

inline Vector3d operator +(const Vector3d &a, const Vector3d &b)
{
	return Vector3d(a.x + b.x, a.y + b.y, a.z + b.z);
}

inline Vector3d operator -(const Vector3d &a, const Vector3d &b)
{
	return Vector3d(a.x - b.x, a.y - b.y, a.z - b.z);
}

inline Vector3d operator *(const Vector3d &a, double d)
{
	return Vector3d(a.x * d, a.y * d, a.z * d);
}

inline Vector3d operator /(const Vector3d &a, double d)
{
	return Vector3d(a.x / d, a.y / d, a.z / d);
}

inline Vector3d operator *(double d, const Vector3d &a)
{
	return Vector3d(a.x * d, a.y * d, a.z * d);
}

inline double operator *(const Vector3d &a, const Vector3d &b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline double qmod(const Vector3d &a)
{
	return a.x * a.x + a.y * a.y + a.z * a.z;
}

inline Vector3d operator %(const Vector3d &a, const Vector3d &b)
{
	return Vector3d(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x);
}

inline bool operator ==(const Vector3d &a, const Vector3d &b)
{
	return !(a - b);
}

inline bool operator !=(const Vector3d &a, const Vector3d &b)
{
	return !(a == b);
}

double length(const Vector3d &v);

class Plane
{
public:
	Vector3d norm;
	double dist;

	Plane() :
		norm(Vector3d(0., 0., 0.)),
		dist(0.)
	{
	}

	Plane(const Vector3d &, const Vector3d &, const Vector3d &);

	Plane(const Vector3d &v, const double &d) :
		norm(v),
		dist(d)
	{
	}

	double operator %(const Vector3d &v) const
	{
		return norm.x * v.x + norm.y * v.y + norm.z * v.z + dist;
	}

	Plane operator -() const
	{
		return Plane(-norm, -dist);
	}

	Vector3d project(const Vector3d &v) const;

	friend std::ostream &operator <<(std::ostream &stream,
			const Plane &a)
	{
		stream << "{(" << a.norm.x << ")x + (" << a.norm.y << ")y +("
			<< a.norm.z << ")z + (" << a.dist << ") == 0}";
		return stream;
	}
};

inline bool operator ==(const Plane &p1, const Plane &p2)
{
	return p1.norm == p2.norm && equal(p1.dist, p2.dist);
}

inline bool operator !=(const Plane &p1, const Plane &p2)
{
	return p1.norm != p2.norm || !equal(p1.dist, p2.dist);
}

bool intersection(const Plane &plane1, const Plane &plane2,
		const Plane &plane3, Vector3d &point);

bool intersection(const Plane &plane1, const Plane &plane2, Vector3d &dir,
		Vector3d &point);

bool intersection(const Plane &plane, const Vector3d &dir,
		const Vector3d &point, Vector3d &res);

/**
 * Checks that double are equal under given precision.
 *
 * @param a		1st number
 * @param b		2nd number
 * @param precision	The precision
 * @return true, if the absolute difference is not greater than precision.
 */
static inline bool equal(const double a, const double b, const double precision)
{
	return fabs(a - b) <= precision;
}

/**
 * Checks that 3d vectors are equal under given precision.
 *
 * @param a		1st vector
 * @param b		2nd vector
 * @param precision	The precision
 * @return true, if the L-inf norm of the difference is not greater than the
 * precision.
 */
static inline bool equal(const Vector3d a, const Vector3d b,
		const double precision)
{
	return equal(a.x, b.x, precision) && equal(a.y, b.y, precision)
		&& equal(a.z, b.z, precision);
}

#endif /* VECTOR3D_H */

