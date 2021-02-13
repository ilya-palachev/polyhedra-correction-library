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
 * @file Vector_3.h
 * @brief CGAL Vector_3 class with PCL extensions;
 */

#ifndef KERNELCGAL_VECTOR_3_H_
#define KERNELCGAL_VECTOR_3_H_

#include "Vector3d.h"

/**
 * CGAL Vector_3 class with PCL extensions
 */
class Vector_3 : public Kernel::Vector_3
{
public:
	/**
	 * Constructs zero CGAL vector from nothing.
	 */
	Vector_3() : Kernel::Vector_3()
	{
	}

	/**
	 * Constructs CGAL vector from PCL vector.
	 *
	 * @param v	The PCL vector.
	 */
	Vector_3(const Vector3d &v) : Kernel::Vector_3(v.x, v.y, v.z)
	{
	}

	/**
	 * Constructs CGAL vector from 3 coordinates.
	 *
	 * @param x	The X coordinate
	 * @param y	The Y coordinate
	 * @param z	The Z coordinate
	 */
	Vector_3(double x, double y, double z) : Kernel::Vector_3(x, y, z)
	{
	}

	/**
	 * Constructs vector from another vector.
	 *
	 * @param v	The original vector
	 */
	Vector_3(const Kernel::Vector_3 &v) : Kernel::Vector_3(v)
	{
	}
};

#endif /* KERNELCGAL_VECTOR_3_H_ */
