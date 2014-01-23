/*
 * Copyright (c) 2009-2014 Ilya Palachev <iliyapalachev@gmail.com>
 *
 * This file is part of Polyhedra Correction Library.
 *
 * Polyhedra Correction Library is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Polyhedra Correction Library is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Polyhedra Correction Library.
 * If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file Polyhedron.h
 * @brief The declaration of Polyhedron - the main class of the project.
 */

#ifndef POLYHEDRON_CLASS_H
#define	 POLYHEDRON_CLASS_H

#include <memory>
#include <list>

/*
 * Forward declarations
 *
 * TODO: they should not be here.
 */
class Vector3d;
class Plane;
class Facet;
class VertexInfo;
class TreeClusterNorm;
class TreeClusterNormNode;
class MatrixDistNorm;
class ShadeContourData;
class EdgeData;

struct _GSCorrectorParameters;

using namespace std;

typedef struct _GSCorrectorParameters GSCorrectorParameters;
typedef shared_ptr<EdgeData> EdgeDataPtr;

/**
 * The implementation of Polyhedron.
 *
 * All methods that change, correct it, etc are done outside this class. This
 * is done to avoid the c++ anti-pattern "huge class".
 *
 * Thus we have separate class for every method that does some big change or
 * correction of polyhedron.
 */
class Polyhedron : public enable_shared_from_this<Polyhedron>
{
public:

	/**
	 * Number of vertices in the polyhedron
	 */
	int numVertices;

	/**
	 * Number of facets in the polyhedron
	 */
	int numFacets;

	/**
	 * Array of vertices
	 */
	Vector3d* vertices;

	/**
	 * Array of facets
	 */
	Facet* facets;

	/**
	 * Array of vertexinfos. Vertexinfo saves information about which facets
	 * the vertex is contained in.
	 */
	VertexInfo* vertexInfos;

protected:

	/**
	 * Enables "shared from this" feature.
	 */
	inline shared_ptr<Polyhedron> get_ptr()
	{
		return shared_from_this();
	}

public:

	/*
	 * ========================================================================
	 * Contained in source file Polyhedron.cpp
	 *
	 * Constructors, destructors and commonly used functions
	 * ========================================================================
	 */

	/**
	 * Creates empty polyhedron.
	 */
	Polyhedron();

	/**
	 * Creates polyhedron with given number of vertices and facets.
	 *
	 * @param numv_orig The number of vertices
	 * @param numf_orig The number of facets
	 */
	Polyhedron(int numv_orig, int numf_orig);

	/**
	 * Creates polyhedron with given number of vertices and facets, with fixed
	 * arrays of vertices and facets.
	 *
	 * @param numv_orig		The number of vertices
	 * @param numf_orig		The number of facets
	 * @param vertex_orig	The array of vertices
	 * @param facet_orig	The array of facets
	 */
	Polyhedron(int numv_orig, int numf_orig, Vector3d* vertex_orig,
			Facet* facet_orig);

	/**
	 * Deletes the polyhedron.
	 */
	~Polyhedron();

	/**
	 * Finds the the coordinates of bounding box of the polyhedron.
	 *
	 * @param xmin	A reference where the minimal x will be stored in.
	 * @param xmax	A reference where the maximal x will be stored in.
	 * @param ymin	A reference where the minimal y will be stored in.
	 * @param ymax	A reference where the maximal y will be stored in.
	 * @param zmin	A reference where the minimal z will be stored in.
	 * @param zmax	A reference where the maximal z will be stored in.
	 */
	void get_boundary(double& xmin, double& xmax, double& ymin, double& ymax,
			double& zmin, double& zmax);

	/**
	 * Deletes facets that contain zero number of vertices. Such situation can
	 * happen after the work of some algorithms.
	 */
	void delete_empty_facets();

	/**
	 * Calcualates the signum of the point relative to the plane.
	 * @param point	The point
	 * @param plane	The plane
	 */
	int signum(Vector3d point, Plane plane);
	
	/**
	 * Sets the shared pointer to the parent polyhedron in all facetsof the
	 * polyhedron.
	 */
	void set_parent_polyhedron_in_facets();

	/*
	 * ========================================================================
	 * Contained in source file Polyhedron_io.cpp
	 *
	 * Functions for reading and writing polyhedron from and to different
	 * possible formats.
	 * ========================================================================
	 */

	/*
	 * TODO: Here must be only 2 functions: fscan and fprint that take the
	 * type of file as the enum.
	 */

	/**
	 * Writes the polyhedron to the file in internal format.
	 * This format contained verbose output of all dumping functions for
	 * every structure.
	 *
	 * @param filename	The name of file.
	 */
	void my_fprint(const char* filename);

	/**
	 * Writes the polyhedron to the file in internal format.
	 * This format contained verbose output of all dumping functions for
	 * every structure.
	 *
	 * @param filename	Opened file descriptor
	 */
	void my_fprint(FILE* file);

	/**
	 * Reads the polyhedron from the file in default format.
	 *
	 * @param filename	The name of file.
	 *
	 * Here is the format:
	 *
	 * numv numf
	 *
	 * x_{i} y_{i} z_{i}
	 *
	 * nv_{j} a_{j} b_{j} c_{j} d_{j}   k_{j_{1}} k_{j_{2}} ... k_{j_{nv_{j}}}
	 */
	void fscan_default_0(const char* filename);

	/**
	 * Reads the polyhedron from the file in default format.
	 *
	 * @param filename	The name of file.
	 *
	 * Here is the format:
	 *
	 * numv numf
	 *
	 * i x_{i} y_{i} z_{i}
	 *
	 * j nv_{j} a_{j} b_{j} c_{j} d_{j} k_{j_{1}} k_{j_{2}} ... k_{j_{nv_{j}}}
	 */
	void fscan_default_1(const char* filename);

	/**
	 * Reads the polyhedron from the file in default format.
	 *
	 * @param filename	The name of file.
	 *
	 * Here is the format:
	 *
	 * #WPolyhedron
	 * #version 1
	 * #dimension
	 * numv numf
	 * #vertices
	 * i x_{i} y_{i} z_{i}
	 * #facets
	 * j nv_{j} a_{j} b_{j} c_{j} d_{j} k_{j_{1}} k_{j_{2}} ... k_{j_{nv_{j}}}
	 */
	void fscan_default_1_1(const char* filename);

	/**
	 * Reads the polyhedron from the file in default format.
	 *
	 * @param filename	The name of file.
	 *
	 * Here is the format:
	 *
	 * TODO: describe the format here.
	 */
	bool fscan_default_1_2(const char* filename);

	/**
	 * Reads the polyhedron from the file in internal format.
	 *
	 * @param filename	The name of file.
	 *
	 * Here is the format:
	 *
	 * numv numf
	 *
	 * x_{i} y_{i} z_{i}
	 *
	 * nv_{j} a_{j} b_{j} c_{j} \
	 * d_{j} k_{j_{1}} k_{j_{2}} ... k_{j_{3 * nv_{j} - 1}}
	 *
	 * The special feature of the format is that the incidence information is
	 * also written to the file.
	 */
	void fscan_my_format(const char* filename);

	/**
	 * Reads the polyhedron from the file in Stanford PLY format.
	 *
	 * @param filename	The name of file
	 *
	 * The specification of PLY format can be found at:
	 * http://en.wikipedia.org/wiki/PLY_%28file_format%29
	 * http://paulbourke.net/dataformats/ply/
	 *
	 * This version of PLY scanner in too hard-coded, since it assumes that the
	 * length of header is 11 lines and ignores all its syntax. Actually it can
	 * only parse one specific case of syntax.
	 *
	 * TODO: It will be much better to use some open-source library to parse
	 * general PLY files (maybe RPLY).
	 */
	void fscan_ply(const char* filename);

	/**
	 * Writes the polyhedron to the file in default format.
	 *
	 * @param filename	The name of file
	 *
	 * Here is the format:
	 *
	 * numv numf
	 *
	 * x_{i} y_{i} z_{i}
	 *
	 * nv_{j} a_{j} b_{j} c_{j} d_{j}   k_{j_{1}} k_{j_{2}} ... k_{j_{nv_{j}}}
	 */
	void fprint_default_0(const char* filename);

	/**
	 * Writes the polyhedron to the file in default format.
	 *
	 * @param filename	The name of file.
	 *
	 * Here is the format:
	 *
	 * numv numf
	 *
	 * i x_{i} y_{i} z_{i}
	 *
	 * j nv_{j} a_{j} b_{j} c_{j} d_{j} k_{j_{1}} k_{j_{2}} ... k_{j_{nv_{j}}}
	 */
	void fprint_default_1(const char* filename);

	/**
	 * Writes the polyhedron to the file in internal format.
	 *
	 * @param filename	The name of file.
	 *
	 * Here is the format:
	 *
	 * numv numf
	 *
	 * x_{i} y_{i} z_{i}
	 *
	 * nv_{j} a_{j} b_{j} c_{j} \
	 * d_{j} k_{j_{1}} k_{j_{2}} ... k_{j_{3 * nv_{j} - 1}}
	 *
	 * The special feature of the format is that the incidence information is
	 * also written to the file.
	 */
	void fprint_my_format(const char* filename);

	/**
	 * Writes the polyhedron to the file in Stanford PLY format.
	 *
	 * @param filename	The name of file
	 * @param comment	The comment to be written to the file. It must contain
	 * only one word.
	 *
	 * The specification of PLY format can be found at:
	 * http://en.wikipedia.org/wiki/PLY_%28file_format%29
	 * http://paulbourke.net/dataformats/ply/
	 *
	 * TODO: It will be much better to use some open-source library to parse
	 * general PLY files (maybe RPLY).
	 */
	void fprint_ply(const char *filename, const char *comment);

	/**
	 * Writes the polyhedron to the file in Stanford PLY format.
	 * Before writing the coordinates of vertices are scaled up.
	 *
	 * @param scale		The scaling factor
	 * @param filename	The name of file
	 * @param comment	The comment to be written to the file. It must contain
	 * only one word.
	 *
	 * The specification of PLY format can be found at:
	 * http://en.wikipedia.org/wiki/PLY_%28file_format%29
	 * http://paulbourke.net/dataformats/ply/
	 *
	 * TODO: It will be much better to use some open-source library to parse
	 * general PLY files (maybe RPLY).
	 */
	void fprint_ply_scale(double scale, const char *filename,
			const char *comment);

	/*
	 * ========================================================================
	 * Contained in source file Polyhedron_preprocess.cpp
	 *
	 * Functions for preprocessing polyhedra
	 * ========================================================================
	 */

	/**
	 * Constructs the information about incidence structure of the polyhedron.
	 * The information for facet is stored in a single array and looks as
	 * follows:
	 *
	 * ------------------------------------------------------------------
	 * | v0 | v1 | .. | vn | v0 | f0 | f1 | .. | fn | p0 | p1 | .. | pn |
	 * ------------------------------------------------------------------
	 * \_______   _________/ ^  \________   ________/\________   _______/
	 *         \ /           |           \ /                  \ /
	 *          |            |            |                    |
	 *       vertices     cycling      neighbor               positions of
	 *                    vertex       facets                 vertices
	 *                                 incident               in neighbor
	 *                                 to edges               facets
	 *
	 * The information for vertex is stored in a single array inside vertexinfo
	 * with the same ID and looks as follows:
	 *
	 * ------------------------------------------------------------------
	 * | f0 | f1 | .. | fn | f0 | v0 | v1 | .. | vn | p0 | p1 | .. | pn |
	 * ------------------------------------------------------------------
	 * \_______   _________/ ^  \________   ________/\________   _______/
	 *         \ /           |           \ /                  \ /
	 *          |            |            |                    |
	 *       facets       cycling      neighbor               positions of
	 *                    facet        vertices               vertices
	 *                                 in facets              in incident
	 *                                                        facets
	 */
	void preprocessAdjacency();

	/*
	 * ========================================================================
	 * Contained in source file Polyhedron_intersection.cpp
	 *
	 * Functions for intersecting polyhedron with the plane.
	 * ========================================================================
	 */

	/**
	 * Intersects polyhedron with the plane. Can handle special cases when the
	 * polyhedron is non-convex  and the intersection contains multiple
	 * components.
	 *
	 * @param iplane	Intersecting plane
	 */
	void intersect(Plane iplane);

	/**
	 * Intersects polyhedron with the plane. Can handle special cases when the
	 * polyhedron is non-convex  and the intersection contains multiple
	 * components.
	 *
	 * This version is adjusted for usage via coalescer.
	 *
	 * @param iplane	Intersecting plane
	 */
	void intersectCoalesceMode(Plane iplane, int jfid);

	/*
	 * ========================================================================
	 * Contained in source file Polyhedron_coalesce_facets.cpp
	 *
	 * Functions for intersecting polyhedron with the plane.
	 * ========================================================================
	 */

	/**
	 * Coalesces 2 adjacent facets into single one.
	 *
	 * This function is used to fix wrongly divided facets, when one big facet
	 * is divided into 2 smaller ones.
	 *
	 * @param fid0	The ID of the first facet
	 * @param fid1	The ID of the second facet
	 *
	 * TODO: For code re-using it will be better call function
	 * coalesceFacets(int n, int* fid)
	 * with n = 2 and fid[] = {fid0, fid1}
	 * To do this we need first verify that the result is the same as before.
	 */
	void coalesceFacets(int fid0, int fid1);

	/**
	 * Coalesces several adjacent facets into single one.
	 *
	 * This function is used to fix wrongly divided facets, when one big facet
	 * is divided into several smaller ones.
	 *
	 * @param fid0	Number of facets
	 * @param fid1	Array of IDs of facets
	 */
	void coalesceFacets(int n, int* fid);

	/*
	 * ========================================================================
	 * Contained in source file Polyhedron_shift_point.cpp
	 *
	 * Functions for shifting one vertex on the vector displacement and then
	 * reconstructing the planarity of all facets.
	 *
	 * Also the are 2 functions that are used outside the coalescer. Thus, they
	 * are here.
	 * ========================================================================
	 */

	/**
	 * Shifts one vertex on the vector displacement and then reconstructs
	 * the planarity of all facets.
	 *
	 * @param id	The ID of shifted vertex
	 * @param delta	Displacement vector
	 *
	 * TODO: This algorithm is not actually working now.
	 */
	void shiftPoint(int id, Vector3d delta);

	/**
	 * Shifts one vertex on the vector displacement and then reconstructs
	 * the planarity of all facets. The minimized functional's summands are
	 * used with weights.
	 *
	 * @param id	The ID of shifted vertex
	 * @param delta	Displacement vector
	 *
	 * TODO: This algorithm is not actually working now.
	 */
	void shiftPointWeighted(int id, Vector3d delta);

	/**
	 * Shifts one vertex on the vector displacement and then reconstructs
	 * the planarity of all facets, the functional is linearized in order to
	 * simplify the algorithm.
	 *
	 * @param id	The ID of shifted vertex
	 * @param delta	Displacement vector
	 */
	void shiftPointLinearGlobal(int id, Vector3d delta);

	/**
	 * Shifts one vertex on the vector displacement and then reconstructs
	 * the planarity of all facets, the functional is linearized in order to
	 * simplify the algorithm, and the displacements are calculated relatively
	 * to the previous state.
	 *
	 * @param id	The ID of shifted vertex
	 * @param delta	Displacement vector
	 */
	void shiftPointLinearLocal(int id, Vector3d delta);

	/**
	 * Tests global linear shifting in a big number of cases.
	 *
	 * @param id		The ID of shifted vertex
	 * @param delta		Displacement vector
	 * @param mode		Mode of shifting
	 * @param num_steps	Reference to the number of step required by algorithm
	 * @param norm_sum	Sum of norms of vector displacements
	 *
	 * TODO: mode should be enum, not integer!
	 */
	void shiftPointLinearTest(int id, Vector3d delta, int mode, int& num_steps,
			double& norm_sum);

	/**
	 * Runs global linear shifting sequentially with small steps.
	 *
	 * This function was added to test whether the self-intersections can be
	 * detected during the minimization.
	 *
	 * @param id		The ID of shifted vertex
	 * @param delta		Displacement vector
	 * @param num		Number of divisions of step that must be done
	 */
	void shiftPointLinearPartial(int id, Vector3d delta, int num);

	/**
	 * Calculates the distance to the nearest neighboring vertex to the given
	 * vertex.
	 *
	 * @param id	The ID of vertex
	 */
	double distToNearestNeighbour(int id);

	/**
	 * Copies coordinates from one polyhedron to another (for backuping)
	 *
	 * @param orig	Reference to another polyhedron
	 */
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
	void printSortedByAreaFacets(void);
	list< struct FacetWithArea > getSortedByAreaFacets(void);

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
	void correctGlobal(shared_ptr<ShadeContourData> contourData,
			GSCorrectorParameters* parameters, list<int>* facetsCorrected);

	// Polyhedron_verification.cpp
	int test_structure();
	int countConsections(bool ifPrint);
	int checkEdges(EdgeDataPtr edgeData);
};

#endif	/* POLYHEDRON_CLASS_H */

