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
 * @file SideOfContour.h
 * @brief The declaration of shadow contour class.
 */

#ifndef SCONTOUR_H
#define	SCONTOUR_H

#include <cstdio>

#include <memory>

#include "Vector3d.h"
#include "Polyhedron/Polyhedron.h"
#include "DataContainers/ShadeContourData/SideOfContour/SideOfContour.h"

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
	shared_ptr<Polyhedron> poly;

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
};

#endif	/* SCONTOUR_H */

