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
 * @file SideOfContour.h
 * @brief The declaration of shadow contour class.
 */

#ifndef SCONTOUR_H
#define	SCONTOUR_H

#include <cstdio>
#include <memory>
#include <vector>

#include <CGAL/basic.h>
#include <CGAL/Filtered_kernel.h>

#include "PCLKernel/PCLKernel.h"

#include "Constants.h"
#include "Vector3d.h"
#include "Polyhedron/Polyhedron.h"
#include "DataContainers/ShadowContourData/SideOfContour/SideOfContour.h"

typedef PCLKernel<double> PCL_K;
typedef CGAL::Filtered_kernel_adaptor<PCL_K> K;

typedef K::Point_2 Point_2;

/**
 * The shadow contour class.
 */
class SContour
{
public:
	/**
	 * The ID of the contour in the array of contours (inside shadow
	 * contour data)
	 */
	int id;

	/** The number of sides in contour */
	int ns;

	/**
	 * The plane of contour.
	 * Usually it includes (0, 0, 0), so in its equation
	 * a*x + b*y + c*z + d = 0
	 * coefficient d = 0
	 */
	Plane plane;

	/**
	 * Pointer to parent polyhedon which given contour from which the
	 * contour was generated (for synthetically generated contours).
	 */
	PolyhedronPtr poly;

	/** The array of contour sides. */
	SideOfContour* sides;

	/* Implemented in file SContour.cpp:*/

	/** Empty contructor. */
	SContour();

	/**
	 * Copy constructor. Creates contour as the copy of another one.
	 *
	 * @param orig	The original contour.
	 */
	SContour(const SContour& orig);

	/**
	 * Creates shadow contour from the std::vector of its points and from its
	 * plane
	 */
	SContour(std::vector<Point_3> points, Plane plane);

	/** The destructor. */
	~SContour();

	/**
	 * The "=" operator, used for copying of shadow contours.
	 *
	 * @param scontour  The original contour.
	 */
	SContour& operator =(const SContour& scontour);

	/**
	 * The equality operator.
	 *
	 * @param scontour  The second contour.
	 */
	bool operator ==(const SContour& scontour) const;

	/**
	 * The inequality operator.
	 *
	 * @param scontour  The second contour.
	 */
	bool operator !=(const SContour& scontour) const;

	/**
	 * Convexifies the contour and returns the convexified version of it.
	 */
	SContour *convexify();

	/* Implemented in file SContour_io.cpp : */

	/**
	 * Dumps internal representation of contour to the file descriptor.
	 *
	 * @param file	The file descriptor
	 */
	void my_fprint(FILE* file);

	/**
	 * Dumps standard output form of contour to the file descriptor.
	 *
	 * @param file	The file descriptor
	 */
	void fprintDefault(FILE* file);

	/**
	 * Gets the std::vector of all points of the contour (all non-equal
	 * points).
	 */
	std::vector<Vector3d> getPoints();
};

#endif	/* SCONTOUR_H */

