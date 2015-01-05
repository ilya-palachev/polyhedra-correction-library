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
 * @file PolyhedronCGAL.h
 * @brief Inclusions from CGAL library.
 *
 * Based on official CGAL tutorial available at
 * http://doc.cgal.org/latest/Polyhedron/index.html#PolyhedronExtending
 *
 * TODO: They can dramatically increase build time. How to move them out from
 * here? Forward declarations are needed.
 */

#ifndef POLYHEDRONCGAL_H_
#define POLYHEDRONCGAL_H_

#include "KernelCGAL.h"
#include <CGAL/HalfedgeDS_vector.h>
#include <CGAL/point_generators_3.h>
#include <CGAL/algorithm.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/convex_hull_3.h>

#include "SparseMatrixEigen.h"
#include "DataContainers/SupportFunctionData/SupportFunctionData.h"

/** Define point creator */
typedef CGAL::Creator_uniform_3<double, Point_3> PointCreator;

/** A vertex type with an ID member variable. */
template <class Refs, class Point>
class VertexIndexed : public CGAL::HalfedgeDS_vertex_base<Refs, CGAL::Tag_true,
	Point>
{
public:
	long int id;

	VertexIndexed() :
		CGAL::HalfedgeDS_vertex_base<Refs, CGAL::Tag_true, Point>(),
		id(-1) {}

	VertexIndexed(const Point_3& point) :
		CGAL::HalfedgeDS_vertex_base<Refs, CGAL::Tag_true, Point>(point),
		id(-1) {}
};

/** A face type with an ID member variable. */
template <class Refs, class Plane>
class FaceIndexed : public CGAL::HalfedgeDS_face_base<Refs, CGAL::Tag_true,
	Plane>
{
public:
	long int id;

	FaceIndexed() :
		CGAL::HalfedgeDS_face_base<Refs, CGAL::Tag_true, Plane>(),
		id(-1) {}

	FaceIndexed(const Plane_3& plane) :
		CGAL::HalfedgeDS_face_base<Refs, CGAL::Tag_true, Plane>(plane),
		id(-1) {}
};

/** A halfedge type with an ID member variable. */
template <class Refs>
class HalfedgeIndexed : public CGAL::HalfedgeDS_halfedge_base<Refs>
{
public:
	long int id;
};

/** An items type using My_face and My_vertex. */
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


class Polyhedron_3 : public CGAL::Polyhedron_3<Kernel, ItemsIndexed,
	CGAL::HalfedgeDS_vector>
{
public:
	/**
	 * Gets the vector of vertices from the polyhedron.
	 *
	 * @return The vector of all vertices.
	 */
	std::vector<Vector_3> getVertices();

	/**
	 * Generates the support values corresponding to the given support
	 * directions.
	 *
	 * @param directions	Given support directions.
	 *
	 * @return Support values constructed for given polyhedron and
	 * directions.
	 */
	VectorXd calculateSupportValues(std::vector<Point_3> directions);

	/**
	 * Generates the support points corresponding to the given support
	 * directions.
	 *
	 * @param directions	Given support directions.
	 *
	 * @return Support points constructed for given polyhedron and
	 * directions.
	 */
	std::vector<Vector3d> calculateSupportPoints(std::vector<Point_3> directions);


	/**
	 * Generates the support data corresponding to the given support
	 * directions.
	 *
	 * @param directions	Given support directions.
	 *
	 * @return Support data constructed for given polyhedron and
	 * directions.
	 */
	SupportFunctionDataPtr calculateSupportData(std::vector<Point_3> directions);
};

#endif /* POLYHEDRONCGAL_H_ */
