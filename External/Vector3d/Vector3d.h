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

/**
 * @file Vector3d.h
 * @brief 3D vectors and planes declaration, with their intersections.
 */

#ifndef VECTOR3D_H
#define VECTOR3D_H

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
	 * Simple assignment operator
	 *
	 * @param v	The vector that should be assigned from.
	 * @return The result of the assignment.
	 */
	Vector3d& operator =(const Vector3d& v)
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
	bool operator <(const Vector3d& v) const
	{
		if (x < v.x)
			return true;
		else if (x <= v.x)
		{
			if (y < v.y)
				return true;
			else if ((y <= v.y) && z < v.z)
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
	bool operator >(const Vector3d& v) const
	{
		if (x > v.x)
			return true;
		else if (x >= v.x)
		{
			if (y > v.y)
				return true;
			else if ((y >= v.y) && z > v.z)
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
	bool operator >=(const Vector3d& v) const
	{
		return !(this->operator<(v));
	}

	/**
	 * Compares 2 vectors in lexicographical order.
	 *
	 * @param v	Reference to 2nd vector
	 * @return	The result of the comparison.
	 */
	bool operator <=(const Vector3d& v) const
	{
		return !(this->operator>(v));
	}

	/* TODO: complete comments for all functions. */
	Vector3d operator -() const
	{
		return Vector3d(-x, -y, -z);
	}

	Vector3d & operator +=(const Vector3d & v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	Vector3d & operator -=(const Vector3d & v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	Vector3d & operator *=(const double d)
	{
		x *= d;
		y *= d;
		z *= d;
		return *this;
	}

	Vector3d & operator /=(const double d)
	{
		x /= d;
		y /= d;
		z /= d;
		return *this;
	}

	Vector3d & null()
	{
		x = y = z = 0.;
		return *this;
	}

	bool operator !() const
	{
		return fpclassify(x) == FP_ZERO &&
			fpclassify(y) == FP_ZERO &&
			fpclassify(z) == FP_ZERO;
	}

	Vector3d& norm(double d);
};

inline Vector3d operator +(const Vector3d& a, const Vector3d& b)
{
	return Vector3d(a.x + b.x, a.y + b.y, a.z + b.z);
}

inline Vector3d operator -(const Vector3d& a, const Vector3d& b)
{
	return Vector3d(a.x - b.x, a.y - b.y, a.z - b.z);
}

inline Vector3d operator *(const Vector3d& a, double d)
{
	return Vector3d(a.x * d, a.y * d, a.z * d);
}

inline Vector3d operator /(const Vector3d& a, double d)
{
	return Vector3d(a.x / d, a.y / d, a.z / d);
}

inline Vector3d operator *(double d, const Vector3d& a)
{
	return Vector3d(a.x * d, a.y * d, a.z * d);
}

inline double operator *(const Vector3d& a, const Vector3d& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline double qmod(const Vector3d& a)
{
	return a.x * a.x + a.y * a.y + a.z * a.z;
}

inline Vector3d operator %(const Vector3d& a, const Vector3d& b)
{
	return Vector3d(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x);
}

inline bool operator !=(const Vector3d& a, const Vector3d& b)
{
	return _nequal(a.x,b.x) || _nequal(a.y,b.y) || _nequal(a.z,b.z);
}

inline bool operator ==(const Vector3d& a, const Vector3d& b)
{
	return _equal(a.x,b.x) && _equal(a.y,b.y) && _equal(a.z,b.z);
}

double length(const Vector3d & v);

inline bool operator !=(const Segment3d & s1, const Segment3d & s2)
{
	return (s1.a != s2.a) || (s1.b != s2.b);
}

inline bool operator ==(const Segment3d & s1, const Segment3d & s2)
{
	return (s1.a == s2.a) && (s1.b == s2.b);
}

inline double length(const Segment3d & s)
{
	return length(s.b - s.a);
}

class Plane
{
public:
	Vector3d norm;
	double dist;

	Plane() :
		norm(Vector3d(0., 0., 0.));
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
};

inline bool operator ==(const Plane &p1, const Plane &p2)
{
	return p1.norm == p2.norm && _equal(p1.dist, p2.dist);
}

inline bool operator !=(const Plane &p1, const Plane &p2)
{
	return p1.norm != p2.norm || _nequal(p1.dist,p2.dist);
}

bool intersection(const Plane &plane1, const Plane &plane2,
		const Plane &plane3, Vector3d &point);

bool intersection(const Plane &plane1, const Plane &plane2, Vector3d &dir,
		Vector3d &point);

bool intersection(const Plane &plane, const Vector3d &dir,
		const Vector3d &point, Vector3d &res);

#endif /* VECTOR3D_H */

