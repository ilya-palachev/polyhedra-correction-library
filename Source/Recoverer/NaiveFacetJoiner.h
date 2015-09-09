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
 * @file NaiveFacetJoiner.h
 * @brief Naive facet joiner that joins facets based on the angle between their
 * planes (declaration).
 */

#ifndef NAIVEFACETJOINER_H_
#define NAIVEFACETJOINER_H_
#include "halfspaces_intersection.h"
#include "Polyhedron_3/Polyhedron_3.h"
#include <CGAL/linear_least_squares_fitting_3.h>


class NaiveFacetJoiner
{
public:
	/**
	 * The default constructor.
	 *
	 * @param threshold	The value of threshold.
	 */
	NaiveFacetJoiner(double threshold);

	/**
	 * The empty destructor.
	 */
	~NaiveFacetJoiner();

	/**
	 * Runs the join procudure.
	 *
	 * @param polyhedron	The input polyhedron.
	 * @return		The processed polyhedron.
	 */
	Polyhedron_3 run(Polyhedron_3 polyhedron);
};

#endif /* NAIVEFACETJOINER_H_ */
