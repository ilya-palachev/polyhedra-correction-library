#include "PolyhedraCorrectionLibrary.h"

#define DEFAULT_NV 1000

Facet::Facet() :
				id(-1),
				numVertices(0),
				plane(),
				indVertices(new int[1]),
				parentPolyhedron(NULL)
{
	rgb[0] = 100;
	rgb[1] = 100;
	rgb[2] = 100;
}

Facet::Facet(const int id_orig, const int nv_orig, const Plane plane_orig,
		const int* index_orig, Polyhedron* poly_orig = NULL, const bool ifLong =
				false) :

				id(id_orig),
				numVertices(nv_orig),
				plane(plane_orig),
				parentPolyhedron(poly_orig)
{

	rgb[0] = 100;
	rgb[1] = 100;
	rgb[2] = 100;

//	if (!poly) {
//		fprintf(stdout, "Facet::Facet. Error. poly = NULL");
//	}
	if (!index_orig)
	{
		fprintf(stdout, "Facet::Facet. Error. index_orig = NULL");
	}
	if (nv_orig < 3)
	{
		fprintf(stdout, "Facet::Facet. Error. nv_orig < 3");
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
			indVertices[i] = -1;
	}
}

Facet::Facet(int id_orig, int nv_orig, Plane plane_orig, Polyhedron* poly_orig) :
				id(id_orig),
				numVertices(nv_orig),
				indVertices(new int[3 * numVertices + 1]),
				plane(plane_orig),
				parentPolyhedron(poly_orig),
				rgb(
				{ 255, 255, 255 })
{
	for (int iVertex = 0; iVertex < numVertices; ++iVertex)
	{
		indVertices[iVertex] = INT_NOT_INITIALIZED;
	}
}

Facet& Facet::operator =(const Facet& facet1)
{
	int i;

	id = facet1.id;
	numVertices = facet1.numVertices;
	plane = facet1.plane;

	if (indVertices)
		delete[] indVertices;
	indVertices = new int[3 * numVertices + 1];
	for (i = 0; i < 3 * numVertices + 1; ++i)
		indVertices[i] = facet1.indVertices[i];

	parentPolyhedron = facet1.parentPolyhedron;

	rgb[0] = facet1.rgb[0];
	rgb[1] = facet1.rgb[1];
	rgb[2] = facet1.rgb[2];

	return *this;
}

Facet::~Facet()
{
#ifdef DEBUG1
	fprintf(stdout, "Deleting facet[%d]", id);
#endif
	if (indVertices != NULL)
		delete[] indVertices;
}

bool Facet::test_initialization()
{
	for (int iVertex = 0; iVertex < numVertices; ++iVertex)
	{
		if (indVertices[iVertex] == INT_NOT_INITIALIZED)
		{
			return false;
		}
	}
	return true;
}

int Facet::get_id()
{
	return id;
}

int Facet::get_nv()
{
	return numVertices;
}

Plane Facet::get_plane()
{
	return plane;
}

int Facet::get_index(int pos)
{
	return indVertices[pos];
}

char Facet::get_rgb(int pos)
{
	return rgb[pos];
}

void Facet::set_id(int id1)
{
	id = id1;
}

void Facet::set_poly(Polyhedron* poly_new)
{
	parentPolyhedron = poly_new;
}

void Facet::set_rgb(char red, char gray, char blue)
{
	rgb[0] = red;
	rgb[1] = gray;
	rgb[2] = blue;
}

void Facet::set_ind_vertex(int position, int value)
{
	if (position >= numVertices)
	{
		ERROR_PRINT("Facet overflow: %d >= %d", position, numVertices);
		return;
	}
	indVertices[position] = value;
}

void Facet::get_next_facet(int pos_curr, int& pos_next, int& fid_next,
		int& v_curr)
{

	fid_next = indVertices[pos_curr + numVertices + 1];
	pos_next = indVertices[pos_curr + 2 * numVertices + 1];
	v_curr = indVertices[pos_curr + 1];
}

int Facet::signum(int i, Plane plane)
{
	return parentPolyhedron->signum(parentPolyhedron->vertices[indVertices[i]],
			plane);
}

void Facet::find_and_replace_vertex(int from, int to)
{
	for (int i = 0; i < numVertices + 1; ++i)
		if (indVertices[i] == from)
		{
			indVertices[i] = to;
		}
}

void Facet::find_and_replace_facet(int from, int to)
{
	for (int i = numVertices + 1; i < 2 * numVertices + 1; ++i)
		if (indVertices[i] == from)
		{
			indVertices[i] = to;
		}
}

void Facet::delete_vertex(int v)
{
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
		//        this->my_fprint_all(stdout);
	}
}

void Facet::find_next_facet(int v, int& fid_next)
{
        int i;
        for (i = 0; i < numVertices; ++i)
                if (indVertices[i] == v)
                        break;
        if (i == numVertices)
        {
                fid_next = -1;
                ERROR_PRINT(
                                "Facet::find_next_facet : Error. Cannot find vertex %d at facet %d",
                                v, id);
                return;
        }
        fid_next = indVertices[numVertices + 1 + i];
}

int Facet::find_vertex(int what)
{
	int i;
	for (i = 0; i < numVertices; ++i)
		if (indVertices[i] == what)
			return i;
	return -1;
}

void Facet::add(int what, int pos)
{

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
}

void Facet::remove(int pos)
{
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
	this->my_fprint_all(stdout);
	test_pair_neighbours();
	DEBUG_PRINT("}}}");
}

void Facet::update_info()
{

	int i, facet, pos, nnv;

	DEBUG_PRINT("update info in facet %d", id);
	DEBUG_PRINT("{{{");
	this->my_fprint_all(stdout);

	for (i = 0; i < numVertices; ++i)
	{
		facet = indVertices[numVertices + 1 + i];
		pos = parentPolyhedron->facets[facet].find_vertex(indVertices[i]);
		if (pos == -1)
		{
			ERROR_PRINT(
					"=======update_info: Error. Cannot find vertex %d in facet %d",
					indVertices[i], facet);
			parentPolyhedron->facets[facet].my_fprint_all(stdout);
			this->my_fprint_all(stdout);
			return;
		}
		indVertices[2 * numVertices + 1] = pos;
		nnv = parentPolyhedron->facets[facet].numVertices;
		pos = (pos + nnv - 1) % nnv;
		if (parentPolyhedron->facets[facet].indVertices[nnv + 1 + pos] != id)
		{
			ERROR_PRINT(
					"=======update_info: Error. Wrong neighbor facet for vertex %d in facet %d",
					indVertices[i], facet);
			parentPolyhedron->facets[facet].my_fprint_all(stdout);
			this->my_fprint_all(stdout);
			return;
		}
		parentPolyhedron->facets[facet].indVertices[2 * nnv + 1 + pos] = i;
	}
	this->my_fprint_all(stdout);
	test_pair_neighbours();
	DEBUG_PRINT("}}}");
}

Vector3d& Facet::find_mass_centre()
{
	int i;
	double c = 1. / (double) numVertices;
	Vector3d* v = new Vector3d(0., 0., 0.);
	Vector3d a;
	for (i = 0; i < numVertices; ++i)
	{
		a = parentPolyhedron->vertices[indVertices[i]];
		*v += a;
		DEBUG_PRINT("facet[%d].index[%d] = (%lf, %lf, %lf)", id, i, a.x, a.y, a.z);
	}
	*v *= c;
	return *v;
}

