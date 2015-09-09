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
 * @file Polyhedron_3.h
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

#include "KernelCGAL/KernelCGAL.h"
typedef Point_3 PCLPoint_3;

#include <CGAL/point_generators_3.h>
#include <CGAL/algorithm.h>
#include <CGAL/convex_hull_3.h>

#include "SparseMatrixEigen.h"
#include "DataContainers/SupportFunctionData/SupportFunctionData.h"
#include "Polyhedron/Polyhedron.h"

/** Define point creator */
typedef CGAL::Creator_uniform_3<double, Point_3> PointCreator;

class Polyhedron_3 : public BasePolyhedron_3
{
public:
	/**
	 * Constructs empty CGAL polyhedron from nothing.
	 */
	Polyhedron_3(): BasePolyhedron_3() {}

	/**
	 * Constructs CGAL polyhedron from PCL polyhedron.
	 *
	 * @param p	The PCL polyhedron.
	 */
	Polyhedron_3(Polyhedron p);

	/**
	 * Shifts all vertices of the polyhedron on the given vector
	 * and recalculates the equation of its plane.
	 */
	void shift(Vector_3 vector);

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
	VectorXd calculateSupportValues(
			std::vector<PCLPoint_3> directions);

	/**
	 * Generates the support points corresponding to the given support
	 * directions.
	 *
	 * @param directions	Given support directions.
	 *
	 * @return Support points constructed for given polyhedron and
	 * directions.
	 */
	std::vector<Vector3d> calculateSupportPoints(
			std::vector<PCLPoint_3> directions);


	/**
	 * Generates the support data corresponding to the given support
	 * directions.
	 *
	 * @param directions	Given support directions.
	 *
	 * @return Support data constructed for given polyhedron and
	 * directions.
	 */
	SupportFunctionDataPtr calculateSupportData(
			std::vector<PCLPoint_3> directions);

	/**
	 * Writes polyhedron to stream in PLY format, by converting it to PCL
	 * polyhedron and then printing it in PLY format.
	 *
	 * @param stream	Output stream
	 * @param p		The polyhedron
	 *
	 * @return		The stream ready for further outputs
	 */
	friend std::ostream &operator<<(std::ostream &stream, Polyhedron_3 &p);

	/**
	 * Initializes the indices of the polyhedron.
	 */
	void initialize_indices();
};

#endif /* POLYHEDRONCGAL_H_ */
