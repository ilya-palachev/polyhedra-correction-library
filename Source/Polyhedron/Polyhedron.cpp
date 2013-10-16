/* 
 * Copyright (c) 2009-2013 Ilya Palachev <iliyapalachev@gmail.com>
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

#include "PolyhedraCorrectionLibrary.h"

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
