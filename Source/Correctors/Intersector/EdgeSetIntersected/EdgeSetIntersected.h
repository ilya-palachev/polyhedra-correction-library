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

#ifndef EDGESETINTERSECTED_H
#define	EDGESETINTERSECTED_H

#include <cstdio>

class EdgeList;
class FutureFacet;

class EdgeSetIntersected
{
private:
	int len;
	int num;
	int* edge0;
	int* edge1;
	int* id_edge_list0;
	int* pos_edge_list0;
	int* id_edge_list1;
	int* pos_edge_list1;
	int* id_future_facet;
	int* pos_future_facet;
public:
	EdgeSetIntersected();
	EdgeSetIntersected(int len_orig);
	EdgeSetIntersected(const EdgeSetIntersected& orig);
	EdgeSetIntersected& operator =(const EdgeSetIntersected& orig);
	~EdgeSetIntersected();

	int get_len();
	int get_num();

	void get_edge(int id, int& v0, int& v1);
	void get_edge(int id, int& v0, int& v1, int& id_el0, int& pos_el0,
			int& id_el1, int& pos_el1, int& id_ff, int& pos_ff);
	int search_edge(int v0, int v1);
	void add_edge(int v0, int v1, int id_el, int pos_el, int id_ff, int pos_ff);
	void add_edge(int v0, int v1);

	void test_info();
	void informate_about_new_vertex(int numv, EdgeList* edge_list,
			FutureFacet* future_facet);

	//EdgeSetIntersected_io.cpp
	void my_fprint(FILE* file);
};

#endif	/* EDGESETINTERSECTED_H */

