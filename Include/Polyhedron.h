#ifndef POLYHEDRON_CLASS_H
#define	 POLYHEDRON_CLASS_H

const int INT_NOT_INITIALIZED = -RAND_MAX;
const double EPS_COLLINEARITY = 1e-14;

class Polyhedron
{
public:
	int numVertices;
	int numFacets;

	Vector3d* vertices;
	Facet* facets;
	VertexInfo* vertexInfos;

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
	int signum(Vector3d point, Plane plane);

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
	void preprocessAdjacency();

	//Polyhedron_intersection.cpp
	void intersect(Plane iplane);
	void intersectCoalesceMode(Plane iplane, int jfid);

	//Polyhedron_coalesce_facets.cpp
	void coalesceFacets(int fid0, int fid1);
	void coalesceFacets(int n, int* fid);

	//Polyhedron_shift_point.cpp
	void shiftPoint(int id, Vector3d delta);
	void shiftPointWeighted(int id, Vector3d delta);
	void shiftPointLinearGlobal(int id, Vector3d delta);
	void shiftPointLinearLocal(int id, Vector3d delta);
	void shiftPointLinearTest(int id, Vector3d delta, int mode, int& num_steps,
			double& norm_sum);
	void shiftPointLinearPartial(int id, Vector3d delta, int num);

	double distToNearestNeighbour(int id);
	void copyCoordinates(Polyhedron& orig);


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

	// Polyhedron_verification.cpp
	int test_structure();
};

#endif	/* POLYHEDRON_CLASS_H */

