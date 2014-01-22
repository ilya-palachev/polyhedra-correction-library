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

#ifndef EDGE_H
#define	EDGE_H

#include <list>
#include <cstdio>

#include "DataContainers/EdgeData/EdgeContourAssociation/EdgeContourAssociation.h"

using namespace std;

class Edge
{
public:
	mutable int id;
	mutable int v0;
	mutable int v1;
	mutable int f0;
	mutable int f1;

	mutable list<EdgeContourAssociation> assocList;

	// Edge.cpp :
	Edge();

	Edge(int v0_orig, int v1_orig);

	Edge(int id_orig, int v0_orig, int v1_orig, int f0_orig, int f1_orig);

	Edge(int v0_orig, int v1_orig, int f0_orig, int f1_orig);

	Edge(int id_orig, int v0_orig, int v1_orig, int f0_orig, int f1_orig,
			list<EdgeContourAssociation> accocList_orig);

	Edge(int v0_orig, int v1_orig, int f0_orig, int f1_orig,
			list<EdgeContourAssociation> accocList_orig);

	Edge(const Edge& orig);
	~Edge();

	Edge& operator =(const Edge& orig);

	bool operator ==(const Edge& e) const;
	bool operator !=(const Edge& e) const;

	//Edge_io.cpp :
	void my_fprint(FILE* file) const;

private:

};

class EdgeComparison
{
public:
	inline bool operator() (Edge edge0, Edge edge1)
	{
		return edge0.v0 < edge1.v0 ||
				(edge0.v0 == edge1.v0 && edge0.v1 < edge1.v1);
	}
};

#endif	/* EDGE_H */

