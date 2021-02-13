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

#ifndef EDGELIST_H
#define EDGELIST_H

#include <memory>

#include "Polyhedron/Polyhedron.h"
#include "Correctors/Intersector/EdgeSetIntersected/EdgeSetIntersected.h"

class EdgeList
{
private:
	int id;
	int len;
	int num;

	int pointer;

	int *edge0;
	int *edge1;
	int *ind0;
	int *ind1;
	int *next_facet;
	int *next_direction;
	double *scalar_mult;
	int *id_v_new;
	bool *isUsed;

	PolyhedronPtr poly;

public:
	// EdgeList.cpp
	EdgeList();
	EdgeList(int id_orig, int len_orig, PolyhedronPtr poly_orig);
	EdgeList(const EdgeList &orig);
	~EdgeList();
	EdgeList &operator=(const EdgeList &orig);
	int get_num();
	//	void set_facet(Facet* facet_orig);
	void set_id_v_new(int id_v);
	void set_isUsed(bool val);
	void set_pointer(int val);

	void goto_header();
	void go_forward();

	int get_pointer();
	void set_isUsed();
	void set_isUsed(int v0, int v1, bool val);

	// EdgeList_intersection.cpp
	void add_edge(int v0, int v1, int i0, int i1, int next_f, int next_d,
				  double sm);
	void add_edge(int v0, int v1, int i0, int i1, double sm);
	void set_curr_info(int next_d, int next_f);

	void search_and_set_info(int v0, int v1, int next_d, int next_f);
	void null_isUsed();
	void get_first_edge(int &v0, int &v1, int &next_f, int &next_d);
	void get_first_edge(int &v0, int &v1);
	void get_next_edge(Plane iplane, int &v0, int &v1, int &next_f,
					   int &next_d);
	void get_next_edge(Plane iplane, int &v0, int &v1, int &i0, int &i1,
					   int &next_f, int &next_d);

	void send(EdgeSetIntersected *edge_set);
	void send_edges(EdgeSetIntersected *edge_set);

	void set_poly(PolyhedronPtr poly_orig);

	// EdgeList_io,cpp
	void my_fprint(FILE *file);
};

#endif /* EDGELIST_H */
