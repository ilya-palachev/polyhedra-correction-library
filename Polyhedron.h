#ifndef POLYHEDRON_H
#define POLYHEDRON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Vector3d.h"

class Polyhedron;
class Facet;
class VertexInfo;
class EdgeList;


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

	bool isPreprocessed;

	bool isPreparedIntersection;
	Plane iplane;
	int num_edges;
	int* edge_list;

public:
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

	void set_poly(Polyhedron* poly_new);

	void my_fprint(FILE* file);
	void my_fprint_all(FILE* file);

	void preprocess_free();
	void preprocess();
	void preprocess_edge(int v0, int v1, int v0_id);
	int preprocess_search_edge(int v0, int v1); //Searches edge and returns position of v1
									 //if success, -1 if not found.
	int preprocess_search_vertex(int v, int& v_next);
	
	void get_next_facet(
		int pos_curr,
		int& pos_next,
		int& fid_next,
		int& v_curr);

	void fprint_default_0(FILE* file);
	void fprint_default_1(FILE* file);
	void fprint_my_format(FILE* file);

	void fprint_ply_vertex(FILE* file);
	void fprint_ply_index(FILE* file);
	void fprint_ply_scale(FILE* file);

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
};

class EdgeList {
private:
	int num;
	int* edge0;
	int* edge1;
	double* scalar_mult;
	Facet* facet;
public:
	EdgeList();
	EdgeList(Facet* facet_orig);
	~EdgeList();
	EdgeList& operator=(const EdgeList& orig);

	void add_edge(int v0, int v1, double sm);
	void get_next_edge(int& v0, int& v1);
};

#endif // POLYHEDRON_H
