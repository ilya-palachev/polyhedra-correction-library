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
 * @file Colouring.h
 * @brief Different procedures for the printing of coloured polyhedrons
 * (declaration).
 */

#ifndef COLOURING_H_
#define COLOURING_H_

#include "Polyhedron_3/Polyhedron_3.h"

/**
 * Prints the intersection of subspaces that correspond to the given planes
 * in such manner that facets that are contained in the same cluster are
 * coloured with the same (random) colour.
 *
 * @param planes		The vector of planes.
 * @param clustersIndices	The clusters of planes represented as a vector
 * 				of indices' vectors.
 * @param suffix		The suffix of output file.
 */
void printColouredIntersection(std::vector<Plane_3> planes,
							   std::vector<std::vector<int>> clustersIndices,
							   const char *suffix);

/**
 * Prints the intersection of subspaces that correspond to the given planes
 * in such manner that facets that are contained in the same cluster are
 * coloured with the same (random) colour.
 *
 * @param planes		The vector of planes.
 * @param clustersIndices	The clusters of planes represented as a vector
 * 				of indices' sets.
 * @param suffix		The suffix of output file.
 */
void printColouredIntersection(std::vector<Plane_3> planes,
							   std::vector<std::set<int>> clustersIndices,
							   const char *suffix);

/**
 * Prints the polyhedron in such manner that facets that are contained in the
 * same cluster are coloured with the same (random) colour.
 *
 * @param polyhedron		The polyhedron.
 * @param clustersIndices	The clusters of planes represented as a vector
 * 				of indices' sets.
 * @param suffix		The suffix of output file.
 */
void printColouredPolyhedron(Polyhedron_3 polyhedron,
							 std::vector<std::set<int>> clustersIndices,
							 const char *suffix);

/**
 * Prints the polyhedron in such manner that facets that are contained in the
 * cluster are coloured with the same (random) colour.
 *
 * @param polyhedron		The polyhedron.
 * @param clusterIndices	The cluster indices set.
 * @param suffix		The suffix of output file.
 */
void printColouredPolyhedron(Polyhedron_3 polyhedron,
							 std::set<int> clusterIndices, const char *suffix);

void printColouredPolyhedronAndLoadParaview(Polyhedron_3 polyhedron,
											std::set<int> clusterIndices);

void printColouredPolyhedronAndLoadParaview(Polyhedron_3 polyhedron,
											std::set<int> clusterIndices);

#endif /* COLOURING_H_ */
