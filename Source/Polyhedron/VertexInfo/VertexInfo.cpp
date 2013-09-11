#include "PolyhedraCorrectionLibrary.h"

VertexInfo::VertexInfo() :
				id(-1),
				numFacets(0),
				indFacets(NULL),
				parentPolyhedron(NULL)
{
}

VertexInfo::VertexInfo(const int id_orig, const int nf_orig,
		const Vector3d vector_orig, const int* index_orig,
		shared_ptr<Polyhedron> poly_orig) :
				id(id_orig),
				numFacets(nf_orig),
				vector(vector_orig),
				parentPolyhedron(poly_orig)
{

	if (!index_orig)
	{
		ERROR_PRINT("Error. index1 = NULL\n");
	}
	if (nf_orig < 3)
	{
		ERROR_PRINT("Error. nf1 < 3\n");
	}
	indFacets = new int[3 * numFacets + 1];
	for (int i = 0; i < 3 * numFacets + 1; ++i)
		indFacets[i] = index_orig[i];
}

VertexInfo::VertexInfo(const int id_orig, const Vector3d vector_orig,
		shared_ptr<Polyhedron> poly_orig) :
				id(id_orig),
				numFacets(0),
				vector(vector_orig),
				indFacets(NULL),
				parentPolyhedron(poly_orig)
{
}

VertexInfo::~VertexInfo()
{

	if (indFacets != NULL && numFacets != 0)
	{
		delete[] indFacets;
	}
	indFacets = NULL;
	numFacets = 0;
}

VertexInfo& VertexInfo::operator =(const VertexInfo& orig)
{
	int i;

	this->id = orig.id;
	this->vector = orig.vector;
	this->parentPolyhedron = orig.parentPolyhedron;
	this->numFacets = orig.numFacets;

	if (numFacets > 0)
	{
		if (indFacets)
			delete[] indFacets;
		indFacets = new int[3 * numFacets + 1];
		for (i = 0; i < 3 * numFacets + 1; ++i)
			indFacets[i] = orig.indFacets[i];
	}
	return *this;
}

int VertexInfo::get_nf()
{
	return numFacets;
}
