/* 
 * File:   Polyhedron-class.h
 * Author: ilya
 *
 * Created on 12 Ноябрь 2012 г., 11:31
 */

#ifndef POLYHEDRON_CLASS_H
#define	POLYHEDRON_CLASS_H
#include "PolyhedraCorrectionLibrary.h"
#include "SContour.h"

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
	int test_consections(bool ifPrint);
	int test_inner_consections(bool ifPrint);
	int test_inner_consections_facet(bool ifPrint, int fid, double* A, double* b, Vector3d* vertex_old);
	int test_inner_consections_pair(bool ifPrint,
			int fid, int id0, int id1, int id2, int id3, double* A, double* b);
	int test_outer_consections(bool ifPrint);
	int test_outer_consections_facet(bool ifPrint, int fid);
	int test_outer_consections_edge(bool ifPrint, int id0, int id1);
	int test_outer_consections_pair(bool ifPrint, int id0, int id1, int fid);

	//Polyhedron_join_points.cpp
	int join_points(int id);
	int join_points_inner(int id);
	int join_points_inner_facet(int id, int fid, double* A, double* b, Vector3d* vertex_old);
	int join_points_inner_pair(
			int id, int fid, int id0, int id1, int id2, int id3, double* A, double* b);

	//Polyhedron_simplify.cpp
	int simplify_vertex(double eps);
	double dist_vertex(int i, int j);

	//Poyhedron_correction.cpp
	int correct_polyhedron(int N, SContour* contours);

	double corpol_calculate_functional(int nume, Edge* edges, int N,
			SContour* contours, Plane* prevPlanes);
	int corpol_iteration(int nume, Edge* edges, int N, SContour* contours,
			Plane* prevPlanes, double* matrix, double* rightPart,
			double* solution, double* matrixFactorized,
			int* indexPivot);
	void corpol_calculate_matrix(int nume, Edge* edges, int N,
			SContour* contours, Plane* prevPlanes, double* matrix,
			double* rightPart, double* solution);

	// Polyhedron_correction_preprocess.cpp
	int corpol_preprocess(int& nume, Edge* &edges, int N,
			SContour* contours);

	int preprocess_edges(int& nume, int numeMax, Edge* edges);
	inline void preed_add(int& nume, int numeMax, Edge* edges,
			int v0, int v1, int f0, int f1);

	int preed_find(int nume, Edge* edges, int v0, int v1);

	int corpol_prep_build_lists_of_visible_edges(int nume, Edge* edges,
			int N, SContour* contours);
	int corpol_prep_map_between_edges_and_contours(int nume, Edge* edges,
			int N, SContour* contours);
	bool corpol_edgeIsVisibleOnPlane(
			Edge& edge,
			Plane planeOfProjection);
	bool corpol_collinear_visibility(int v0, int v1,
			Plane planeOfProjection, int ifacet);

	// Polyhedron_correction_test.cpp

	int corpolTest(int ncont, double maxMoveDelta);

	SContour& corpolTest_createOneContour(
			int numEdges, Edge* edges,
			int icont, Plane planeOfProjection,
			bool* bufferBool, int* bufferInt0, int* bufferInt1);

	int corpolTest_createAllContours(int numEdges, Edge* edges, int numContours,
			SContour* contours);

	void corpolTest_slightRandomMove(double maxMoveDelta);
	inline void corpolTest_slightRandomMoveFacet(double maxMoveDelta, int ifacet);
	inline void corpolTest_slightRandomMoveVertex(double maxMoveDelta, int ivertex);

	void makeCube(double a);
        
};

#endif	/* POLYHEDRON_CLASS_H */

