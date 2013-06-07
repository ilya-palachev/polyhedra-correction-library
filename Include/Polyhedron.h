#ifndef POLYHEDRON_CLASS_H
#define	POLYHEDRON_CLASS_H

const int INT_NOT_INITIALIZED = -RAND_MAX;

const double EPSILON_EDGE_CONTOUR_VISIBILITY = 1e-6;
const double EPS_SIGNUM = 1e-15;
const double EPS_COLLINEARITY = 1e-14;
const double EPS_SAME_POINTS = 1e-16;

const double DEFAULT_ERROR_FOR_DOUBLE_FUNCTIONS = -RAND_MAX;

const double DEFAULT_DERIVATIVE_STEP = 1e-8;
const double DEFAULT_DERIVATIVE_STEP_RECIPROCAL = 1e+8;
const double EPSILON_FOR_WARNING_IN_DERIVATIVE_TESTING_ABSOLUTE = 1e-2;
const double EPSILON_FOR_WARNING_IN_DERIVATIVE_TESTING_RELATIVE = 0.5;

const double EPSILON_FOR_DIVISION = 1e-16;

const double POROG = 0.5;

const double ASSOCIATOR_MIN_PORTION_REL = 0.1;

const double THRESHOLD_ESSENTIAL_ASSOCIATION = 0.1;

enum Orientation
{
	ORIENTATION_LEFT, ORIENTATION_RIGHT
};

class Polyhedron
{
public:
	int numVertices;
	int numFacets;

	Vector3d* vertices;
	Facet* facets;
	VertexInfo* vertexInfos;
	EdgeList* edgeLists;

public:

	//Polyhedron.cpp
	Polyhedron();
	Polyhedron(int numv_orig, int numf_orig);
	Polyhedron(int numv_orig, int numf_orig, Vector3d* vertex_orig,
			Facet* facet_orig);
	Polyhedron(int numv_orig, int numf_orig, Vector3d* vertex_orig,
			Facet* facet_orig, VertexInfo* vertexinfo);
	~Polyhedron();

	void get_boundary(double& xmin, double& xmax, double& ymin, double& ymax,
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
	void fprint_ply_scale(double scale, const char *filename,
			const char *comment);

	//Polyhedron_preprocess.cpp
	void preprocess_polyhedron();

	//Polyhedron_figures.cpp

	void deleteContent();

	void makeCube(double height, double xCenter, double yCenter,
			double zCenter);

	void makePyramid(int numVerticesBase, double height, double radius);

	void makePrism(int numVerticesBase, double height, double radius);

	void makeCubeCutted();

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
	void set_vertex(int position, Vector3d vec);
	void print_vertex(int i);

	void clear_unused();
	void find_and_replace_vertex(int from, int to);
	void find_and_replace_facet(int from, int to);

	/////////////////////////////////////////
	void join_facets(int fid0, int fid1);
	void multi_join_facets(int n, int* fid);
	//        void join_facets_calculate_plane(int fid0, int fid1, Plane& plane, int& nv);
	void join_facets_calculate_plane(int fid0, int fid1, Facet& join_facet,
			Plane& plane);
	void multi_join_facets_calculate_plane(int n, int* fid, Facet& join_facet,
			Plane& plane);
	void join_facets_build_index(int fid0, int fid1, Plane& plane,
			Facet& join_facet, int& nv);
	void multi_join_facets_build_index(int n, int* fid, Facet& join_facet,
			int& nv);
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
	void derf2(int n, double* x, double* A, int id, int* sum, double* tmp0,
			double* tmp1, double* tmp2, double* tmp3);

	void deform_w(int id, Vector3d delta);
	void f_w(int n, double* x, double* fx, int id, bool* khi, int K);
	void derf_w(int n, double* x, double* A, int id, bool* khi, int K,
			double* tmp0, double* tmp1, double* tmp2, double* tmp3);

	//Polyhedron_deform_linear.cpp
	void deform_linear(int id, Vector3d delta);
	void deform_linear2(int id, Vector3d delta);
	void deform_linear_partial(int id, Vector3d delta, int num);
	void deform_linear_facets(double* x, double* y, double* z);
	void deform_linear_vertices(int id, double K, double* A, double* B,
			double* x, double* y, double* z);
	void deform_linear_vertices(double K, double* A, double* B);
	double deform_linear_calculate_error();
	double deform_linear_calculate_deform(double* x, double* y, double* z);
	double min_dist(int id);
	void import_coordinates(Polyhedron& orig);

	void deform_linear_test(int id, Vector3d delta, int mode, int& num_steps,
			double& norm_sum);

	//Polyhedron_test_consections.cpp
	int test_consections(bool ifPrint);
	int test_inner_consections(bool ifPrint);
	int test_inner_consections_facet(bool ifPrint, int fid, double* A,
			double* b, Vector3d* vertex_old);
	int test_inner_consections_pair(bool ifPrint, int fid, int id0, int id1,
			int id2, int id3, double* A, double* b);
	int test_outer_consections(bool ifPrint);
	int test_outer_consections_facet(bool ifPrint, int fid);
	int test_outer_consections_edge(bool ifPrint, int id0, int id1);
	int test_outer_consections_pair(bool ifPrint, int id0, int id1, int fid);

	//Polyhedron_join_points.cpp
	int join_points(int id);
	int join_points_inner(int id);
	int join_points_inner_facet(int id, int fid, double* A, double* b,
			Vector3d* vertex_old);
	int join_points_inner_pair(int id, int fid, int id0, int id1, int id2,
			int id3, double* A, double* b);

	//Polyhedron_simplify.cpp
	int simplify_vertex(double eps);
	double dist_vertex(int i, int j);

	//Polyhedron_inertia.cpp
	double calculate_J11(int N);

	double consection_x(double y, double z);

	//Polyhedron_volume.cpp
	double volume();
	double area();
	double area(int iFacet);
	void J(double& Jxx, double& Jyy, double& Jzz, double& Jxy, double& Jyz,
			double& Jxz);
	void get_center(double& xc, double& yc, double& zc);
	void inertia(double& l0, double& l1, double& l2, Vector3d& v0, Vector3d& v1,
			Vector3d& v2);

	//Polyhedron_cluster.cpp
	int clusterisation(double p);
	void clusterisation2(double p);

	//Polyhedron_hierarchical_clustering.cpp
	TreeClusterNorm& build_TreeClusterNorm();
	void giveClusterNodeArray(TreeClusterNormNode* nodeArray,
			MatrixDistNorm& matrix);
	void reClusterNodeArray(TreeClusterNormNode* nodeArray_in,
			MatrixDistNorm& matrix_in, TreeClusterNormNode* nodeArray_out,
			MatrixDistNorm& matrix_out);

	// Polyhedron_correction.cpp
	void correctGlobal(ShadeContourData* contourData);

};

#endif	/* POLYHEDRON_CLASS_H */

