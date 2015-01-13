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

#include <cmath>

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "Vector3d.h"
#include "Polyhedron/Polyhedron.h"
#include "Polyhedron/Facet/Facet.h"
#include "Polyhedron/VertexInfo/VertexInfo.h"
#include "DataContainers/ShadowContourData/SContour/SContour.h"

Polyhedron::Polyhedron() :
				numVertices(0),
				numFacets(0),
				vertices(NULL),
				facets(NULL),
				vertexInfos(NULL)
{
	DEBUG_START;
	DEBUG_PRINT("Creating empty polyhedron...\n");
	DEBUG_END;
}

Polyhedron::Polyhedron(int numv_orig, int numf_orig) :
				numVertices(numv_orig),
				numFacets(numf_orig),
				vertices(),
				facets(),
				vertexInfos(NULL)
{
	DEBUG_START;

	if (vertices != NULL)
	{
		delete[] vertices;
	}
	vertices = new Vector3d[numv_orig];

	if (facets != NULL)
	{
		delete[] facets;
	}
	facets = new Facet[numf_orig];

	DEBUG_PRINT("Creating polyhedron with numv = %d, numf = %d...\n",
			numVertices, numFacets);
	DEBUG_END;
}

Polyhedron::Polyhedron(int numv_orig, int numf_orig, Vector3d* vertex_orig,
		Facet* facet_orig) :
				numVertices(numv_orig),
				numFacets(numf_orig),
				vertices(),
				facets(),
				vertexInfos(NULL)
{
	DEBUG_START;
	DEBUG_PRINT("Creating polyhedron by coping...\n");

	if (vertices != NULL)
	{
		delete[] vertices;
	}
	vertices = new Vector3d[numv_orig];

	if (facets != NULL)
	{
		delete[] facets;
	}
	facets = new Facet[numf_orig];

	for (int i = 0; i < numVertices; ++i)
		vertices[i] = vertex_orig[i];
	for (int i = 0; i < numFacets; ++i)
		facets[i] = facet_orig[i];
	DEBUG_END;
}

Polyhedron::Polyhedron(BasePolyhedron_3 p)
{
	DEBUG_START;

	/* Check for non-emptiness. */
	ASSERT(p.size_of_vertices());
	ASSERT(p.size_of_facets());

	numVertices = p.size_of_vertices();
	numFacets = p.size_of_facets();

	/* Allocate memory for arrays. */
	vertices = new Vector3d[numVertices];
	facets = new Facet[numFacets];
	vertexInfos = NULL;

	/* Transform vertexes. */
	int iVertex = 0;
	for (auto vertex = p.vertices_begin(); vertex != p.vertices_end(); ++vertex)
	{
		vertices[iVertex++] = Vector3d(
			vertex->point().x(),
			vertex->point().y(),
			vertex->point().z());
	}

	/*
	 * Transform facets.
	 * This algorithm is based on example kindly provided at CGAL online user
	 * manual. See example Polyhedron/polyhedron_prog_off.cpp
	 */
	int iFacet = 0;
	auto plane = p.planes_begin();
	auto facet = p.facets_begin();
	/* Iterate through the std::lists of planes and facets. */
	do
	{
		facets[iFacet].id = iFacet;

		/* Transform current plane. */
		facets[iFacet].plane = Plane(Vector3d(plane->a(), plane->b(),
			plane->c()), plane->d());

		/*
		 * Iterate through the std::list of halfedges incident to the curent CGAL
		 * facet.
		 */
		auto halfedge = facet->facet_begin();

		/* Facets in polyhedral surfaces are at least triangles. 	*/
		CGAL_assertion(CGAL::circulator_size(halfedge) >= 3);

		facets[iFacet].numVertices = CGAL::circulator_size(halfedge);
		facets[iFacet].indVertices =
			new int[3 * facets[iFacet].numVertices + 1];
		/*
		 * TODO: It's too unsafe architecture if we do such things as setting
		 * the size of internal array outside the API functions. Moreover, it
		 * can cause us to write memory leaks.
		 * indFacets and numFacets should no be public members.
		 */

		int iFacetVertex = 0;
		do
		{
			facets[iFacet].indVertices[iFacetVertex++] =
				std::distance(p.vertices_begin(), halfedge->vertex());
		} while (++halfedge != facet->facet_begin());

		/* Add cycling vertex to avoid assertion during printing. */
		facets[iFacet].indVertices[facets[iFacet].numVertices] =
			facets[iFacet].indVertices[0];

		/* Increment the ID of facet. */
		++iFacet;

	} while (++plane != p.planes_end() && ++facet != p.facets_end());

	DEBUG_END;
}

Polyhedron::Polyhedron(ShadowContourDataPtr data)
{
	DEBUG_START;

	ASSERT(data);
	ASSERT(data->numContours > 0);
	ASSERT(!data->empty());
	numFacets = data->numContours;
	facets = new Facet[numFacets];

	/* Get vertices from contours. */
	std::vector<Vector3d> verticesAll;
	numVertices = 0;
	int iVertex = 0;
	for (int iContour = 0; iContour < data->numContours; ++iContour)
	{
		SContour* contour = &data->contours[iContour];
		auto verticesPortion = contour->getPoints();
		ASSERT(!verticesPortion.empty());
		
		Facet *facet = &facets[iContour];
		facet->id = iContour;
		facet->numVertices = verticesPortion.size();
		facet->plane = contour->plane;
		facet->indVertices = new int[3 * facet->numVertices + 1];
		for (int iVertexLocal = 0;
			(unsigned) iVertexLocal < verticesPortion.size();
			++iVertexLocal)
		{
			facet->indVertices[iVertexLocal] = iVertex++;
		}
		/* Cycle vertices. */
		facet->indVertices[facet->numVertices] = facet->indVertices[0];

		numVertices += verticesPortion.size();

		verticesAll.insert(verticesAll.end(), verticesPortion.begin(),
				verticesPortion.end());
	}

	vertices = new Vector3d[numVertices];
	iVertex = 0;
	for (auto &vertex: verticesAll)
	{
		vertices[iVertex++] = vertex;
	}
	vertexInfos = new VertexInfo[numVertices];

	ASSERT(numVertices > 0);
	DEBUG_END;
}

Polyhedron::Polyhedron(PolyhedronPtr p) :
				numVertices(p->numVertices),
				numFacets(p->numFacets),
				vertices(NULL),
				facets(NULL),
				vertexInfos(NULL)

{
	DEBUG_START;
	vertices = new Vector3d[numVertices];
	for (int i = 0; i < numVertices; ++i)
		vertices[i] = p->vertices[i];
	facets = new Facet[numFacets];
	for (int i = 0; i < numFacets; ++i)
		facets[i] = p->facets[i];
	DEBUG_END;
}

Polyhedron::~Polyhedron()
{
	DEBUG_START;
	DEBUG_PRINT("Deleting Polyhedron...");
	if (vertices)
	{
		delete[] vertices;
		vertices = NULL;
	}
	if (facets)
	{
		delete[] facets;
		facets = NULL;
	}
	if (vertexInfos)
	{
		delete[] vertexInfos;
		vertexInfos = NULL;
	}
	DEBUG_END;
}

const double EPS_SIGNUM = 1e-15;

int Polyhedron::signum(Vector3d point, Plane plane)
{
	DEBUG_START;
	double d = plane % point;
	if (fabs(d) < EPS_SIGNUM)
	{
		DEBUG_END;
		return 0;
	}
	DEBUG_END;
	return d > 0 ? 1 : (d < 0 ? -1 : 0);
}

void Polyhedron::get_boundary(double& xmin, double& xmax, double& ymin,
		double& ymax, double& zmin, double& zmax)
{
	DEBUG_START;
	int i;
	double tmp;

	if (numVertices <= 0)
	{
		ERROR_PRINT("Number of vertices is %d, so getting boundary is "
				"aborted!", numVertices);
		xmin = xmax = ymin = ymax = zmin = zmax = 0.;
		DEBUG_END;
		return;
	}
	xmin = xmax = vertices[0].x;
	ymin = ymax = vertices[0].y;
	zmin = zmax = vertices[0].z;

	for (i = 0; i < numVertices; ++i)
	{
		tmp = vertices[i].x;
		if (tmp > xmax)
			xmax = tmp;
		if (tmp < xmin)
			xmin = tmp;

		tmp = vertices[i].y;
		if (tmp > ymax)
			ymax = tmp;
		if (tmp < ymin)
			ymin = tmp;

		tmp = vertices[i].z;
		if (tmp > zmax)
			zmax = tmp;
		if (tmp < zmin)
			zmin = tmp;
	}
	DEBUG_END;
}

void Polyhedron::delete_empty_facets()
{
	DEBUG_START;
	int i, j;
	for (i = 0; i < numFacets; ++i)
	{
		if (facets[i].numVertices < 3)
		{
			DEBUG_PRINT("===Facet %d is empty...\n", i);
			for (j = i; j < numFacets - 1; ++j)
			{
				facets[j] = facets[j + 1];
				facets[j].id = j;
			}

			--numFacets;
		}
	}
	DEBUG_END;
}

void Polyhedron::set_parent_polyhedron_in_facets()
{
	DEBUG_START;
	for (int iFacet = 0; iFacet < numFacets; ++iFacet)
	{
		DEBUG_PRINT("Setting parent polyhedron in facet #%d", iFacet);
		PolyhedronPtr this_shared = shared_from_this();
		DEBUG_PRINT("Polyhedron use count = %ld",
					this_shared.use_count());
		facets[iFacet].set_poly(this_shared);
	}
	DEBUG_END;
}

