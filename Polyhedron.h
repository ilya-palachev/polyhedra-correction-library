#ifndef POLYHEDRON_H
#define POLYHEDRON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Vector3d.h"

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
////////                                                    ///////
////////    Set of classes for working with POLYHEDRONS     //////
////////                                                    ///////
////////              (MAIN FILE)                         ///////
////////                                                    ///////
////////                                                    ///////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////


// Following macroses allow to print debugging information:
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


// Main class for working with polyhedrons

class Polyhedron {
public:
    int numv; //Number of vertexes
    int numf; //Number of facets

    Vector3d* vertex; //Array of vertexes
    Facet* facet; //Array of facets
    VertexInfo* vertexinfo; //Array of the information about vertexes
    EdgeList* edge_list; //Array of HELP structures which is used when 
    //the polyhedron is consected with plane


public:

    ////////////////////////////////////////////////////////
    // BASIC FUNCTION - MEMBERS:
    // Location: Polyhedron.cpp
    ////////////////////////////////////////////////////////
    
    // Default constructor:
    Polyhedron();

    // Simple constructor:
    Polyhedron(
            int numv_orig,
            int numf_orig,
            Vector3d* vertex_orig,
            Facet* facet_orig);

    // Full constructor:
    Polyhedron(
            int numv_orig,
            int numf_orig,
            Vector3d* vertex_orig,
            Facet* facet_orig,
            VertexInfo* vertexinfo);

    // Destructor:
    ~Polyhedron();

    // Get information about max/min coordinates of vertexes of polyhedron
    void get_boundary(
            double& xmin, double& xmax,
            double& ymin, double& ymax,
            double& zmin, double& zmax);

    // Delete facets which contains not more than 2 vertexes
    // (i. e. empty facets)
    void delete_empty_facets();

    //////////////////////////////////////////////////////////////////////
    // FUNCTION MEMBERS FOR INPUT AND OUTPUT
    // Location: Polyhedron_io.cpp
    //////////////////////////////////////////////////////////////////////
    
    // Print polyhedron to file:
    void my_fprint(const char* filename);
    
    // Print polyhedron to stream:
    void my_fprint(FILE* file);

    // Scan polyhedron from file (5 different formats) : 
    // (see information in Polyhedron_io.cpp)
    void fscan_default_0(const char* filename);
    void fscan_default_1(const char* filename);
    void fscan_default_1_1(const char* filename);
    void fscan_my_format(const char* filename);
    void fscan_ply(const char* filename);
    
    // Print polyhedron to file (5 different formats) : 
    // (see information in Polyhedron_io.cpp)    
    void fprint_default_0(const char* filename);
    void fprint_default_1(const char* filename);
    void fprint_my_format(const char* filename);
    void fprint_ply(const char *filename, const char *comment);
    void fprint_ply_scale(double scale, const char *filename, const char *comment);

    /////////////////////////////////////////////////////////////////////////
    // FUNCTION - MEMBER for preprocessing (organizing the information about
    // neighbours for facets, positions, etc.)
    // Location : Polyhedron_preprocess.cpp
    /////////////////////////////////////////////////////////////////////////
    void preprocess_polyhedron();

    /////////////////////////////////////////////////////////////////////////
    // FUNCTIONS - MEMBERS for creating the simple polyhedra
    // Location : Polyhedron_figures.cpp
    /////////////////////////////////////////////////////////////////////////
    
    // Cube : 
    void poly_cube(double h, double x, double y, double z);
    
    // Pyramid : 
    void poly_pyramid(int n, double h, double r);
    
    // Prism :
    void poly_prism(int n, double h, double r);
    
    // Letter "pi" : 
    void poly_cube_cutted();

    /////////////////////////////////////////////////////////////////////////
    // FUNCTIONS - MEMBERS for INTERSECTING the polyhedra
    // Location : Polyhedron_intersection.cpp
    /////////////////////////////////////////////////////////////////////////
    
    // Intersecting the polyhedron with the plane (any plane) :
    void intersect(Plane iplane);
    
    // This function joins the points of neighbour facets in one contour
    // and intersects the polyhedron with LSM plane for them :
    void intersect_test(int facet_id0, int facet_id1);

    // Secondary function : 
    void set_isUsed(int v0, int v1, bool val);

    /////////////////////////////////////////////////////////////////////////
    // FUNCTION - MEMBERS for joining facets:
    // Location : Polyhedron_join_facets.cpp
    /////////////////////////////////////////////////////////////////////////
    
    /////////// There are two MAIN functions : 
    // Join 2 (two) facets : 
    void join_facets(int fid0, int fid1);
    
    // Join ANY number of facets : 
    void multi_join_facets(int n, int* fid);
    //////////
    
    // Construt the contour (for 2 facets): 
    void join_facets_build_index(int fid0, int fid1, Plane& plane, Facet& join_facet, int& nv);

    // Construt the contour (for N facets): 
    void multi_join_facets_build_index(int n, int* fid, Facet& join_facet, int& nv);
    
    // Helps the "multi_join_facets_build_index" to join 2 facets firstly : 
    bool build_index_pair(int fid0, int fid1, Facet& join_facet);

    // Calculating the middle plane for the constructes contour (for 2 facets):
    void join_facets_calculate_plane(int fid0, int fid1, Facet& join_facet, Plane& plane);
    
    // Calculating the middle plane for the constructes contour (for N facets):
    void multi_join_facets_calculate_plane(int n, int* fid, Facet& join_facet, Plane& plane);

    // Rise all vertexes which lay under the LSM plane : 
    void join_facets_rise(int fid0);
    
    // Find vertex which is to be rised (i. e. with the minimal distance):
    void join_facets_rise_find(int fid0, int& imin);
    
    // Find in what distance point may be rised:
    void join_facets_rise_find_step(int fid0, int i, double& d);
    
    // RISE the point untill the LSM plane: 
    void join_facets_rise_point(int fid0, int imin);
    
    // Calulating the middle plane for the array of points with LSM : 
    void list_squares_method(int nv, int* vertex_list, Plane* plane);
    
    // Calulating the middle plane for the array of points with LSM 
    // (vertexes act with their weights - the lengths of the incident edges): 
    void list_squares_method_weight(int nv, int* vertex_list, Plane* plane);


    
    // Delete vertex "v" from polyhedron : 
    void delete_vertex_polyhedron(int v);
    
    // Add new vertex to tha array of the vectors in polyhedron : 
    int add_vertex(Vector3d& vec);
    
    // Print the vector : 
    void print_vertex(int i);

    // Delete all vertexes which do not lay in any facet : 
    void clear_unused();
    
    // Replace vertex "from" in polyhedron to the vertex "to" : 
    void find_and_replace_vertex(int from, int to);
    
    // Replace facet "from" in polyhedron to the facet "to" : 
    void find_and_replace_facet(int from, int to);


    // Test the structure of the polyhedron, i. e. test if there is any inner
    // or outer consection in polyhedron : 
    int test_structure();

    
    //////////////////////////////////////////////////////
    ///  FUNCTION-MEMBER for intersection the polyhedron by the plane
    ///  for the case of joing facets : 
    ///  Location : Polyhedron_intersection_j.cpp
    /////////////////////////////////////////////////////////
    void intersect_j(Plane iplane, int jfid);

    //////////////////////////////////////////////////////
    // FUNCTION - MEMBER for the calculating of the signum of the point 
    //   to the plane (up, down or lays in it):
    // Location : Polyhedron.h
    //////////////////////////////////////////////////////
    int signum(Vector3d point, Plane plane);

    //////////////////////////////////////////////////////
    //  FUNCTION-MEMBERS for deforming  (NON-LINEAR)one point of the polyhedron : 
    //  (Warning: not tested)
    //  Location : Polyhedron_deform.cpp
    //////////////////////////////////////////////////////
    
    // Deform the vertex : 
    void deform(int id, Vector3d delta);
    
    // Calculate the help functional
    void f(int n, double* x, double* fx, int id, int* sum);
    
    // Calculate the derivate of the functional : 
    void derf(int n, double* x, double* A, int id, int* sum);
    
    // Calculate the seconf derivate : 
    void derf2(int n, double* x, double* A, int id, int* sum,
            double* tmp0, double* tmp1, double* tmp2, double* tmp3);

    // Deform the vertex (second): 
    void deform_w(int id, Vector3d delta);
    
    // Calculate the help functional : 
    void f_w(int n, double* x, double* fx, int id, bool* khi, int K);
    
    // Calculate the derivate of the functional (second): 
    void derf_w(int n, double* x, double* A, int id, bool* khi, int K,
            double* tmp0, double* tmp1, double* tmp2, double* tmp3);

    
    //////////////////////////////////////////////////////
    //  FUNCTION-MEMBERS for deforming  (LINEAR) one point of the polyhedron : 
    //  
    //  Location : Polyhedron_deform_linear.cpp
    //////////////////////////////////////////////////////

    // Deform by minimizing global variations : 
    void deform_linear(int id, Vector3d delta);
    
    // Deform by minimizing local variations : 
    void deform_linear2(int id, Vector3d delta);
    
    // Deform by the dividing the deformaation to steps : 
    void deform_linear_partial(int id, Vector3d delta, int num);
    
    // Calculate better facets :
    void deform_linear_facets(double* x, double* y, double* z);
    
    // Calculate better vertices (global) : 
    void deform_linear_vertices(int id, double K, double* A, double* B, double* x, double* y, double* z);
    
    // Calculate better vertices (local) : 
    void deform_linear_vertices(int id, double K, double* A, double* B);
    
    // Calculate the error of the laying of the vertices in the planes : 
    double deform_linear_calculate_error();
    
    // Calculate the summing information of the vertices : 
    double deform_linear_calculate_deform(double* x, double* y, double* z);
    
    // Calculates the minimal distance from one point to all of its neighbour points : 
    double min_dist(int id);
    
    // Copy the vertex coordintes from one polyhedon to another : 
    void import_coordinates(Polyhedron& orig);

    // This function just can call some versions (by number "mode") of the function deform_linear : 
    void deform_linear_test(int id, Vector3d delta, int mode, int& num_steps, double& norm_sum);

    //////////////////////////////////////////////////////
    //  FUNCTION-MEMBERS for deforming  (LINEAR) one point of the polyhedron : 
    //  These functions helps the previos algorithm (in Polyhedron_deform_linear.cpp)
    //  to change location of points without creating inner consections.
    //  Location : Polyhedron_deform_linear_control.cpp
    //////////////////////////////////////////////////////
    
    // Deform vertex[I] to the new position v_new. There deformation is controled
    // whether there are any consections in facets after it. If there are, v_new
    // is changed by the decreasing the length of the deformation, i. e. (v_new - vertex[I])
    // so that there will not be any consections after it.
    void deform_linear_vertex_control(int I, Vector3d& v_new, Vector3d* vertex_old);
    
    // Calculate better vertices (global) with control of not-creating of consections : 
    void deform_linear_vertices_control(int id, double K, double* A, double* B, double* x, double* y, double* z, Vector3d* vertex_old);
    
    // Deform by minimizing global variations : 
    void deform_linear_control(int id, Vector3d delta);
 
    // Calculate better vertices (local) with control of not-creating of consections : 
    void deform_linear_vertices_control(int id, double K, double* A, double* B, Vector3d* vertex_old);
 
    // Deform by minimizing local variations : 
    void deform_linear2_control(int id, Vector3d delta);
    
    //////////////////////////////////////////////////////
    //  FUNCTION-MEMBERS for deforming  (LINEAR) one point of the polyhedron : 
    //  These functions helps the previos algorithm (in Polyhedron_deform_linear.cpp)
    //  to change location of points without creating inner consections.
    //  It is reached by using of method of polyhedra region of allowed deformations
    //  of point.
    //  Location : Polyhedron_deform_linear_vertmin.cpp
    //////////////////////////////////////////////////////

    // Calculate planes which determine the polyhedra region of allowed 
    // deformations of point :
    void calculate_allowed_region(int id, int& n, Plane* plane);
    
    // Calculate the functionsl which is minimized in this method :
    double calculate_functional(int id, double K, Vector3d vnew);
    
    // Change the position of one point in purpose to minimize the functional.
    // Method of the polyhedra region of allowed deformations is used :
    void deform_linear_vertex_vertmin(int id, double K, double* A, double* B, Plane* plane);

    // Calculate the case when all lambdas = 0 :
    bool calculate_point_case0(int id, double K, double* matrix, double* row, Vector3d& vnew,
    double Maa, double Mab, double Mac, 
    double Mad, double Mbb, double Mbc, 
    double Mbd, double Mcc, double Mcd);
    
    // Calculate the case when one and only one lambda is not null :
    bool calculate_point_case1(int id, double K, double* matrix, double* row, Vector3d& vnew, 
    Plane plane, double& lambda,
    double Maa, double Mab, double Mac, 
    double Mad, double Mbb, double Mbc, 
    double Mbd, double Mcc, double Mcd);
    
    // Calculate the case when two and only two lambdas are not null :
    bool calculate_point_case2(int id, double K, double* matrix, double* row, Vector3d& vnew, 
    Plane plane0, Plane plane1, double& lambda0, double& lambda1,
    double Maa, double Mab, double Mac, 
    double Mad, double Mbb, double Mbc, 
    double Mbd, double Mcc, double Mcd);
    
    // Calculate the case when three and only three lambdas are not null :
    bool calculate_point_case3(int id, double K, double* matrix, double* row, Vector3d& vnew, 
    Plane plane0, Plane plane1, Plane plane2, 
    double& lambda0, double& lambda1, double& lambda2,
    double Maa, double Mab, double Mac, 
    double Mad, double Mbb, double Mbc, 
    double Mbd, double Mcc, double Mcd);
    

    // Calculate better vertices (global) with control of not-creating of consections : 
    // Method of the polyhedra region of allowed deformations is used :
    void deform_linear_vertices_vermin(int id, double K, double* A, double* B, double* x, double* y, double* z);
    
    // Deform by minimizing global variations : 
    // Method of the polyhedra region of allowed deformations is used :
    void deform_linear_vertmin(int id, Vector3d delta);
 
    // Calculate better vertices (local) with control of not-creating of consections : 
    // Method of the polyhedra region of allowed deformations is used :
    void deform_linear_vertices_vertmin(int id, double K, double* A, double* B);
 
    // Deform by minimizing local variations : 
    // Method of the polyhedra region of allowed deformations is used :
    void deform_linear2_vertmin(int id, Vector3d delta);
    
    //////////////////////////////////////////////////////////////////////
    //    FUNCTION-MEMBERS which test if there is any inner or outer consections in 
    //    the polyhedron : 
    //    Location : Polyhedron_test_consections.cpp
    //////////////////////////////////////////////////////////////////////
    
    // Test all consections : 
    int test_consections(bool ifPrint);
    
    // Test inner consections (i. e. edges consect in one facet) :
    int test_inner_consections(bool ifPrint);
    
    // Test inner consections in one facet : 
    int test_inner_consections_facet(bool ifPrint, int fid, double* A, double* b, Vector3d* vertex_old);
    
    // Test if the 2 edges consect : 
    int test_inner_consections_pair(bool ifPrint,
            int fid, int id0, int id1, int id2, int id3, double* A, double* b);
    
    // Test if the 2 edges consect (another method, as in deform_linear_vertex_control): 
    int test_inner_consections_pair2(bool ifPrint,
            int fid, int id0, int id1, int id2, int id3, double* A, double* b);
    
    // Test outer consections (i.e. edge consects another facet) : 
    int test_outer_consections(bool ifPrint);
    
    // Test outer consections in one facet : 
    int test_outer_consections_facet(bool ifPrint, int fid);
    
    // Test if one edge consects any another facet : 
    int test_outer_consections_edge(bool ifPrint, int id0, int id1);
    
    // Test if one edge consects one facet : 
    int test_outer_consections_pair(bool ifPrint, int id0, int id1, int fid);

    
    
    /////////////////////////////////////////////////////////////////////////
    //   FUNCTION-MEMBERS which join the point if theit location is near each other
    //   Location : Polyhedron_join_points.cpp
    /////////////////////////////////////////////////////////////////////////
    
    // Join nearest points : 
    int join_points(int id);
    
    // Join points (same): 
    int join_points_inner(int id);
    
    // Join points in one facet
    int join_points_inner_facet(int id, int fid, double* A, double* b, Vector3d* vertex_old);
    
    // Join two points : 
    int join_points_inner_pair(
            int id, int fid, int id0, int id1, int id2, int id3, double* A, double* b);

    
    /////////////////////////////////////////////////////////////////////////\
    //  FUNCTION-MEMBERS which calculate the number of points if they can be joined : 
    //  Location : Polyhedron_simplify.cpp
    /////////////////////////////////////////////////////////////////////////
    
    // Calculate the number of points if they can be joined
    int simplify_vertex(double eps);
    
    // Calculate the distance between to points : 
    double dist_vertex(int i, int j);

};


// Informarion about vertex : 
class VertexInfo {
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

    VertexInfo& operator =(const VertexInfo& orig);
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
    EdgeList& operator =(const EdgeList& orig);
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
    void get_next_edge(Plane iplane, int& v0, int& v1, int& next_f, int& next_d, bool ifSpecialFacet);
    void get_next_edge(Plane iplane, int& v0, int& v1, int& next_f, int& next_d);
    void get_next_edge(Plane iplane, int& v0, int& v1, int& i0, int& i1, int& next_f, int& next_d, bool ifSpecialFacet);
    void get_next_edge(Plane iplane, int& v0, int& v1, int& i0, int& i1, int& next_f, int& next_d);

    void find_edge_for_vertex(int v, int& v0, int& v1, int& next_f);

    void send(EdgeSet* edge_set);
    void send_edges(EdgeSet* edge_set);

    void set_poly(Polyhedron* poly_orig);

    //EdgeList_io,cpp
    void my_fprint(FILE* file);
};

class Facet {
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
    Facet& operator =(const Facet& facet1);
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
    
    /////////////////////////////////////////////
    // FUNCTION-MEMBERS for deformation by method of polyhedra regions:
    // Location : Facet_deform_linear_vertmin.cpp
    /////////////////////////////////////////////
    
    // Determine if there is convexness (1) or non-convexness (-1) or
    // collinearity (0) in point is in the contour of facet:
    int convexness_point(int i);

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
    FutureFacet& operator =(const FutureFacet& orig);
    FutureFacet& operator +=(const FutureFacet& v);
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
    EdgeSet& operator =(const EdgeSet& orig);
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
