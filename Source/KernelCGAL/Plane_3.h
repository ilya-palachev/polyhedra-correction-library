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
class Plane_3: public Kernel::Plane_3
{
public:
	/**
	 * Constructs CGAL plane from PCL plane.
	 *
	 * @param p	The PCL plane.
	 */
	Plane_3(Plane p): Kernel::Plane_3(p.norm.x, p.norm.y, p.norm.z, p.dist) {}

	/**
	 * Constructs CGAL plane from 4 values.
	 * A * x + B * y + C * z + D = 0.
	 *
	 * @param a	The A coefficient
	 * @param b	The B coefficient
	 * @param c	The C coefficient
	 * @param d	The D coefficient
	 */
	Plane_3(const double a, const double b, const double c,
			const double d): Kernel::Plane_3(a, b, c, d) {}
};

#endif /* KERNELCGAL_PLANE_3_H_ */