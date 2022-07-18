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

#ifndef FACET_H
#define FACET_H

#include <memory>

#include "Vector3d.h"

class Polyhedron;
class Facet;

struct FacetWithArea
{
	Facet *facet;
	double area;
};

class Facet
{
public:
	int id;
	int numVertices;
	Plane plane;
	std::vector<int> indVertices;
	unsigned char rgb[3];

	void init_full(const std::vector<int> &index_orig, const bool ifLong);
	void init_empty();

	double calculateAreaAndMaybeCenter(bool ifCalculateCenter, Vector3d &center, Polyhedron &polyhedron);

public:
	// Facet.cpp
	Facet();

	Facet(const int id_orig, const int nv_orig, const Plane plane_orig, const std::vector<int> &index_orig,
		  const bool ifLong);

	Facet(const int id_orig, const int nv_orig, const Plane plane_orig);

	Facet &operator=(const Facet &facet1);
	~Facet();

	bool test_initialization();

	int get_id();
	int get_nv();
	Plane get_plane();
	int get_index(int pos);
	char get_rgb(int pos);

	void get_next_facet(int pos_curr, int &pos_next, int &fid_next, int &v_curr);

	void set_id(int id1);
	void set_rgb(unsigned char red, unsigned char gray, unsigned char blue);
	void set_ind_vertex(int position, int value);

	void find_and_replace_vertex(int from, int to);
	void find_and_replace_facet(int from, int to);

	void delete_vertex(int v);

	void find_next_facet(int v, int &fid_next);
	int find_vertex(int what);

	void add(int what, int pos);
	void remove(int pos);
	void update_info(Polyhedron &polyhedron);

	Vector3d find_mass_centre(Polyhedron &polyhedron);

	// Facet_preprocess.cpp
	void preprocess_free();
	void preprocess(Polyhedron &polyhedron);
	void preprocess_edge(int v0, int v1, int v0_id, Polyhedron &polyhedron);
	int preprocess_search_edge(int v0,
							   int v1); // Searches edge and returns position of v1
	// if success, -1 if not found.
	int preprocess_search_vertex(int v, int &v_next);

	// Facet_io.cpp
	void my_fprint(FILE *file);
	void my_fprint_all(FILE *file, Polyhedron &polyhedron);

	void fprint_default_0(FILE *file);
	void fprint_default_1(FILE *file);
	void fprint_default_1_2(FILE *file);
	void fprint_my_format(FILE *file);

	void fprint_ply_vertex(FILE *file, Polyhedron &polyhedron);
	void fprint_ply_index(FILE *file);
	void fprint_ply_scale(FILE *file);

	void my_fprint_edge_list(FILE *file);

	// Facet_test.cpp
	bool test_self_intersection(Polyhedron &polyhedron);

	// Facet_inertia.cpp
	bool consect_x(double y, double z, double &x, Polyhedron &polyhedron);

	// Facet_area.cpp
	double area(Polyhedron &polyhedron);
	double calculateAreaAndCenter(Vector3d &center, Polyhedron &polyhedron);

	// Facet_verification.cpp
	bool verifyIncidenceStructure(Polyhedron &polyhedron);
	void test_pair_neighbours();
	bool verifyUniqueValues(void);

	/**
	 * Checks whether the plane of facet is correct.
	 *
	 * @return true/false if the plane is correct or not.
	 */
	bool correctPlane();
};

#endif /* FACET_H */
