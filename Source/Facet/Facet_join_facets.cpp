#include "PolyhedraCorrectionLibrary.h"

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
		printf("\tdelete_vertex(%d) in facet %d\n", v, id);
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
		printf(
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
	printf("add %d at position %d in facet %d\n", what, pos, id);
	printf("{{{\n");
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
	printf("}}}\n");
}

void Facet::remove(int pos)
{
	int i;
	printf("remove position %d (vertex %d) in facet %d\n", pos,
			indVertices[pos], id);
	printf("{{{\n");
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
	printf("}}}\n");
}

void Facet::update_info()
{

	int i, facet, pos, nnv;

	printf("update info in facet %d\n", id);
	printf("{{{\n");
	this->my_fprint_all(stdout);

	for (i = 0; i < numVertices; ++i)
	{
		facet = indVertices[numVertices + 1 + i];
		pos = parentPolyhedron->facets[facet].find_vertex(indVertices[i]);
		if (pos == -1)
		{
			printf(
					"=======update_info: Error. Cannot find vertex %d in facet %d\n",
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
			printf(
					"=======update_info: Error. Wrong neighbor facet for vertex %d in facet %d\n",
					indVertices[i], facet);
			parentPolyhedron->facets[facet].my_fprint_all(stdout);
			this->my_fprint_all(stdout);
			return;
		}
		parentPolyhedron->facets[facet].indVertices[2 * nnv + 1 + pos] = i;
	}
	this->my_fprint_all(stdout);
	test_pair_neighbours();
	printf("}}}\n");
}

int Facet::test_structure()
{

	int i, facet, pos, nnv;

	for (i = 0; i < numVertices; ++i)
	{
		facet = indVertices[numVertices + 1 + i];
		pos = parentPolyhedron->facets[facet].find_vertex(indVertices[i]);
		if (pos == -1)
		{
			printf(
					"=======test_structure: Error. Cannot find vertex %d in facet %d\n",
					indVertices[i], facet);
			parentPolyhedron->facets[facet].my_fprint_all(stdout);
			this->my_fprint_all(stdout);
			return 1;
		}
		indVertices[2 * numVertices + 1] = pos;
		nnv = parentPolyhedron->facets[facet].numVertices;
		pos = (pos + nnv - 1) % nnv;
		if (parentPolyhedron->facets[facet].indVertices[nnv + 1 + pos] != id)
		{
			printf(
					"=======test_structure: Error. Wrong neighbor facet for vertex %d in facet %d\n",
					indVertices[i], facet);
			parentPolyhedron->facets[facet].my_fprint_all(stdout);
			this->my_fprint_all(stdout);
			return 1;
		}
		parentPolyhedron->facets[facet].indVertices[2 * nnv + 1 + pos] = i;
	}
	return 0;
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
		printf("facet[%d].index[%d] = (%lf, %lf, %lf)\n", id, i, a.x, a.y, a.z);
	}
	*v *= c;
	return *v;
}

void Facet::test_pair_neighbours()
{
	int i, j;
	for (i = 0; i < numVertices; ++i)
	{
		for (j = 0; j < i; ++j)
		{
			if (indVertices[numVertices + 1 + i]
					== indVertices[numVertices + 1 + j])
			{
				printf(
						"WARNING!!! test_pair_neighbours in facet %d. neighbour[%d] = %d and neighbour[%d] = %d\n\n",
						id, j, indVertices[numVertices + 1 + j], i,
						indVertices[numVertices + 1 + i]);
			}
		}

	}
}
