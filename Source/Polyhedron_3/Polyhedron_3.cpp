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
 * @file PolyhedronCGAL.cpp
 * @brief Inclusions from CGAL library (implementation of additional features).
 */

#include "Polyhedron_3/Polyhedron_3.h"
#include "DebugPrint.h"
#include "DataConstructors/SupportFunctionDataConstructor/SupportFunctionDataConstructor.h"
#include "Polyhedron/Polyhedron.h"
#include "Polyhedron/Facet/Facet.h"

#include <CGAL/Polyhedron_incremental_builder_3.h>

/**
 * Builds CGAL polyhedron from PCL polyhedron.
 *
 * Based on example described at
 * http://doc.cgal.org/latest/Polyhedron/classCGAL_1_1Polyhedron__incremental__builder__3.html
 */
template <class HDS>
class BuilderFromPCLPolyhedron: public CGAL::Modifier_base<HDS>
{
public:
	/** The PCL polyhedron. */
	const Polyhedron polyhedron;

	/**
	 * Constructs the builder.
	 *
	 * @param p	The PCL polyhedron
	 */
	BuilderFromPCLPolyhedron(const Polyhedron p): polyhedron(p) {}

	/**
	 * Builds CGAL polyhedron from PCL polyhedron.
	 *
	 * @param hds	TODO: I don't understand what it means...
	 */
	void operator()(HDS& hds)
	{
		CGAL::Polyhedron_incremental_builder_3<HDS> builder(hds, true);

		/* Count edges of polyhedron. */
		int numHalfedges = 0;
		for (int i = 0; i < polyhedron.numFacets; ++i)
			numHalfedges += polyhedron.facets[i].numVertices;
		
		/* TODO: Add assertion of Euler's rule. */

		builder.begin_surface(polyhedron.numVertices,
				polyhedron.numFacets, numHalfedges);
		typedef typename HDS::Vertex Vertex;
		typedef typename Vertex::Point Point;

		for (int i = 0; i < polyhedron.numVertices; ++i)
		{
			Vector3d vertice = polyhedron.vertices[i];
			builder.add_vertex(Point(vertice.x, vertice.y,
						vertice.z));
		}

		for (int i = 0; i < polyhedron.numFacets; ++i)
		{
			Facet *facet = &polyhedron.facets[i];
			builder.begin_facet();
			for (int j = 0; j < facet->numVertices; ++j)
			{
				builder.add_vertex_to_facet(
						facet->indVertices[j]);
			}
			builder.end_facet();
		}

		builder.end_surface();
	}
};

Polyhedron_3::Polyhedron_3(Polyhedron p)
{
	DEBUG_START;
	BuilderFromPCLPolyhedron<HalfedgeDS> builder(p);
	this->delegate(builder);
	DEBUG_END;
}

std::vector<Vector_3> Polyhedron_3::getVertices()
{
	DEBUG_START;
	std::vector<Vector_3> vertices;
	for (auto v = vertices_begin(); v != vertices_end(); ++v)
	{
		vertices.push_back(v->point() - CGAL::ORIGIN);
	}
	DEBUG_END;
	return vertices;
}


VectorXd Polyhedron_3::calculateSupportValues(
	std::vector<PCLPoint_3> directions)
{
	DEBUG_START;
	SupportFunctionDataPtr data = calculateSupportData(directions);
	VectorXd values = data->supportValues();
	DEBUG_END;
	return values;
}

std::vector<Vector3d> Polyhedron_3::calculateSupportPoints(
	std::vector<PCLPoint_3> directions)
{
	DEBUG_START;
	SupportFunctionDataPtr data = calculateSupportData(directions);
	std::vector<Vector3d> points = data->supportPoints();
	DEBUG_END;
	return points;
}

SupportFunctionDataPtr Polyhedron_3::calculateSupportData(
	std::vector<PCLPoint_3> directions)
{
	DEBUG_START;
	SupportFunctionDataConstructor ctor;
	SupportFunctionDataPtr data = ctor.run(directions, *this);
	DEBUG_END;
	return data;
}

std::ostream &operator<<(std::ostream &stream, Polyhedron_3 &p)
{
	DEBUG_START;
	PolyhedronPtr polyhedronPCL(new Polyhedron(p));
	stream << polyhedronPCL;
	DEBUG_END;
	return stream;
}
