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
	bool fscan_default_1_2(const char* filename);
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

	//Polyhedron_group_vertices.cpp
	int groupVertices(int id);

	//Polyhedron_size.cpp
	double calculate_J11(int N);
	double volume();
	double areaOfSurface();
	double areaOfFacet(int iFacet);
	void J(double& Jxx, double& Jyy, double& Jzz, double& Jxy, double& Jyz,
			double& Jxz);
	void get_center(double& xc, double& yc, double& zc);
	void inertia(double& l0, double& l1, double& l2, Vector3d& v0, Vector3d& v1,
			Vector3d& v2);

	//Polyhedron_clusterize.cpp
	int clusterize(double p);
	void clusterize2(double p);
	TreeClusterNorm& build_TreeClusterNorm();
	void giveClusterNodeArray(TreeClusterNormNode* nodeArray,
			MatrixDistNorm& matrix);
	void reClusterNodeArray(TreeClusterNormNode* nodeArray_in,
			MatrixDistNorm& matrix_in, TreeClusterNormNode* nodeArray_out,
			MatrixDistNorm& matrix_out);

	// Polyhedron_correction.cpp
	void correctGlobal(ShadeContourData* contourData,
			GSCorrectorParameters* parameters);

	// Polyhedron_verification.cpp
	int test_structure();
	int countConsections(bool ifPrint);
	int checkEdges(EdgeData* edgeData);
};

#endif	/* POLYHEDRON_CLASS_H */

