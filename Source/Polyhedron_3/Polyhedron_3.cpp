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
#include "DebugAssert.h"
#include "DataConstructors/SupportFunctionDataConstructor/SupportFunctionDataConstructor.h"
#include "Polyhedron/Polyhedron.h"
#include "Polyhedron/Facet/Facet.h"

#include <CGAL/Polyhedron_incremental_builder_3.h>

/**
 * A functor computing the plane containing a triangular facet
 */
struct Plane_from_facet
{
	Polyhedron_3::Plane_3 operator()(Polyhedron_3::Facet& f)
	{
		Polyhedron_3::Halfedge_handle h = f.halfedge();
		return Polyhedron_3::Plane_3(h->vertex()->point(),
				h->next()->vertex()->point(), h->opposite()->vertex()->point());
	}
};

/**
 * Builds CGAL polyhedron from PCL polyhedron->
 *
 * Based on example described at
 * http://doc.cgal.org/latest/Polyhedron/classCGAL_1_1Polyhedron__incremental__builder__3.html
 */
template <class HDS>
class BuilderFromPCLPolyhedron: public CGAL::Modifier_base<HDS>
{
public:
	/** The PCL polyhedron-> */
	Polyhedron *polyhedron;

	/**
	 * Constructs the builder.
	 *
	 * @param p	The PCL polyhedron
	 */
	BuilderFromPCLPolyhedron(Polyhedron *p): polyhedron(p)
	{
		DEBUG_START;
		DEBUG_END;
	}

	/**
	 * Builds CGAL polyhedron from PCL polyhedron->
	 *
	 * @param hds	TODO: I don't understand what it means...
	 */
	void operator()(HDS& hds)
	{
		DEBUG_START;
		CGAL::Polyhedron_incremental_builder_3<HDS> builder(hds, true);

		ASSERT(polyhedron->numVertices > 0);
		ASSERT(polyhedron->numFacets > 0);

		/* Count edges of polyhedron-> */
		int numHalfedges = 0;
		for (int i = 0; i < polyhedron->numFacets; ++i)
			numHalfedges += polyhedron->facets[i].numVertices;
		ASSERT(numHalfedges > 0);
		
		/* TODO: Add assertion of Euler's rule. */

		builder.begin_surface(polyhedron->numVertices,
				polyhedron->numFacets, numHalfedges);
		typedef typename HDS::Vertex Vertex;
		typedef typename Vertex::Point Point;

		for (int i = 0; i < polyhedron->numVertices; ++i)
		{
			Vector3d vertice = polyhedron->vertices[i];
			builder.add_vertex(Point(vertice.x, vertice.y,
						vertice.z));
		}

		for (int i = 0; i < polyhedron->numFacets; ++i)
		{
			Facet *facet = &polyhedron->facets[i];
			DEBUG_PRINT("Constructing facet #%d, number = %d", i,
					facet->numVertices);
			if (facet->numVertices <= 0)
				continue;
			builder.begin_facet();
			for (int j = 0; j < facet->numVertices; ++j)
			{
				int ind = facet->indVertices[j];
				ASSERT(ind >= 0);
				ASSERT(ind < polyhedron->numVertices);
				builder.add_vertex_to_facet(ind);
			}
			builder.end_facet();
		}

		builder.end_surface();
		DEBUG_END;
	}
};

Polyhedron_3::Polyhedron_3(Polyhedron p)
{
	DEBUG_START;

	Polyhedron *copy = new Polyhedron(p);
	BuilderFromPCLPolyhedron<HalfedgeDS> builder(copy);
	this->delegate(builder);
	
	/*
	 * assign a plane equation to each polyhedron facet using functor
	 * Plane_from_facet
	 */
	std::transform(facets_begin(), facets_end(), planes_begin(),
		Plane_from_facet());

	ASSERT(size_of_vertices() == (unsigned) p.numVertices);
	ASSERT(size_of_facets() == (unsigned) p.numFacets);
	ASSERT(size_of_vertices() > 0);
	ASSERT(size_of_facets() > 0);
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
