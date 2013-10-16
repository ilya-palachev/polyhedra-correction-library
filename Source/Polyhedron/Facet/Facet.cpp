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

#define DEFAULT_NV 1000

Facet::Facet() :
				id(-1),
				numVertices(0),
				plane(),
				indVertices(NULL)
{
	DEBUG_START;
	rgb[0] = 100;
	rgb[1] = 100;
	rgb[2] = 100;
	DEBUG_END;
}

Facet::Facet(const int id_orig, const int nv_orig, const Plane plane_orig,
		const int* index_orig, shared_ptr<Polyhedron> poly_orig = NULL,
		const bool ifLong = false) :

				id(id_orig),
				numVertices(nv_orig),
				plane(plane_orig),
				parentPolyhedron(poly_orig)
{
	DEBUG_START;
	init_full(index_orig, ifLong);
	DEBUG_END;
}

void Facet::init_full(const int* index_orig, const bool ifLong)
{
	DEBUG_START;

	rgb[0] = 100;
	rgb[1] = 100;
	rgb[2] = 100;

	if (!index_orig)
	{
		ERROR_PRINT("Error. index_orig = NULL");
	}
	if (numVertices < 3)
	{
		ERROR_PRINT("Error. nv_orig < 3");
	}
	indVertices = new int[3 * numVertices + 1];
	if (ifLong)
	{
		for (int i = 0; i < 3 * numVertices + 1; ++i)
			indVertices[i] = index_orig[i];
	}
	else
	{
		for (int i = 0; i < numVertices + 1; ++i)
			indVertices[i] = index_orig[i];
		for (int i = numVertices + 1; i < 3 * numVertices + 1; ++i)
			indVertices[i] = INT_NOT_INITIALIZED;
	}
	DEBUG_END;
}

Facet::Facet(int id_orig, int nv_orig, Plane plane_orig,
		shared_ptr<Polyhedron> poly_orig) :
				id(id_orig),
				numVertices(nv_orig),
				plane(plane_orig),
				parentPolyhedron(poly_orig)
{
	DEBUG_START;
	init_empty();
	DEBUG_END;
}

void Facet::init_empty()
{
	DEBUG_START;
	rgb[0] = rgb[1] = rgb[2] = 255;
	indVertices = new int[3 * numVertices + 1];
	for (int iVertex = 0; iVertex < 3 * numVertices + 1; ++iVertex)
	{
		indVertices[iVertex] = INT_NOT_INITIALIZED;
	}
	DEBUG_END;
}

Facet& Facet::operator =(const Facet& facet1)
{
	DEBUG_START;
	int i;

	id = facet1.id;
	numVertices = facet1.numVertices;
	plane = facet1.plane;

	if (indVertices != NULL)
	{
		delete[] indVertices;
	}
	indVertices = new int[3 * numVertices + 1];
	for (i = 0; i < 3 * numVertices + 1; ++i)
		indVertices[i] = facet1.indVertices[i];

	parentPolyhedron = facet1.parentPolyhedron;

	rgb[0] = facet1.rgb[0];
	rgb[1] = facet1.rgb[1];
	rgb[2] = facet1.rgb[2];

	DEBUG_END;
	return *this;
}

Facet::~Facet()
{
	DEBUG_START;
	DEBUG_PRINT("Deleting facet[%d]", id);
	clear();
	DEBUG_END;
}

void Facet::clear()
{
	DEBUG_START;
	if (indVertices != NULL)
	{
		delete[] indVertices;
		indVertices = NULL;
	}
	numVertices = 0;
	DEBUG_END;
}

bool Facet::test_initialization()
{
	DEBUG_START;
	for (int iVertex = 0; iVertex < numVertices; ++iVertex)
	{
		if (indVertices[iVertex] == INT_NOT_INITIALIZED)
		{
			DEBUG_END;
			return false;
		}
	}
	DEBUG_END;
	return true;
}

int Facet::get_id()
{
	DEBUG_START;
	DEBUG_END;
	return id;
}

int Facet::get_nv()
{
	DEBUG_START;
	DEBUG_END;
	return numVertices;
}

Plane Facet::get_plane()
{
	DEBUG_START;
	DEBUG_END;
	return plane;
}

int Facet::get_index(int pos)
{
	DEBUG_START;
	DEBUG_END;
	return indVertices[pos];
}

char Facet::get_rgb(int pos)
{
	DEBUG_START;
	DEBUG_END;
	return rgb[pos];
}

void Facet::set_id(int id1)
{
	DEBUG_START;
	id = id1;
	DEBUG_END;
}

void Facet::set_poly(shared_ptr<Polyhedron> poly_new)
{
	DEBUG_START;
	parentPolyhedron = poly_new;
	DEBUG_END;
}

void Facet::set_rgb(unsigned char red, unsigned char gray, unsigned char blue)
{
	DEBUG_START;
	rgb[0] = red;
	rgb[1] = gray;
	rgb[2] = blue;
	DEBUG_END;
}

void Facet::set_ind_vertex(int position, int value)
{
	DEBUG_START;
	if (position >= numVertices)
	{
		ERROR_PRINT("Facet overflow: %d >= %d", position, numVertices);
		DEBUG_END;
		return;
	}
	DEBUG_PRINT("setting indVertices[%d] = %d", position, value);
	indVertices[position] = value;
	/* Previous assignment can break the cycling vertex. Thus we need to repair
	 * it. */
	indVertices[numVertices] = indVertices[0];
	DEBUG_END;
}

void Facet::get_next_facet(int pos_curr, int& pos_next, int& fid_next,
		int& v_curr)
{
	DEBUG_START;
	fid_next = indVertices[pos_curr + numVertices + 1];
	pos_next = indVertices[pos_curr + 2 * numVertices + 1];
	v_curr = indVertices[pos_curr + 1];
	DEBUG_END;
}

int Facet::signum(int i, Plane plane)
{
	DEBUG_START;
	if (auto polyhedron = parentPolyhedron.lock())
	{
		DEBUG_END;
		return polyhedron->signum(polyhedron->vertices[indVertices[i]], plane);
	}
	else
	{
		ERROR_PRINT("parentPolyhedron expired.");
		ASSERT(0);
		DEBUG_END;
		return -RAND_MAX;
	}
}

void Facet::find_and_replace_vertex(int from, int to)
{
	DEBUG_START;
	for (int i = 0; i < numVertices + 1; ++i)
		if (indVertices[i] == from)
		{
			indVertices[i] = to;
		}
	DEBUG_END;
}

void Facet::find_and_replace_facet(int from, int to)
{
	DEBUG_START;
	for (int i = numVertices + 1; i < 2 * numVertices + 1; ++i)
		if (indVertices[i] == from)
		{
			indVertices[i] = to;
		}
	DEBUG_END;
}

void Facet::delete_vertex(int v)
{
	DEBUG_START;
	int i, j, found = 0;
	for (i = 0; i < numVertices; ++i)
	{
		if (indVertices[i] == v)
		{
			found = 1;
			for (j = i; j < numVertices - 1; ++j)
				indVertices[2 * numVertices + 1 + j] = indVertices[2
						* numVertices + 2 + j];
			for (j = i; j < 2 * numVertices - 2; ++j)
				indVertices[numVertices + 1 + j] = indVertices[numVertices + 2
						+ j];
			for (j = i; j < 3 * numVertices - 2; ++j)
				indVertices[j] = indVertices[j + 1];
			--numVertices;
			if (i == 0)
				indVertices[numVertices] = indVertices[0];
		}
	}
	if (found == 1)
	{
		DEBUG_PRINT("\tdelete_vertex(%d) in facet %d", v, id);
	}
	DEBUG_END;
}

void Facet::find_next_facet(int v, int& fid_next)
{
	DEBUG_START;
	int i;
	for (i = 0; i < numVertices; ++i)
			if (indVertices[i] == v)
					break;
	if (i == numVertices)
	{
			fid_next = -1;
			ERROR_PRINT("Error. Cannot find vertex %d at facet %d", v, id);
			DEBUG_END;
			return;
	}
	fid_next = indVertices[numVertices + 1 + i];
	DEBUG_END;
}

int Facet::find_vertex(int what)
{
	DEBUG_START;
	int i;
	for (i = 0; i < numVertices; ++i)
		if (indVertices[i] == what)
		{
			DEBUG_END;
			return i;
		}
	DEBUG_END;
	return -1;
}

void Facet::add(int what, int pos)
{
	DEBUG_START;

	int i, *index_new;
	DEBUG_PRINT("add %d at position %d in facet %d", what, pos, id);
	DEBUG_PRINT("{{{");
	this->my_fprint_all(stdout);

	if (pos < numVertices)
	{
		index_new = new int[3 * numVertices + 4];

		for (i = 3 * numVertices + 3; i > 2 * (numVertices + 1) + 1 + pos; --i)
		{
			index_new[i] = indVertices[i - 3];
		}
		index_new[2 * (numVertices + 1) + 1 + pos] = -1;
		for (i = 2 * (numVertices + 1) + pos; i > numVertices + 1 + 1 + pos;
				--i)
		{
			index_new[i] = indVertices[i - 2];
		}
		index_new[numVertices + 1 + 1 + pos] = -1;
		for (i = numVertices + 1 + pos; i > pos; --i)
		{
			index_new[i] = indVertices[i - 1];
		}
		index_new[pos] = what;
		for (i = pos - 1; i >= 0; --i)
		{
			index_new[i] = indVertices[i];
		}
		if (indVertices != NULL)
			delete[] indVertices;
		indVertices = index_new;
		++numVertices;
	}
	else
	{
		indVertices[pos] = what;
	}
	this->my_fprint_all(stdout);
	test_pair_neighbours();
	DEBUG_PRINT("}}}");
	DEBUG_END;
}

void Facet::remove(int pos)
{
	DEBUG_START;
	int i;
	DEBUG_PRINT("remove position %d (vertex %d) in facet %d", pos,
			indVertices[pos], id);
	DEBUG_PRINT("{{{");
	this->my_fprint_all(stdout);

	for (i = pos; i < numVertices + pos; ++i)
	{
		indVertices[i] = indVertices[i + 1];
	}
	for (i = numVertices + pos; i < pos + 2 * numVertices - 1; ++i)
	{
		indVertices[i] = indVertices[i + 2];
	}
	for (i = pos + 2 * numVertices - 1; i < 3 * numVertices - 2; ++i)
	{
		indVertices[i] = indVertices[i + 3];
	}
	--numVertices;

	indVertices[numVertices] = indVertices[0];

	this->my_fprint_all(stdout);
	test_pair_neighbours();
	DEBUG_PRINT("}}}");
	DEBUG_END;
}

void Facet::update_info()
{
	DEBUG_START;

	DEBUG_PRINT("update info in facet %d", id);
	DEBUG_PRINT("{{{");
	this->my_fprint_all(stdout);
	
	if (auto polyhedron = parentPolyhedron.lock())
	{

		for (int iVertex = 0; iVertex < numVertices; ++iVertex)
		{
			int iFacet = indVertices[numVertices + 1 + iVertex];
			int pos = polyhedron->facets[iFacet].find_vertex(indVertices[iVertex]);
			if (pos == -1)
			{
				ERROR_PRINT(
						"=======update_info: Error. Cannot find vertex %d in facet %d",
						indVertices[iVertex], iFacet);
				polyhedron->facets[iFacet].my_fprint_all(stdout);
				this->my_fprint_all(stdout);
				DEBUG_END;
				return;
			}
			indVertices[2 * numVertices + 1] = pos;
			int nnv = polyhedron->facets[iFacet].numVertices;
			pos = (pos + nnv - 1) % nnv;
			if (polyhedron->facets[iFacet].indVertices[nnv + 1 + pos] != id)
			{
				ERROR_PRINT(
						"=======update_info: Error. Wrong neighbor facet for vertex"
						" %d in facet %d", indVertices[iVertex], iFacet);
				polyhedron->facets[iFacet].my_fprint_all(stdout);
				this->my_fprint_all(stdout);
				DEBUG_END;
				return;
			}
			polyhedron->facets[iFacet].indVertices[2 * nnv + 1 + pos] = iVertex;
		}
	}
	this->my_fprint_all(stdout);
	test_pair_neighbours();
	DEBUG_PRINT("}}}");
	DEBUG_END;
}

Vector3d& Facet::find_mass_centre()
{
	DEBUG_START;
	int i;
	double c = 1. / (double) numVertices;
	Vector3d* v = new Vector3d(0., 0., 0.);
	Vector3d a;
	
	Vector3d* vertices = NULL;
	
	if (auto polyhedron = parentPolyhedron.lock())
	{
		vertices = polyhedron->vertices;
	}
	else
	{
		ERROR_PRINT("parentPolyhedron expired");
		ASSERT(0 && "parentPolyhedron expired");
		DEBUG_END;
		return *v;
	}
	
	for (i = 0; i < numVertices; ++i)
	{
		a = vertices[indVertices[i]];
		*v += a;
		DEBUG_PRINT("facet[%d].index[%d] = (%lf, %lf, %lf)", id, i, a.x, a.y, a.z);
	}
	*v *= c;
	DEBUG_END;
	return *v;
}

