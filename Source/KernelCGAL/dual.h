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
 * @file dual.h
 * @brief Transformation of duality between planes and points.
 */

#ifndef KERNELCGAL_DUAL_H
#define KERNELCGAL_DUAL_H

#include "KernelCGAL/KernelCGAL.h"

/**
 * Creates a dual point for a given plane.
 *
 * @param p	The plane.
 * @return	The dual point.
 */
Point_3 dual(Plane_3 p);

/**
 * Calculates dual plane for a given point.
 *
 * @param p	The point.
 * @return	The dual plane.
 */
Plane_3 dual(Point_3 p);

#endif /* KERNELCGAL_DUAL_H */
