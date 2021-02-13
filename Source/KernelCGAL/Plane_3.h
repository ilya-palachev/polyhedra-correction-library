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
 * @file Plane_3.h
 * @brief CGAL Plane_3 class with PCL extensions;
 */

#ifndef KERNELCGAL_PLANE_3_H_
#define KERNELCGAL_PLANE_3_H_

#include "Vector3d.h"

/**
 * CGAL Plane_3 class with PCL extensions
 */
class Plane_3 : public Kernel::Plane_3
{
public:
	/**
	 * Constructs the empty plane (default empty constructor).
	 */
	Plane_3() : Kernel::Plane_3(0., 0., 0., 0.)
	{
	}

	/**
	 * Constructs CGAL plane from PCL plane.
	 *
	 * @param p	The PCL plane.
	 */
	Plane_3(const Plane &p) :
		Kernel::Plane_3(p.norm.x, p.norm.y, p.norm.z, p.dist)
	{
	}

	/**
	 * Constructs the plane from general CGAL plane.
	 *
	 * @param p	The CGAL plane
	 */
	Plane_3(const Kernel::Plane_3 &p) : Kernel::Plane_3(p)
	{
	}

	/**
	 * Constructs CGAL plane from 4 values.
	 * A * x + B * y + C * z + D = 0.
	 *
	 * @param a	The A coefficient
	 * @param b	The B coefficient
	 * @param c	The C coefficient
	 * @param d	The D coefficient
	 */
	Plane_3(double a, double b, double c, double d) :
		Kernel::Plane_3(a, b, c, d)
	{
	}

	/**
	 * Constructs CGAL plane from three points.
	 *
	 * @param a	The 1st point
	 * @param b	The 2nd point
	 * @param c	The 3rd point
	 */
	Plane_3(const Kernel::Point_3 &a, const Kernel::Point_3 &b,
			const Kernel::Point_3 &c) :
		Kernel::Plane_3(a, b, c)
	{
	}
};

#endif /* KERNELCGAL_PLANE_3_H_ */
