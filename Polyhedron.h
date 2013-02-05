#ifndef POLYHEDRON_H
#define POLYHEDRON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Vector3d.h"

//#define DEBUG
//#define OUTPUT

#define EPS_SIGNUM 1e-16


class Polyhedron;
class Facet;
class VertexInfo;
class EdgeList;
class FutureFacet;
class EdgeSet;

class Polyhedron
{
public:
	int numv;
	int numf;

	Vector3d* vertex;
	Facet* facet;
	VertexInfo* vertexinfo;
	EdgeList* edge_list;

	
//	FILE* log_file;

public:

	//Polyhedron.cpp
	Polyhedron();
	Polyhedron(
		int numv_orig,
		int numf_orig,
		Vector3d* vertex_orig,
		Facet* facet_orig);
//		FILE* log_file_orig);
	~Polyhedron();

	void get_boundary(
		double& xmin, double& xmax,
		double& ymin, double& ymax,
		double& zmin, double& zmax);

	//Polyhedron_io.cpp
	void my_fprint(const char* filename);
	void my_fprint(FILE* file);

	void fscan_default_0(const char* filename);
	void fscan_default_1(const char* filename);
	void fscan_default_1_1(const char* filename);
	void fscan_my_format(const char* filename);
	void fscan_ply(const char* filename);
	void fprint_default_0(const char* filename);
	void fprint_default_1(const char* filename);
	void fprint_my_format(const char* filename);
	void fprint_ply(const char *filename, const char *comment);
	void fprint_ply_scale(double scale, const char *filename, const char *comment);

	//Polyhedron_preprocess.cpp
	void preprocess_polyhedron();

	//Polyhedron_figures.cpp
	void poly_cube(double h, double x, double y, double z);
	void poly_pyramid(int n, double h, double r);
	void poly_prism(int n, double h, double r);
	void poly_cube_cutted();

	//Polyhedron_intersection.cpp
	void intersect(Plane iplane);
	void intersect_test(int facet_id0, int facet_id1);

	//Polyhedron_join_facets.cpp
	int join_facets_count_nv(int facet_id0, int facet_id1);
	void join_facets_create_vertex_list(int facet_id0, int facet_id1, int nv,
													int* vertex_list, int* edge_list);
	void list_squares_method(int nv, int* vertex_list, Plane* plane);
	void list_squares_method_weight(int nv, int* vertex_list, Plane* plane);


	//Polyhedron.h
	int signum(Vector3d point, Plane plane);
};



class VertexInfo
{
private:
	int id;
	int nf;
	Vector3d vector;
	int* index;
	Polyhedron* poly;

public:
	VertexInfo();
	VertexInfo(
			const int id_orig,
			const int nf_orig,
			const Vector3d vector_orig,
			const int* index_orig,
			Polyhedron* poly_orig);
	VertexInfo(
		const int id_orig,
		const Vector3d vector_orig,
		Polyhedron* poly_orig);

	VertexInfo& operator = (const VertexInfo& orig);
	~VertexInfo();

	void preprocess();

	void fprint_my_format(FILE* file);
	void my_fprint_all(FILE* file);

	int intersection_find_next_facet(Plane iplane, int facet_id);
};

class EdgeList {
private:
	int id;
	int len;
	int num;

	int pointer;

	int* edge0;
	int* edge1;
	int* ind0;
	int* ind1;
	int* next_facet;
	int* next_direction;
	double* scalar_mult;
	int* id_v_new;
	bool* isUsed;
	
	Polyhedron* poly;

public:
	//EdgeList.cpp
	EdgeList();
	EdgeList(int id_orig, int len_orig, Polyhedron* poly_orig);
	EdgeList(const EdgeList& orig);
	~EdgeList();
	EdgeList& operator = (const EdgeList& orig);
	int get_num();
//	void set_facet(Facet* facet_orig);
	void set_id_v_new(int id_v);
	void set_isUsed(bool val);
	void set_pointer(int val);

	void goto_header();
	void go_forward();

	int get_pointer();
	void set_isUsed();

	//EdgeList_intersection.cpp
	void add_edge(int v0, int v1, int i0, int i1, int next_f, int next_d, double sm);
	void add_edge(int v0, int v1, int i0, int i1, double sm);
	void set_curr_info(int next_d, int next_f);

	void search_and_set_info(int v0, int v1, int next_d, int next_f);
	void null_isUsed();
	void get_first_edge(int& v0, int& v1, int& next_f, int& next_d);
	void get_first_edge(int& v0, int& v1);
	void get_next_edge(int& v0, int& v1, int& next_f, int& next_d);
	void get_next_edge(int& v0, int& v1, int& i0, int& i1, int& next_f, int& next_d);

	void send(EdgeSet* edge_set);
	void send_edges(EdgeSet* edge_set);

	//EdgeList_io,cpp
	void my_fprint(FILE* file);
};

class Facet
{
public:
	int id;
	int nv;
	Plane plane;
	int* index;
	Polyhedron* poly;
	char rgb[3];

	EdgeList edge_list;

public:
	//Facet.cpp
	Facet();
	Facet(
		const int id_orig,
		const int nv_orig,
		const Plane plane_orig,
		const int* index_orig,
		Polyhedron* poly_orig,
		const bool ifLong);
	Facet& operator = (const Facet& facet1);
	~Facet();

	int get_id();
	int get_nv();
	Plane get_plane();
	int get_index(int pos);
	char get_rgb(int pos);

	void get_next_facet(
		int pos_curr,
		int& pos_next,
		int& fid_next,
		int& v_curr);

	void set_id(int id1);
	void set_poly(Polyhedron* poly_new);
	void set_rgb(char red, char gray, char blue);

	//Facet_preprocess.cpp
	void preprocess_free();
	void preprocess();
	void preprocess_edge(int v0, int v1, int v0_id);
	int preprocess_search_edge(int v0, int v1); //Searches edge and returns position of v1
									 //if success, -1 if not found.
	int preprocess_search_vertex(int v, int& v_next);

	//Facet_io.cpp
	void my_fprint(FILE* file);
	void my_fprint_all(FILE* file);

	void fprint_default_0(FILE* file);
	void fprint_default_1(FILE* file);
	void fprint_my_format(FILE* file);

	void fprint_ply_vertex(FILE* file);
	void fprint_ply_index(FILE* file);
	void fprint_ply_scale(FILE* file);

	void my_fprint_edge_list(FILE* file);

	//Facet_intersecion.cpp
	int signum(int i, Plane plane);
	int prepare_edge_list(Plane iplane);
	
	bool intersect(
		Plane iplane,
		FutureFacet& ff,
		int& n_components);

	void find_and_replace(int from, int to);

	//Facet_test.cpp
	bool test_self_intersection();
};

class FutureFacet {
private:
	int id;
	int len;
	int nv;
	int* edge0;
	int* edge1;
	int* src_facet;
	int* id_v_new;
public:
	//FutureFacet.cpp
	FutureFacet();
	~FutureFacet();
	FutureFacet(int nv_orig);
	FutureFacet(const FutureFacet& orig);
	FutureFacet& operator = (const FutureFacet& orig);
	FutureFacet& operator += (const FutureFacet& v);
	void free();
	

	int get_nv();

	void set_id(int val);

	void add_edge(int v0, int v1, int src_f);
	void get_edge(int pos, int& v0, int& v1, int& src_f, int& id_v);
	
	//FutureFacet_io.cpp
	void my_fprint(FILE* file);
	
	//FutureFacet_intersection.cpp
	void generate_facet(
		Facet& facet,
		int fid,
		Plane& iplane,
		int numv,
		EdgeSet* es);
};

class EdgeSet {
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
	EdgeSet();
	EdgeSet(int len_orig);
	EdgeSet(const EdgeSet& orig);
	EdgeSet& operator = (const EdgeSet& orig);
	~EdgeSet();

	int get_len();
	int get_num();

	void get_edge(int id, int& v0, int& v1);
	void get_edge(
		int id,
		int& v0,
		int& v1,
		int& id_el0,
		int& pos_el0,
		int& id_el1,
		int& pos_el1,
		int& id_ff,
		int& pos_ff);
	int search_edge(int v0, int v1);
	void add_edge(
		int v0,
		int v1,
		int id_el,
		int pos_el,
		int id_ff,
		int pos_ff);
	void add_edge(
		int v0,
		int v1);

	void test_info();
	void informate_about_new_vertex(
		int numv,
		EdgeList* edge_list,
		FutureFacet* future_facet);

	//EdgeSet_io.cpp
	void my_fprint(FILE* file);
};



#endif // POLYHEDRON_H
