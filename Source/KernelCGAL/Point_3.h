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
 * @file Point_3.h
 * @brief CGAL Point_3 class with PCL extensions;
 */

#ifndef KERNELCGAL_POINT_3_H_
#define KERNELCGAL_POINT_3_H_

#include "Vector3d.h"

/**
 * CGAL Point_3 class with PCL extensions
 */
class Point_3: public Kernel::Point_3
{
public:
	/**
	 * Constructs zero CGAL point from nothing.
	 */
	Point_3(): Kernel::Point_3() {}

	/**
	 * Constructs CGAL point from PCL vector.
	 *
	 * @param v	The PCL vector.
	 */
	Point_3(const Vector3d v): Kernel::Point_3(v.x, v.y, v.z) {}

	/**
	 * Constructs CGAL point from 3 coordinates.
	 *
	 * @param x	The X coordinate
	 * @param y	The Y coordinate
	 * @param z	The Z coordinate
	 */
	Point_3(const double x, const double y, const double z):
		Kernel::Point_3(x, y, z) {}

	/**
	 * Constructs CGAL vector from statement like "a + b" where a is CGAL
	 * point and b is CGAL vector
	 *
	 * TODO: It's too dirty hack. We'd avoid it.
	 *
	 * @param p	The result of "a + b"
	 */
	Point_3(CGAL::Type_equality_wrapper<
			CGAL::Cartesian_base_no_ref_count<
				double, CGAL::Epick>, 
				CGAL::Epick>::Point_3 p):
		Kernel::Point_3(p.x(), p.y(), p.z()) {}

	/**
	 * Calculates the squared length of the vector with source at the origin
	 * and with target at the point.
	 */
	double squared_length()
	{
		return (*this - CGAL::Origin()).squared_length();
	}
};

#endif /* KERNELCGAL_POINT_3_H_ */
