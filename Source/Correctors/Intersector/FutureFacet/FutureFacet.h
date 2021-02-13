/*
 * Copyright (c) 2009-2016 Ilya Palachev <iliyapalachev@gmail.com>
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

#ifndef FUTUREFACET_H
#define FUTUREFACET_H

#include <cstdio>

#include "Polyhedron/Facet/Facet.h"
#include "Vector3d.h"

class EdgeSetIntersected;

class FutureFacet
{
private:
	int id;
	int len;
	int nv;
	int *edge0;
	int *edge1;
	int *src_facet;
	int *id_v_new;

public:
	// FutureFacet.cpp
	FutureFacet();
	~FutureFacet();
	FutureFacet(int nv_orig);
	FutureFacet(const FutureFacet &orig);
	FutureFacet &operator=(const FutureFacet &orig);
	FutureFacet &operator+=(const FutureFacet &v);
	void free();

	int get_nv();

	void set_id(int val);

	void add_edge(int v0, int v1, int src_f);
	void get_edge(int pos, int &v0, int &v1, int &src_f, int &id_v);

	// FutureFacet_io.cpp
	void my_fprint(FILE *file);

	// FutureFacet_intersection.cpp
	void generate_facet(Facet &facet, int fid, Plane &iplane, int numv,
						EdgeSetIntersected *es);
};

#endif /* FUTUREFACET_H */
