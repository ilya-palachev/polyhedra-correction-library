/* 
 * File:   Facet.h
 * Author: ilya
 *
 * Created on 12 Ноябрь 2012 г., 11:29
 */

#ifndef FACET_H
#define	FACET_H

class Facet
{
public:
	int id;
	int numVertices;
	Plane plane;
	int* indVertices;
	Polyhedron* parentPolyhedron;
	char rgb[3];

	EdgeList edgeList;

public:
	// Facet.cpp
	Facet();
	Facet(const int id_orig, const int nv_orig, const Plane plane_orig,
			const int* index_orig, Polyhedron* poly_orig, const bool ifLong);
	Facet(const int id_orig, const int nv_orig, const Plane plane_orig,
			Polyhedron* poly_orig);
	Facet& operator =(const Facet& facet1);
	~Facet();
	bool test_initialization();

	int get_id();
	int get_nv();
	Plane get_plane();
	int get_index(int pos);
	char get_rgb(int pos);

	void get_next_facet(int pos_curr, int& pos_next, int& fid_next,
			int& v_curr);

	void set_id(int id1);
	void set_poly(Polyhedron* poly_new);
	void set_rgb(char red, char gray, char blue);
	void set_ind_vertex(int position, int value);

	int signum(int i, Plane plane);
	void find_and_replace_vertex(int from, int to);
	void find_and_replace_facet(int from, int to);

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

	// Facet_join_facets.cpp
	void delete_vertex(int v);
	void add_before_position(int pos, int v, int next_f);
	void add_after_position(int pos, int v, int next_f);

	void find_next_facet(int v, int& fid_next);
	void find_next_facet2(int v, int& fid_next);
	void find_and_replace2(int from, int to);
	int find_total(int what);
	int find_vertex(int what);

	void add(int what, int pos);
	void remove(int pos);
	void update_info();
	int test_structure();

	Vector3d& find_mass_centre();

	void test_pair_neighbours();

	// Facet_inertia.cpp
	bool consect_x(double y, double z, double& x);

	// Facet_area.cpp
	double area();

};

#endif	/* FACET_H */

