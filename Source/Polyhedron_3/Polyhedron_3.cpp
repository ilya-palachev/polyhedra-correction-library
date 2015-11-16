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
 * @file PolyhedronCGAL.cpp
 * @brief Inclusions from CGAL library (implementation of additional features).
 */

#include "Polyhedron_3/Polyhedron_3.h"
#include "DebugPrint.h"
#include "DebugAssert.h"
#include "halfspaces_intersection.h"
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

void Polyhedron_3::shift(Vector_3 vector)
{
	for (auto v = vertices_begin(); v != vertices_end(); ++v)
	{
		v->point() = v->point() + vector;
	}
	std::transform(facets_begin(), facets_end(), planes_begin(),
		Plane_from_facet());
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
	if (p.facetColours.size() != p.size_of_facets()
		|| p.halfedgeColours.size() != p.size_of_halfedges())
	{
		p.initialize_indices();
	}
	int numVertices = p.size_of_vertices();
	int numFacets = p.size_of_facets();
	if (p.whetherPrintEdgeColouringFacets)
	{
		numVertices = 2 * p.size_of_vertices();
		numFacets = p.size_of_facets() + p.size_of_halfedges() / 2;
	}

	stream << "ply" << std::endl;
	stream << "format ascii 1.0" << std::endl;
	stream << "comment generated by Polyhedra Correction Library"
		<< std::endl;
	stream << "element vertex " << numVertices << std::endl;
	stream << "property float x" << std::endl;
	stream << "property float y" << std::endl;
	stream << "property float z" << std::endl;
	stream << "element face " << numFacets << std::endl;
	stream << "property list uchar uint vertex_indices" << std::endl;
	stream << "property uchar red" << std::endl;
	stream << "property uchar green" << std::endl;
	stream << "property uchar blue" << std::endl;
	stream << "end_header" << std::endl;
	int iVertex = 0;
	double scale = 1e+6;
	double factor = 1.05;
	for (auto vertex = p.vertices_begin(); vertex != p.vertices_end();
			++vertex)
	{
		auto point = vertex->point();
		stream << (int) std::floor(scale * point.x()) << " "
			<< (int) std::floor(scale * point.y()) << " "
			<< (int) std::floor(scale * point.z());
		stream << std::endl;
		if (p.whetherPrintEdgeColouringFacets)
		{
			point = Point_3(point.x() * factor, point.y() * factor,
					point.z() * factor);
			stream << (int) std::floor(scale * point.x()) << " "
				<< (int) std::floor(scale * point.y()) << " "
				<< (int) std::floor(scale * point.z());
			stream << std::endl;
		}
		++iVertex;
	}
	int iFacet = 0;
	for (auto facet = p.facets_begin(); facet != p.facets_end(); ++facet)
	{
		auto colour = p.facetColours[iFacet];
		stream << facet->facet_degree();
		auto halfedge = facet->facet_begin();
		do
		{
			int iVertex = halfedge->vertex()->id;
			if (p.whetherPrintEdgeColouringFacets)
				iVertex *= 2;;
			stream << " " << iVertex;
			++halfedge;
		} while (halfedge != facet->facet_begin());
		stream << " " << static_cast<int>(colour.red)
			<< " " << static_cast<int>(colour.green)
			<< " " << static_cast<int>(colour.blue) << std::endl;
		++iFacet;
	}
	if (p.whetherPrintEdgeColouringFacets)
	{
		int iHalfedge = 0;
		for (auto halfedge = p.halfedges_begin();
				halfedge != p.halfedges_end(); ++halfedge)
		{
			int iVertex1 = halfedge->vertex()->id;
			int iVertex2 = halfedge->opposite()->vertex()->id;
			if (iVertex1 < iVertex2)
			{
				auto colour = p.halfedgeColours[iHalfedge];
				stream << 4 << " " << 2 * iVertex1 << " "
					<< 2 * iVertex1 + 1
					<< " " << 2 * iVertex2 + 1 << " "
					<< 2 * iVertex2;
				stream << " " << static_cast<int>(colour.red)
					<< " " << static_cast<int>(colour.green)
					<< " " << static_cast<int>(colour.blue)
					<< std::endl;
			}
			++iHalfedge;
		}
	}
	DEBUG_END;
	return stream;
}

void Polyhedron_3::initialize_indices()
{
	DEBUG_START;
	std::cerr << "Inidices are initialized!" << std::endl;
	whetherPrintEdgeColouringFacets = false;
	facetColours = std::vector<Colour>(size_of_facets());
	halfedgeColours = std::vector<Colour>(size_of_halfedges());
	Colour grey;
	grey.red = 100;
	grey.green = 100;
	grey.blue = 100;

	auto vertex = vertices_begin();
	for (int i = 0; i < (int) size_of_vertices(); ++i)
	{
		vertex->id = i;
		++vertex;
	}

	auto facet = facets_begin();
	for (int i = 0; i < (int) size_of_facets(); ++i)
	{
		facet->id = i;
		facetColours[i] = grey;
		++facet;
	}

	auto halfedge = halfedges_begin();
	for (int i = 0; i < (int) size_of_halfedges(); ++i)
	{
		halfedge->id = i;
		halfedgeColours[i] = grey;
		++halfedge;
	}
	DEBUG_END;
}

std::pair<Polyhedron_3::Vertex_iterator, double>
Polyhedron_3::findTangientVertex(PCLPoint_3 direction)
{
	DEBUG_START;
	double maximum  = -1e100;
	auto tangient = vertices_end();
	for (auto vertex = vertices_begin(); vertex != vertices_end(); ++vertex)
	{
		if (vertex->degree() < 3)
		{
			std::cerr << "warning degree of " << vertex->id
				<< " is " << vertex->degree() << std::endl;
			continue;
		}
		auto point = vertex->point();
		double product = direction * (point - CGAL::ORIGIN);
		if (product > maximum)
		{
			maximum = product;
			tangient = vertex;
		}
	}
	DEBUG_END;
	return std::make_pair(tangient, maximum);
}

VectorXd Polyhedron_3::findTangientPointsConcatenated(
		std::vector<PCLPoint_3> directions)
{
	DEBUG_START;
	VectorXd solution(3 * directions.size());
	for (int i = 0; i < (int) directions.size(); ++i)
	{
		 auto direction = directions[i];
		 auto pair = findTangientVertex(direction);
		 auto vertex = pair.first;
		 auto point = vertex->point();
		 solution(3 * i) = point.x();
		 solution(3 * i + 1) = point.y();
		 solution(3 * i + 2) = point.z();
	}

	DEBUG_END;
	return solution;
}

PCLPlane_3 normalize(PCLPlane_3 plane)
{
	double a = plane.a();
	double b = plane.b();
	double c = plane.c();
	double d = plane.d();
	double norm = sqrt(a * a + b * b + c * c);
	a /= norm;
	b /= norm;
	c /= norm;
	d /= norm;
	return PCLPlane_3(a, b, c, d);
}

double distance(PCLPoint_3 point, PCLPlane_3 planeGiven)
{
	DEBUG_START;
	PCLPlane_3 plane = normalize(planeGiven);
	double value = plane.a() * point.x() + plane.b() * point.y()
		+ plane.c() * point.z() + plane.d();
	DEBUG_END;
	return sqrt(value * value);
}

double distanceSum(Polyhedron_3::Facet& facet, PCLPlane_3 plane)
{
	DEBUG_START;
	auto halfedgeBegin = facet.facet_begin();
	auto halfedge = halfedgeBegin;
	double errorSum = 0.;
	do
	{
		auto point = halfedge->vertex()->point();
		double error = distance(point, plane);
		errorSum += error;
		++halfedge;
	}
	while (halfedge != halfedgeBegin);

	DEBUG_END;
	return errorSum;
}

int findBestPlaneOriginal(Polyhedron_3::Facet& facet,
		std::vector<PCLPlane_3> planes)
{
	DEBUG_START;
	double minimum = 1e100;
	int iPlaneBest = -1;
	for (int i = 0; i < (int) planes.size(); ++i)
	{
		PCLPlane_3 plane = planes[i];
		double errorSum = distanceSum(facet, plane);
		if (errorSum < minimum)
		{
			iPlaneBest = i;
			minimum = errorSum;
		}
	}
	if (iPlaneBest < 0)
	{
		ERROR_PRINT("Failed to find best plane");
		exit(EXIT_FAILURE);
	}
	DEBUG_END;
	return iPlaneBest;
}

struct Plane_from_planes
{
	std::vector<Plane_3> planesOriginal;

	Plane_from_planes(std::vector<Plane_3> planes):
		planesOriginal(planes)
	{
	}

	Polyhedron_3::Plane_3 operator()(Polyhedron_3::Facet& facet)
	{
		int iPlane = findBestPlaneOriginal(facet, planesOriginal);
		Plane_3 planeBest = planesOriginal[iPlane];
		double a = planeBest.a();
		double b = planeBest.b();
		double c = planeBest.c();
		double d = planeBest.d();
		double norm = sqrt(a * a + b * b + c * c);
		a /= norm;
		b /= norm;
		c /= norm;
		d /= norm;
		if (d > 0.)
		{
			a = -a;
			b = -b;
			c = -c;
			d = -d;
		}
		planeBest = Plane_3(a, b, c, d);
		return planeBest;
	}
};

double distance(PCLPlane_3 alphaGiven, PCLPlane_3 betaGiven)
{
	DEBUG_START;
	PCLPlane_3 alpha = normalize(alphaGiven);
	PCLPlane_3 beta = normalize(betaGiven);
	double a = alpha.a() - beta.a();
	double b = alpha.b() - beta.b();
	double c = alpha.c() - beta.c();
	double d = alpha.d() - beta.d();
	double error = sqrt(a * a + b * b + c * c + d * d);
	DEBUG_END;
	return error;
}

PCLPlane_3 nearestPlane(PCLPlane_3 alpha, std::vector<PCLPlane_3> planes)
{
	DEBUG_START;
	double minimum = 1e100;
	PCLPlane_3 planeBest;
	for (PCLPlane_3 plane: planes)
	{
		double error = distance(alpha, plane);
		if (error < minimum)
		{
			minimum = error;
			planeBest = plane;
		}
	}
	DEBUG_END;
	return planeBest;
}

void Polyhedron_3::initialize_indices(std::vector<PCLPlane_3> planes)
{
	DEBUG_START;
	int iFacet = 0;
 	std::vector<int> index(size_of_facets());
	std::set<int> usedIndices;
	for (auto facet = facets_begin(); facet != facets_end(); ++facet)
	{
		int iBestPlane = findBestPlaneOriginal(*facet, planes);
		index[iFacet] = iBestPlane;
		facet->id = iFacet;
		usedIndices.insert(iBestPlane);
		++iFacet;
	}
	if (usedIndices.size() < index.size())
	{
		std::cerr << "Equal indices: " << usedIndices.size() << " < "
			<< index.size() << std::endl;
	}
	indexPlanes_ = index;
	DEBUG_END;
}

Polyhedron_3::Polyhedron_3(std::vector<PCLPlane_3> planes)
{
	DEBUG_START;
	CGAL::internal::halfspaces_intersection(planes.begin(), planes.end(),
			*this, Kernel());
	std::transform(facets_begin(), facets_end(), planes_begin(),
			Plane_from_planes(planes));
	initialize_indices();
	DEBUG_END;
}
