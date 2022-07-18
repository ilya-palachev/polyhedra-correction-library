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
typedef Plane_3 PCLPlane_3;

#include <CGAL/point_generators_3.h>
#include <CGAL/algorithm.h>
#include <CGAL/convex_hull_3.h>
#include <CGAL/Polyhedron_3.h>

#include "KernelCGAL/ItemsIndexed.h"
#include "SparseMatrixEigen.h"
#include "DataContainers/SupportFunctionData/SupportFunctionData.h"
#include "Polyhedron/Polyhedron.h"

typedef struct
{
	unsigned char red;
	unsigned char green;
	unsigned char blue;
} Colour;

/** Define point creator */
typedef CGAL::Creator_uniform_3<double, Point_3> PointCreator;

template <class KernelT, class ItemsIndexedT> class BasePolyhedron_3 : public CGAL::Polyhedron_3<KernelT, ItemsIndexedT>
{
public:
	/** Whether to print "edge-colouring facets" in the PLY output. */
	bool whetherPrintEdgeColouringFacets;

	/** Facet colours. */
	std::vector<Colour> facetColours;

	/** Halfedge colours.  */
	std::vector<Colour> halfedgeColours;

	/**
	 * Constructs empty CGAL polyhedron from nothing.
	 */
	BasePolyhedron_3() : CGAL::Polyhedron_3<KernelT, ItemsIndexedT>()
	{
	}

	/**
	 * Constructs CGAL polyhedron from PCL polyhedron.
	 *
	 * @param p	The PCL polyhedron.
	 */
	BasePolyhedron_3(Polyhedron p);

	/**
	 * Constructs CGAL polyhedron as an intersection of halfspaces.
	 *
	 * @param planesBegin	The iterator pointing to the beginning of
	 * 			planes container.
	 * @param planesEnd	The iterator pointing to the end of
	 * 			planes container.
	 */
	template <class PlaneIterator> BasePolyhedron_3(PlaneIterator planesBegin, PlaneIterator planesEnd);

	/**
	 * Constructs CGAL polyhedron as an intersection of halfspaces.
	 *
	 * @param planes	The vector of planes that correspond to
	 * 			halfspaces.
	 */
	BasePolyhedron_3(std::vector<PCLPlane_3> planes) : BasePolyhedron_3(planes.begin(), planes.end())
	{
	}

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
	VectorXd calculateSupportValues(std::vector<PCLPoint_3> directions);

	/**
	 * Generates the support points corresponding to the given support
	 * directions.
	 *
	 * @param directions	Given support directions.
	 *
	 * @return Support points constructed for given polyhedron and
	 * directions.
	 */
	std::vector<Vector3d> calculateSupportPoints(std::vector<PCLPoint_3> directions);

	/**
	 * Generates the support data corresponding to the given support
	 * directions.
	 *
	 * @param directions	Given support directions.
	 *
	 * @return Support data constructed for given polyhedron and
	 * directions.
	 */
	SupportFunctionDataPtr calculateSupportData(std::vector<PCLPoint_3> directions);

	/**
	 * Initializes the indices of the polyhedron.
	 */
	void initialize_indices();

	/**
	 * Calculates the tangient points and support value for the given
	 * direction.
	 *
	 * @param direction	The 3D direction.
	 * @return		The pair tangient point and direction.
	 */
	template <class VertexIteratorT> std::pair<VertexIteratorT, double> findTangientVertex(PCLPoint_3 direction);

	/**
	 * Calculates the concatenation of tangient points coordinates for
	 * the given directions.
	 *
	 * @param directions	The 3D directions.
	 * @return		The support values.
	 */
	VectorXd findTangientPointsConcatenated(std::vector<PCLPoint_3> directions);

	/**
	 * The indices of planes in the given polyhedron.
	 */
	std::vector<int> indexPlanes_;

	/**
	 * Initializes the indices of the polyhedron by the indices of given
	 * planes.
	 *
	 * @param planesBegin	The iterator pointing to the beginning of
	 * 			planes container.
	 * @param planesEnd	The iterator pointing to the end of
	 * 			planes container.
	 */
	template <class PlaneIterator> void initialize_indices(PlaneIterator planesBegin, PlaneIterator planesEnd);

	/**
	 * Initializes the indices of the polyhedron by the indices of given
	 * vector of planes.
	 *
	 * @param planes	The vector of planes from which indices should
	 * 			be initialized from.
	 */
	void initialize_indices(std::vector<PCLPlane_3> planes)
	{
		initialize_indices(planes.begin(), planes.end());
	}

	static BasePolyhedron_3 dual(const BasePolyhedron_3 &p);
};

/**
 * Finds the ID of best plane corresponding to the given facet.
 *
 * @param facet			The facet.
 * @param planesOriginal	The vector of original planes.
 */
int findBestPlaneOriginal(BasePolyhedron_3<Kernel, ItemsIndexed>::Facet &facet, std::vector<PCLPlane_3> planesOriginal);

#define CGAL_GRAPH_TRAITS_INHERITANCE_TEMPLATE_PARAMS typename KernelT, typename ItemsIndexedT
#define CGAL_GRAPH_TRAITS_INHERITANCE_CLASS_NAME BasePolyhedron_3<KernelT, ItemsIndexedT>
#define CGAL_GRAPH_TRAITS_INHERITANCE_BASE_CLASS_NAME CGAL::Polyhedron_3<KernelT, ItemsIndexedT>
#include <CGAL/boost/graph/graph_traits_inheritance_macros.h>
typedef BasePolyhedron_3<Kernel, ItemsIndexed> Polyhedron_3;

/**
 * Writes polyhedron to stream in PLY format, by converting it to PCL
 * polyhedron and then printing it in PLY format.
 *
 * @param stream	Output stream
 * @param p		The polyhedron
 *
 * @return		The stream ready for further outputs
 */
std::ostream &operator<<(std::ostream &stream, Polyhedron_3 &p);

#endif /* POLYHEDRONCGAL_H_ */
