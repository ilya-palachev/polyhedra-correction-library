#ifndef POLYHEDRON_H
#define POLYHEDRON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Vector3d.h"

#define DEBUG

#define EPS_SIGNUM 1e-14


class Polyhedron;
class Facet;
class VertexInfo;
class EdgeList;
class FutureFacet;
class EdgeSet;

class Polyhedron
{
public:
	Vector3d* vertex;
	Facet* facet;
	VertexInfo* vertexinfo;
	int numv;
	int numf;

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
	void intersection(Plane iplane);

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

	VertexInfo& operator=(const VertexInfo& orig);
	~VertexInfo();

	void preprocess();

	void fprint_my_format(FILE* file);
	void my_fprint_all(FILE* file);

	int intersection_find_next_facet(Plane iplane, int facet_id);
};

class EdgeList {
private:
	int num;
	int* edge0;
	int* edge1;
	int* next_facet;
	int* next_direction;
	double* scalar_mult;
	Facet* facet;
public:
	//EdgeList.cpp
	EdgeList();
	EdgeList(Facet* facet_orig);
	~EdgeList();
	EdgeList& operator=(const EdgeList& orig);
	void set_facet(Facet* facet_orig);

	//EdgeList_intersection.cpp
	void add_edge(int v0, int v1, int next_f, int next_d, double sm);
	void prepare_next_direction();
	void get_next_edge(int& v0, int& v1);

	//EdgeList_io,cpp
	void my_fprint(FILE* file);
};

class Facet
{
private:
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
	Facet& operator=(const Facet& facet1);
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

	void set_poly(Polyhedron* poly_new);

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
//	void intersection();

	//Facet_test.cpp
	bool test_self_intersection();
};

class FutureFacet {
private:
	int len;
	int nv;
	int* edge0;
	int* edge1;
	int* src_facet;
public:
	FutureFacet();
	~FutureFacet();
	FutureFacet(int nv_orig);
	FutureFacet(const FutureFacet& orig);
	FutureFacet& operator = (const FutureFacet& orig);

	void add_edge(int v0, int v1);
	Facet make_facet();
};

class EdgeSet {
private:
	int len;
	int num;
	int* ind;
	int* edge0;
	int* edge1;
public:
	EdgeSet();
	EdgeSet(int len_orig);
	EdgeSet(const EdgeSet& orig);
	EdgeSet& operator = (const EdgeSet& orig);
	~EdgeSet();

	int get_len();
	int get_num();

	void get_edge(int id, int& v0, int& v1);
	int search_edge(int v0, int v1);
	int add_edge(int v0, int v1);
};


#endif // POLYHEDRON_H
