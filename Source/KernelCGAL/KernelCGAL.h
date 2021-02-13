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
 * @file KernelCGAL.h
 * @brief Kernel typedefs from CGAL library.
 */

#ifndef KERNELCGAL_H_
#define KERNELCGAL_H_

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;

#include "KernelCGAL/Vector_3.h"
#include "KernelCGAL/Point_3.h"
#include "KernelCGAL/Plane_3.h"

typedef Kernel::Segment_3 Segment_3;
typedef Kernel::Line_3 Line_3;
#include "dual.h"

#endif /* KERNELCGAL_H_ */
