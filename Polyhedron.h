#ifndef POLYHEDRON_H
#define POLYHEDRON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Vector3d.h"

//#define DEBUG
//#define DEBUG1
#define OUTPUT
//#define TCPRINT //Печатать вывод из Polyhedron::test_consections()
#define EPS_SIGNUM 1e-15


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
	Polyhedron(
		int numv_orig,
		int numf_orig,
		Vector3d* vertex_orig,
		Facet* facet_orig,
                VertexInfo* vertexinfo);
	~Polyhedron();

	void get_boundary(
		double& xmin, double& xmax,
		double& ymin, double& ymax,
		double& zmin, double& zmax);
        
        void delete_empty_facets();

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
        void set_isUsed(int v0, int v1, bool val);
	void intersect(Plane iplane);
	void intersect_test(int facet_id0, int facet_id1);

	//Polyhedron_join_facets.cpp
	int join_facets_count_nv(int facet_id0, int facet_id1);
	void join_facets_create_vertex_list(int facet_id0, int facet_id1, int nv,
                int* vertex_list, int* edge_list);
	void list_squares_method(int nv, int* vertex_list, Plane* plane);
	void list_squares_method_weight(int nv, int* vertex_list, Plane* plane);

        void join_create_first_facet(int fid0, int fid1);
        void delete_vertex_polyhedron(int v);
        int add_vertex(Vector3d& vec);
        void print_vertex(int i);
       
        void clear_unused();
        void find_and_replace_vertex(int from, int to);
        void find_and_replace_facet(int from, int to);
        
        /////////////////////////////////////////
        void join_facets(int fid0, int fid1);
        void multi_join_facets(int n, int* fid);
//        void join_facets_calculate_plane(int fid0, int fid1, Plane& plane, int& nv);
        void join_facets_calculate_plane(int fid0, int fid1, Facet& join_facet, Plane& plane);
        void multi_join_facets_calculate_plane(int n, int* fid, Facet& join_facet, Plane& plane);
        void join_facets_build_index(int fid0, int fid1, Plane& plane, Facet& join_facet, int& nv);
        void multi_join_facets_build_index(int n, int* fid, Facet& join_facet, int& nv);
        void join_facets_rise(int fid0);
        void join_facets_rise_find(int fid0, int& imin);
        void join_facets_rise_find_step(int fid0, int i, double& d);
        void join_facets_rise_point(int fid0, int imin);
        
        int test_structure();
        
        //Polyhedron_intersection_j.cpp
        void intersect_j(Plane iplane, int jfid);

	//Polyhedron.h
	int signum(Vector3d point, Plane plane);
        
        //Polyhedron_deform.cpp
        void deform(int id, Vector3d delta);
        void f(int n, double* x, double* fx, int id, int* sum);
        void derf(int n, double* x, double* A, int id, int* sum);
        void derf2(int n, double* x, double* A, int id, int* sum, 
                double* tmp0, double* tmp1, double* tmp2, double* tmp3);
        
        void deform_w(int id, Vector3d delta);
        void f_w(int n, double* x, double* fx, int id, bool* khi, int K);
        void derf_w(int n, double* x, double* A, int id, bool* khi, int K, 
                double* tmp0, double* tmp1, double* tmp2, double* tmp3);
        
        //Polyhedron_deform_linear.cpp
        void deform_linear(int id, Vector3d delta);
        void deform_linear2(int id, Vector3d delta);
        void deform_linear_partial(int id, Vector3d delta, int num);
        void deform_linear_facets(double* x, double* y, double* z);
        void deform_linear_vertices(int id, double K, double* A, double* B, double* x, double* y, double* z);
        void deform_linear_vertices(double K, double* A, double* B);
        double deform_linear_calculate_error();
        double deform_linear_calculate_deform(double* x, double* y, double* z);
        double min_dist(int id);
        void import_coordinates(Polyhedron& orig);

        void deform_linear_test(int id, Vector3d delta, int mode, int& num_steps, double& norm_sum);
        
        //Polyhedron_test_consections.cpp
        int test_consections();
        int test_inner_consections();
        int test_inner_consections_facet(int fid, double* A, double* b, Vector3d* vertex_old);
        int test_inner_consections_pair(
                int fid, int id0, int id1, int id2, int id3, double* A, double* b);
        int test_outer_consections();
        int test_outer_consections_facet(int fid);
        int test_outer_consections_edge(int id0, int id1);
        int test_outer_consections_pair(int id0, int id1, int fid);
        
        //Polyhedron_simplify.cpp
        int simplify_vertex(double eps);
        double dist_vertex(int i, int j);
        
};



class VertexInfo
{
public:
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

        int get_nf();
	void preprocess();
        
        void find_and_replace_facet(int from, int to);
        void find_and_replace_vertex(int from, int to);

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
	void set_isUsed(int v0, int v1, bool val);

	//EdgeList_intersection.cpp
	void add_edge(int v0, int v1, int i0, int i1, int next_f, int next_d, double sm);
	void add_edge(int v0, int v1, int i0, int i1, double sm);
	void set_curr_info(int next_d, int next_f);

	void search_and_set_info(int v0, int v1, int next_d, int next_f);
	void null_isUsed();
	void get_first_edge(int& v0, int& v1, int& next_f, int& next_d);
	void get_first_edge(int& v0, int& v1);
	void get_next_edge(Plane iplane, int& v0, int& v1, int& next_f, int& next_d);
	void get_next_edge(Plane iplane, int& v0, int& v1, int& i0, int& i1, int& next_f, int& next_d);

	void send(EdgeSet* edge_set);
	void send_edges(EdgeSet* edge_set);
        
        void set_poly(Polyhedron* poly_orig);

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

	void find_and_replace_vertex(int from, int to);
	void find_and_replace_facet(int from, int to);

	//Facet_test.cpp
	bool test_self_intersection();
        
        //Facet_join_facets.cpp
        void clear_bad_vertexes();
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
