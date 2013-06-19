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
//		fprintf(stdout, "Facet::Facet. Error. poly = NULL\n");
//	}
	if (!index_orig)
	{
		fprintf(stdout, "Facet::Facet. Error. index_orig = NULL\n");
	}
	if (nv_orig < 3)
	{
		fprintf(stdout, "Facet::Facet. Error. nv_orig < 3\n");
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
	fprintf(stdout, "Deleting facet[%d]\n", id);
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
