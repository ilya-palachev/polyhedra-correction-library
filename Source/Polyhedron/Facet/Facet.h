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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FACET_H
#define	FACET_H

class Facet {
public:
	int id;
	int numVertices;
	Plane plane;
	int* indVertices;
	weak_ptr<Polyhedron> parentPolyhedron;
	unsigned char rgb[3];

	void init_full(const int* index_orig, const bool ifLong);
	void init_empty();

public:
	// Facet.cpp
	Facet();

	Facet(const int id_orig, const int nv_orig, const Plane plane_orig,
			const int* index_orig, shared_ptr<Polyhedron> poly_orig,
			const bool ifLong);

	Facet(const int id_orig, const int nv_orig, const Plane plane_orig,
			shared_ptr<Polyhedron> poly_orig);

	Facet& operator =(const Facet& facet1);
	~Facet();
	void clear();

	bool test_initialization();

	int get_id();
	int get_nv();
	Plane get_plane();
	int get_index(int pos);
	char get_rgb(int pos);

	void get_next_facet(int pos_curr, int& pos_next, int& fid_next,
			int& v_curr);

	void set_id(int id1);
	void set_poly(shared_ptr<Polyhedron> poly_new);
	void set_rgb(unsigned char red, unsigned char gray, unsigned char blue);
	void set_ind_vertex(int position, int value);

	int signum(int i, Plane plane);
	void find_and_replace_vertex(int from, int to);
	void find_and_replace_facet(int from, int to);

	void delete_vertex(int v);

	void find_next_facet(int v, int& fid_next);
	int find_vertex(int what);

	void add(int what, int pos);
	void remove(int pos);
	void update_info();

	Vector3d& find_mass_centre();

	// Facet_preprocess.cpp
	void preprocess_free();
	void preprocess();
	void preprocess_edge(int v0, int v1, int v0_id);
	int preprocess_search_edge(int v0, int v1); //Searches edge and returns position of v1
	//if success, -1 if not found.
	int preprocess_search_vertex(int v, int& v_next);

	// Facet_io.cpp
	void my_fprint(FILE* file);
	void my_fprint_all(FILE* file);

	void fprint_default_0(FILE* file);
	void fprint_default_1(FILE* file);
	void fprint_my_format(FILE* file);

	void fprint_ply_vertex(FILE* file);
	void fprint_ply_index(FILE* file);
	void fprint_ply_scale(FILE* file);

	void my_fprint_edge_list(FILE* file);

	// Facet_test.cpp
	bool test_self_intersection();

	// Facet_inertia.cpp
	bool consect_x(double y, double z, double& x);

	// Facet_area.cpp
	double area();

	// Facet_verification.cpp
	bool verifyIncidenceStructure();
	void test_pair_neighbours();

};

#endif	/* FACET_H */

