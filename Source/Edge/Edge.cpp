/* 
 * File:   Edge.cpp
 * Author: ilya
 * 
 * Created on 19 Ноябрь 2012 г., 8:17
 */

#include "PolyhedraCorrectionLibrary.h"

Edge::Edge() :
				id(-1),
				v0(-1),
				v1(-1),
				f0(-1),
				f1(-1),
				assocList()
{
}

Edge::Edge(int id_orig, int v0_orig, int v1_orig, int f0_orig, int f1_orig) :
				id(id_orig),
				v0(v0_orig),
				v1(v1_orig),
				f0(f0_orig),
				f1(f1_orig),
				assocList()
{
}

Edge::Edge(int v0_orig, int v1_orig, int f0_orig, int f1_orig) :
				id(-1),
				v0(v0_orig),
				v1(v1_orig),
				f0(f0_orig),
				f1(f1_orig),
				assocList()
{
}

Edge::Edge(int id_orig, int v0_orig, int v1_orig, int f0_orig, int f1_orig,
		list<EdgeContourAssociation> assocList_orig) :
				id(id_orig),
				v0(v0_orig),
				v1(v1_orig),
				f0(f0_orig),
				f1(f1_orig),
				assocList(assocList_orig)
{
}

Edge::Edge(int v0_orig, int v1_orig, int f0_orig, int f1_orig,
		list<EdgeContourAssociation> assocList_orig) :
				id(-1),
				v0(v0_orig),
				v1(v1_orig),
				f0(f0_orig),
				f1(f1_orig),
				assocList(assocList_orig)
{
}

Edge::Edge(const Edge& orig) :
				id(orig.id),
				v0(orig.v0),
				v1(orig.v1),
				f0(orig.f0),
				f1(orig.f1),
				assocList(orig.assocList)
{
}

Edge::~Edge()
{
	DEBUG_PRINT("Attention! edge #%d is deleting now\n", id);
}

Edge& Edge::operator =(const Edge& orig)
{
	id = orig.id;
	v0 = orig.v0;
	v1 = orig.v1;
	f0 = orig.f0;
	f1 = orig.f1;
	assocList = orig.assocList;

	return *this;
}

