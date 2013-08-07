#include "PolyhedraCorrectionLibrary.h"

EdgeList::EdgeList() :
				id(-1),
				len(0),
				num(0),
				pointer(0),
				edge0(NULL),
				edge1(NULL),
				ind0(NULL),
				ind1(NULL),
				next_facet(NULL),
				next_direction(NULL),
				scalar_mult(NULL),
				id_v_new(NULL),
				isUsed(NULL),
				poly(NULL)
{
}

EdgeList::EdgeList(int id_orig, int len_orig, Polyhedron* poly_orig) :
				id(id_orig),
				len(len_orig),
				num(0),
				pointer(0),
				edge0(new int[len]),
				edge1(new int[len]),
				ind0(new int[len]),
				ind1(new int[len]),
				next_facet(new int[len]),
				next_direction(new int[len]),
				scalar_mult(new double[len]),
				id_v_new(new int[len]),
				isUsed(new bool[len]),
				poly(poly_orig)
{
}

EdgeList::~EdgeList()
{
	if (edge0 != NULL)
		delete[] edge0;
	if (edge1 != NULL)
		delete[] edge1;
	if (ind0 != NULL)
		delete[] ind0;
	if (ind1 != NULL)
		delete[] ind1;
	if (next_facet != NULL)
		delete[] next_facet;
	if (next_direction != NULL)
		delete[] next_direction;
	if (scalar_mult != NULL)
		delete[] scalar_mult;
	if (id_v_new != NULL)
		delete[] id_v_new;
	if (isUsed != NULL)
		delete[] isUsed;
}

EdgeList& EdgeList::operator =(const EdgeList& orig)
{
	int i;

	id = orig.id;
	len = orig.len;
	num = orig.num;
	pointer = orig.pointer;

	if (edge0 != NULL)
		delete[] edge0;
	if (edge1 != NULL)
		delete[] edge1;
	if (ind0 != NULL)
		delete[] edge0;
	if (ind1 != NULL)
		delete[] edge1;
	if (next_facet != NULL)
		delete[] next_facet;
	if (next_direction != NULL)
		delete[] next_direction;
	if (scalar_mult != NULL)
		delete[] scalar_mult;
	if (id_v_new != NULL)
		delete[] id_v_new;
	if (isUsed != NULL)
		delete[] isUsed;

	edge0 = new int[len];
	edge1 = new int[len];
	ind0 = new int[len];
	ind1 = new int[len];
	next_facet = new int[len];
	next_direction = new int[len];
	scalar_mult = new double[len];
	id_v_new = new int[len];
	isUsed = new bool[len];

	for (i = 0; i < num; ++i)
	{
		edge0[i] = orig.edge0[i];
		edge1[i] = orig.edge1[i];
		ind0[i] = orig.ind0[i];
		ind1[i] = orig.ind1[i];
		next_facet[i] = orig.next_facet[i];
		next_direction[i] = orig.next_direction[i];
		scalar_mult[i] = orig.scalar_mult[i];
		id_v_new[i] = orig.id_v_new[i];
		isUsed[i] = orig.isUsed[i];
	}
	return *this;
}

int EdgeList::get_num()
{
	return num;
}

void EdgeList::set_id_v_new(int id_v)
{
	id_v_new[pointer] = id_v;
}

void EdgeList::set_isUsed(bool val)
{
	DEBUG_PRINT("EdgeList[%d].isUsed[%d] = %d\n",
			id, pointer, val);
	isUsed[pointer] = val;
}

void EdgeList::set_pointer(int val)
{
	if (val < 0 || val > num - 1)
	{
		ERROR_PRINT("Error. num = %d, val = %d", num, val);
		return;
	}
	DEBUG_PRINT("EdgeList[%d].set_pointer(%d)\n",
			id, val);
	pointer = val;
}

void EdgeList::goto_header()
{
	set_pointer(0);
}

void EdgeList::go_forward()
{
	set_pointer(pointer + 1);
}

