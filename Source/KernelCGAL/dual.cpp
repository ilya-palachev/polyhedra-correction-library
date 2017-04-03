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
 * @file dual.cpp
 * @brief Transformation of duality between planes and points (implementation)
 */

#include "KernelCGAL/dual.h"

Point_3 dual(const Plane_3 &p)
{
	double reciprocal = -1. / p.d();
	return Point_3(p.a() * reciprocal, p.b() * reciprocal,
			p.c() * reciprocal);
}

Plane_3 dual(const Point_3 &p)
{
	return Plane_3(p.x(), p.y(), p.z(), -1.);
}
