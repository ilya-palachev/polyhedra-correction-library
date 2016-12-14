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
 * @file FixedTopology.h
 * @brief Fixed topology used for the correction of polyhedrons.
 */

#ifndef FIXEDTOPOLOGY_H
#define FIXEDTOPOLOGY_H

#include "Polyhedron_3/Polyhedron_3.h"
#include "DataContainers/SupportFunctionData/SupportFunctionData.h"

/**
 * Describes the topology of the given (polyhedron, s-data) pair.
 */
struct FixedTopology
{
	/**
	 * Vector of sets, i-th element of which contains IDs of support
	 * directions for which i-th vertex of the polyhedron is tangient.
	 */
	std::vector<std::set<int>> tangient;

	/**
	 * Vector of sets, i-th element of which contains IDs of vertices
	 * incident to the i-th facet.
	 */
	std::vector<std::set<int>> incident;

	/**
	 * Vector of sets, i-th element of which contains IDs of vertices
	 * incident to the neighbor facets of the i-th facet.
	 */
	std::vector<std::set<int>> influent;

	/**
	 * Vector of sets, i-th element of which contains IDs of vertices
	 * incident to some edge that is incident to the i-th vertex.
	 */
	std::vector<std::set<int>> neighbors;

	FixedTopology(Polyhedron_3 initialP, SupportFunctionDataPtr SData);
};

#endif /* FIXEDTOPOLOGY_H */
