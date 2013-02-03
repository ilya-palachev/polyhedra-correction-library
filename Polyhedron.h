#ifndef POLYHEDRON_H
#define POLYHEDRON_H

#include "Vector3d.h"
#include "Facet.h"
#include "VertexInfo.h"

class Polyhedron
{
private:
	Vector3d* vertex;
	Facet* facet;
	VertexInfo* vertexinfo;
	int numv;
	int numf;

public:

	//polyhedron.cpp
	Polyhedron();
        Polyhedron(int numv_orig, int numf_orig, Vector3d* vertex_orig, Facet* facet_orig);
	~Polyhedron();
	Polyhedron& operator=(const Polyhedron& poly);

	//polyhedron_io.cpp
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

	//polyhedron_preprocess.cpp
	void preprocess_polyhedron();
	void preprocess_free_facet();
	void preprocess_facet(int id);
	void preprocess_edge(int v0, int v1, int facet_id, int v0_id);
	void preprocess_vertexinfo(int id);
	int find_edge(int v0, int v1);
	int find_vertex(int f_id, int v);

	//polyhedron_join_facets.cpp
	void join_facets(int facet_id0, int facet_id1);
	void join_facets_cycle(
			int* join_index,
			int& join_nv,
			int& len_vertex,
			int nv,
			int* vertex_list,
			int* edge_list,
			Plane plane,
			int facet_id0,
			int facet_id1
			);
	int join_facets_count_nv(int facet_id0, int facet_id1);
	void join_facets_create_vertex_list(int facet_id0, int facet_id1, int nv,
													int* vertex_list, int* edge_list);
	void list_squares_method(int nv, int* vertex_list, Plane* plane);
	int join_facets_cut_vertex_list(int nv, int* vertex_list, int* edge_list,
												int *vertex_cut);

	//polyhedron_intersetion.cpp
	int signum(int i, Plane plane);
	Vector3d intersection_edge(int i0, int i1, Plane plane);

	int intersection_facet_create_edge_list(
			int facet_id, Plane plane, int* edge_list);
	void test_intersection_facet_create_edge_list(Plane plane);
	void intersection_facet_step(
			Plane plane,
			int facet_id, int v0, int v1,
			int& facet_id_new, int& v0_new, int& v1_new);
	void intersection_facet_step_cut(
			Plane plane,
			int facet_id, int v0, int v1,
			int& facet_id_new, int& v0_new, int& v1_new,
			int new_vertex, int& len_vertex);

	//polyhedron_figures.cpp
	void poly_cube(double h, double x, double y, double z);
	void poly_pyramid(int n, double h, double r);
	void poly_prism(int n, double h, double r);
};

#endif // POLYHEDRON_H
