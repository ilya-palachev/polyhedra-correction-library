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
 * @file ItemsIndexed.h
 * @brief Extension of CGAL::Polyhedron_items_3 with indexed types
 */

#ifndef ITEMSINDEXED_H_
#define ITEMSINDEXED_H_

#include <CGAL/Polyhedron_items_3.h>

#include "KernelCGAL/VertexIndexed.h"
#include "KernelCGAL/FaceIndexed.h"
#include "KernelCGAL/HalfedgeIndexed.h"

/** An items type using VertexIndexed, FaceIndexed, HalfedgeIndexed. */
struct ItemsIndexed : public CGAL::Polyhedron_items_3
{

	template <class Refs, class Traits>
	struct Vertex_wrapper {
		typedef typename Traits::Point_3 Point;
		typedef VertexIndexed<Refs, Point> Vertex;
	};

	template < class Refs, class Traits>
	struct Halfedge_wrapper {
		typedef HalfedgeIndexed<Refs> Halfedge;
	};

	template <class Refs, class Traits>
	struct Face_wrapper {
		typedef typename Traits::Plane_3 Plane;
		typedef FaceIndexed<Refs, Plane> Face;
	};
};

#endif /* ITEMSINDEXED_H_ */
