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
 * @file VertexIndexed.h
 * @brief Extension of CGAL vertex with added index.
 */

#ifndef VERTEXINDEXED_H_
#define VERTEXINDEXED_H_

#include "KernelCGAL/Point_3.h"

/** A vertex type with an ID member variable. */
template <class Refs, class Point>
class VertexIndexed : public CGAL::HalfedgeDS_vertex_base<Refs, CGAL::Tag_true,
	Point>
{
public:
	long int id;

	VertexIndexed():
		CGAL::HalfedgeDS_vertex_base<Refs, CGAL::Tag_true, Point>(),
		id(-1) {}

	VertexIndexed(const Point_3 &point):
		CGAL::HalfedgeDS_vertex_base<Refs, CGAL::Tag_true, Point>(point),
		id(-1) {}

	VertexIndexed(Kernel::Point_3 point):
		CGAL::HalfedgeDS_vertex_base<Refs, CGAL::Tag_true, Point>(point),
		id(-1) {}
};

#endif /* VERTEXINDEXED_H_ */
