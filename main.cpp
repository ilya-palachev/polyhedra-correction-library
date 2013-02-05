#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <string.h>

#include "Polyhedron.h"
#include "Gauss_test.h"
//#define NOT_DEBUG

using namespace std;
void test_cube_cutted();
void test_figures();
void test(const char* name, int type, double a, double b, double c, double d);
void test2(const char* name, int type, int fid0, int fid1);
void test3(const char* name, int type, int fid0, int fid1);
void test_deform(const char* name, int type, int id, Vector3d delta);

int main(int argc, char** argv) {
	argc = argc;
	argv = argv;
        
//        Gauss_test(3);
        test_deform("poly-cube", 0, 7, Vector3d(-1e-10, 1e-10, 1e-10));
        
        
        
//	test_cube_cutted();
//	test("poly-small", 0, 0., 0., 1., 3.);
//	test("poly-med", 0, 0., 0., 1., 1.);

//	test("poly-big", 0, 1., 0., 0., 3.);
//	test("poly-big", 0, 1., 0., 0., 3.1);
//	test("poly-big", 0, 1., 0., 0., 3.3);
//	test("poly-big", 0, 1., 0., 0., 3.4);
//	test("poly-big", 0, 1., 0., 0., 3.5);

//	Vector3d origin, normal;
//	double a, b, c, d;
//
//	origin = Vector3d(-3308.14593415889, -85.2348088534985, -2744.2585328124) * 0.001;
//	normal = Vector3d(0.9907942064402, 0.134257976187036, -0.017367680175984);
//	a = normal.x;
//	b = normal.y;
//	c = normal.z;
//	d = - origin * normal;
//	test("poly-big", 0, a, b, c, d);


//37, 249
//0, 250, 14
//2, 253
//3, 13, 252, 248
//1, 12, 251

//	test2("polyhedron-2010-11-25", 2, 37, 249);
//	test2("polyhedron-2010-11-25", 2, 0, 250);
//	test2("polyhedron-2010-11-25", 2, 0, 14);
//	test2("polyhedron-2010-11-25", 2, 2, 253);
//	test2("polyhedron-2010-11-25", 2, 3, 13);
//	test2("polyhedron-2010-11-25", 2, 3, 252);
//	test2("polyhedron-2010-11-25", 2, 13, 248);
//	test2("polyhedron-2010-11-25", 2, 13, 252);
//	test2("polyhedron-2010-11-25", 2, 1, 12);
//	test2("polyhedron-2010-11-25", 2, 1, 251);
//	test2("polyhedron-2010-11-25", 2, 12, 251);

//67 112
//68 106
//69 188
//71 115
//72 208
//73 173
//74 103
//79 196 184
//84 169

//        test3("poly-med", 0, 3, 189);
//        test3("poly-test-2", 0, 5, 6);
//	test3("polyhedron-2010-12-19", 2, 67, 112);
//	test3("polyhedron-2010-12-19", 2, 68, 106);
//	test3("polyhedron-2010-12-19", 2, 69, 188);
        
        
//	test3("polyhedron-2010-12-19", 2, 71, 115);
//	test2("polyhedron-2010-12-19", 2, 72, 208);
//	test2("polyhedron-2010-12-19", 2, 73, 173);
//	test2("polyhedron-2010-12-19", 2, 74, 103);
//	test2("polyhedron-2010-12-19", 2, 79, 196);
//	test2("polyhedron-2010-12-19", 2, 79, 184);
//	test2("polyhedron-2010-12-19", 2, 196, 184);
//	test2("polyhedron-2010-12-19", 2, 84, 169);

	return 0;
}

void test_deform(const char* name, int type, int id, Vector3d delta) {
	double xmin, xmax, ymin, ymax, zmin, zmax;
	Polyhedron poly;

	char *curr_time, *file_name_in, *file_name_out0, *file_name_out;

	fprintf(stdout, "******************************************************\n");
	fprintf(stdout, "********************   Deformation    ****************\n");
	fprintf(stdout, "********************     [%d]        *****************\n", id);
	fprintf(stdout, "*********** (%lf, %lf, %lf) *****************\n", 
                delta.x, delta.y, delta.z);
	fprintf(stdout, "******************************************************\n");

	file_name_in = new char[255];
	file_name_out0 = new char[255];
	file_name_out = new char[255];

	sprintf(file_name_in, "../poly-data-in/%s.dat", name);
	sprintf(file_name_out0, "../poly-data-out/%s.ply", name);

	switch (type) {
		case 0 :
			poly.fscan_default_0(file_name_in);
			break;
		case 1 :
			poly.fscan_default_1(file_name_in);
			break;
		case 2 :
			poly.fscan_default_1_1(file_name_in);
			break;
		default :
			if (file_name_in != NULL)
				delete[] file_name_in;
			if (file_name_out != NULL)
				delete[] file_name_out;
			return;
			break;
	}

	poly.fprint_ply_scale(1000.,  file_name_out0, "made-by-Polyhedron_join_facets_2");

	poly.get_boundary(xmin, xmax, ymin, ymax, zmin, zmax);
	fprintf(stdout, "boundary : \n");
	fprintf(stdout, "x [%lf,  %lf]\n", xmin, xmax);
	fprintf(stdout, "y [%lf,  %lf]\n", ymin, ymax);
	fprintf(stdout, "z [%lf,  %lf]\n", zmin, zmax);

	poly.preprocess_polyhedron();
        poly.deform(id, delta);

	time_t seconds = time(NULL);
	tm* timeinfo = localtime(&seconds);
	curr_time = asctime(timeinfo);

	sprintf(file_name_out, "../poly-data-out/%s - %d-%d-%d %d:%d:%d - %d.ply",
			name,
			timeinfo->tm_year + 1900,
			timeinfo->tm_mon + 1,
			timeinfo->tm_mday,
			timeinfo->tm_hour,
			timeinfo->tm_min,
			timeinfo->tm_sec,
			id);

	poly.fprint_ply_scale(1000., file_name_out,
			"made-by-Polyhedron_join_facets_2");

	if (file_name_in != NULL)
		delete[] file_name_in;
	if (file_name_out0 != NULL)
		delete[] file_name_out0;
	if (file_name_out != NULL)
		delete[] file_name_out;
    
}

//
//void test3(const char* name, int type, int fid0, int fid1) {
//	double xmin, xmax, ymin, ymax, zmin, zmax;
//	Plane iplane;
//	Polyhedron poly;
//
//	char *curr_time, *file_name_in, *file_name_out0, *file_name_out;
//
//	fprintf(stdout, "******************************************************\n");
//	fprintf(stdout, "********************   %d       %d    ****************\n",
//			fid0, fid1);
//	fprintf(stdout, "******************************************************\n");
//
//	file_name_in = new char[255];
//	file_name_out0 = new char[255];
//	file_name_out = new char[255];
//
//	sprintf(file_name_in, "../poly-data-in/%s.dat", name);
//	sprintf(file_name_out0, "../poly-data-out/%s.ply", name);
//
//	switch (type) {
//		case 0 :
//			poly.fscan_default_0(file_name_in);
//			break;
//		case 1 :
//			poly.fscan_default_1(file_name_in);
//			break;
//		case 2 :
//			poly.fscan_default_1_1(file_name_in);
//			break;
//		default :
//			if (file_name_in != NULL)
//				delete[] file_name_in;
//			if (file_name_out != NULL)
//				delete[] file_name_out;
//			return;
//			break;
//	}
//
//	poly.fprint_ply_scale(1000.,  file_name_out0, "made-by-Polyhedron_join_facets_2");
//
//	poly.get_boundary(xmin, xmax, ymin, ymax, zmin, zmax);
//	fprintf(stdout, "boundary : \n");
//	fprintf(stdout, "x [%lf,  %lf]\n", xmin, xmax);
//	fprintf(stdout, "y [%lf,  %lf]\n", ymin, ymax);
//	fprintf(stdout, "z [%lf,  %lf]\n", zmin, zmax);
//
//	poly.preprocess_polyhedron();
//	poly.join_create_first_facet(fid0, fid1);
//
//	time_t seconds = time(NULL);
//	tm* timeinfo = localtime(&seconds);
//	curr_time = asctime(timeinfo);
//
//	sprintf(file_name_out, "../poly-data-out/%s - %d-%d-%d %d:%d:%d - %d - %d.ply",
//			name,
//			timeinfo->tm_year + 1900,
//			timeinfo->tm_mon + 1,
//			timeinfo->tm_mday,
//			timeinfo->tm_hour,
//			timeinfo->tm_min,
//			timeinfo->tm_sec,
//			fid0, fid1);
//
//	poly.fprint_ply_scale(1000., file_name_out,
//			"made-by-Polyhedron_join_facets_2");
//
//	if (file_name_in != NULL)
//		delete[] file_name_in;
//	if (file_name_out0 != NULL)
//		delete[] file_name_out0;
//	if (file_name_out != NULL)
//		delete[] file_name_out;
//}
//
//
//
//void test2(const char* name, int type, int fid0, int fid1) {
//	double xmin, xmax, ymin, ymax, zmin, zmax;
//	Plane iplane;
//	Polyhedron poly;
//
//	char *curr_time, *file_name_in, *file_name_out0, *file_name_out;
//
//	fprintf(stdout, "******************************************************\n");
//	fprintf(stdout, "********************   %d       %d    ****************\n",
//			fid0, fid1);
//	fprintf(stdout, "******************************************************\n");
//
//	file_name_in = new char[255];
//	file_name_out0 = new char[255];
//	file_name_out = new char[255];
//
//	sprintf(file_name_in, "../poly-data-in/%s.dat", name);
//	sprintf(file_name_out0, "../poly-data-out/%s.ply", name);
//
//	switch (type) {
//		case 0 :
//			poly.fscan_default_0(file_name_in);
//			break;
//		case 1 :
//			poly.fscan_default_1(file_name_in);
//			break;
//		case 2 :
//			poly.fscan_default_1_1(file_name_in);
//			break;
//		default :
//			if (file_name_in != NULL)
//				delete[] file_name_in;
//			if (file_name_out != NULL)
//				delete[] file_name_out;
//			return;
//			break;
//	}
//
//	poly.fprint_ply_scale(1000.,  file_name_out0, "made-by-Polyhedron_join_facets_2");
//
//	poly.get_boundary(xmin, xmax, ymin, ymax, zmin, zmax);
//	fprintf(stdout, "boundary : \n");
//	fprintf(stdout, "x [%lf,  %lf]\n", xmin, xmax);
//	fprintf(stdout, "y [%lf,  %lf]\n", ymin, ymax);
//	fprintf(stdout, "z [%lf,  %lf]\n", zmin, zmax);
//
//	poly.preprocess_polyhedron();
////	poly.intersect(Plane(Vector3d(a, b, c), d));
//	poly.intersect_test(fid0, fid1);
//
//	time_t seconds = time(NULL);
//	tm* timeinfo = localtime(&seconds);
//	curr_time = asctime(timeinfo);
//
//	sprintf(file_name_out, "../poly-data-out/%s - %d-%d-%d %d:%d:%d - %d - %d.ply",
//			name,
//			timeinfo->tm_year + 1900,
//			timeinfo->tm_mon + 1,
//			timeinfo->tm_mday,
//			timeinfo->tm_hour,
//			timeinfo->tm_min,
//			timeinfo->tm_sec,
//			fid0, fid1);
//
//	poly.fprint_ply_scale(1000., file_name_out,
//			"made-by-Polyhedron_join_facets_2");
//
//	if (file_name_in != NULL)
//		delete[] file_name_in;
//	if (file_name_out0 != NULL)
//		delete[] file_name_out0;
//	if (file_name_out != NULL)
//		delete[] file_name_out;
//}
//void test(const char* name, int type, double a, double b, double c, double d) {
//	double xmin, xmax, ymin, ymax, zmin, zmax;
//	Plane iplane;
//	Polyhedron poly;
//
//	char *curr_time, *file_name_in, *file_name_out0, *file_name_out;
//	file_name_in = new char[255];
//	file_name_out0 = new char[255];
//	file_name_out = new char[255];
//
//	sprintf(file_name_in, "../poly-data-in/%s.dat", name);
//	sprintf(file_name_out0, "../poly-data-out/%s.ply", name);
//
//	switch (type) {
//		case 0 :
//			poly.fscan_default_0(file_name_in);
//			break;
//		case 1 :
//			poly.fscan_default_1(file_name_in);
//			break;
//		case 2 :
//			poly.fscan_default_1_1(file_name_in);
//			break;
//		default :
//			if (file_name_in != NULL)
//				delete[] file_name_in;
//			if (file_name_out != NULL)
//				delete[] file_name_out;
//			return;
//			break;
//	}
//
//	poly.fprint_ply_scale(1000.,  file_name_out0, "made-by-Polyhedron_join_facets_2");
//
//	poly.get_boundary(xmin, xmax, ymin, ymax, zmin, zmax);
//	fprintf(stdout, "boundary : \n");
//	fprintf(stdout, "x [%lf,  %lf]\n", xmin, xmax);
//	fprintf(stdout, "y [%lf,  %lf]\n", ymin, ymax);
//	fprintf(stdout, "z [%lf,  %lf]\n", zmin, zmax);
//
//	poly.preprocess_polyhedron();
//	poly.intersect(Plane(Vector3d(a, b, c), d));
//
//	time_t seconds = time(NULL);
//	tm* timeinfo = localtime(&seconds);
//	curr_time = asctime(timeinfo);
//
//	sprintf(file_name_out, "../poly-data-out/%s - %d-%d-%d %d:%d:%d.ply",
//			name,
//			timeinfo->tm_year + 1900,
//			timeinfo->tm_mon + 1,
//			timeinfo->tm_mday,
//			timeinfo->tm_hour,
//			timeinfo->tm_min,
//			timeinfo->tm_sec);
//
//	poly.fprint_ply_scale(1000., file_name_out,
//			"made-by-Polyhedron_join_facets_2");
//
//	if (file_name_in != NULL)
//		delete[] file_name_in;
//	if (file_name_out0 != NULL)
//		delete[] file_name_out0;
//	if (file_name_out != NULL)
//		delete[] file_name_out;
//}
//
//void test_cube_cutted() {
//	Plane iplane;
//
//	Polyhedron poly;
//
//	poly.poly_cube_cutted();
//	poly.fprint_ply_scale(1e3, "../poly-data-out/cube_cutted.ply", "poly");
//	poly.preprocess_polyhedron();
//	iplane = Plane(Vector3d(0., 0., 1), -1.);
//	poly.intersect(iplane);
//	poly.fprint_ply_scale(1e3, "../poly-data-out/cube_cutted_0x0y1z_1.ply", "poly");
//
//	poly.poly_cube_cutted();
//	poly.preprocess_polyhedron();
//	iplane = Plane(Vector3d(0., 0., 1), -2.);
//	poly.intersect(iplane);
//	poly.fprint_ply_scale(1e3, "../poly-data-out/cube_cutted_0x0y1z_2.ply", "poly");
//
//	poly.poly_cube_cutted();
//	poly.preprocess_polyhedron();
//	iplane = Plane(Vector3d(0., 0., 1), -3.);
//	poly.intersect(iplane);
//	poly.fprint_ply_scale(1e3, "../poly-data-out/cube_cutted_0x0y1z_3.ply", "poly");
//}
//
//
//void test_figures() {
//	char *curr_time, *file_name;
//
//		file_name = new char[255];
//
//		time_t seconds = time(NULL);
//		tm* timeinfo = localtime(&seconds);
//		curr_time = asctime(timeinfo);
//
//		Polyhedron poly;
//
//		/////// Pyramid /////////
//		sprintf(file_name, "../poly-data-out/pyramid - %d-%d-%d %d:%d:%d.ply",
//				timeinfo->tm_year + 1900,
//				timeinfo->tm_mon + 1,
//				timeinfo->tm_mday,
//				timeinfo->tm_hour,
//				timeinfo->tm_min,
//				timeinfo->tm_sec);
//
//		poly.poly_pyramid(10, 3., 1.);
//
//	#ifdef NOT_DEBUG
//		poly.fprint_ply_scale(1e3, file_name,	"created-by-polyhedron");
//	#endif
//
//		sprintf(file_name, "../poly-data-out/pyramid - %d-%d-%d %d:%d:%d.txt",
//				timeinfo->tm_year + 1900,
//				timeinfo->tm_mon + 1,
//				timeinfo->tm_mday,
//				timeinfo->tm_hour,
//				timeinfo->tm_min,
//				timeinfo->tm_sec);
//		poly.preprocess_polyhedron();
//		poly.fprint_my_format(file_name);
//	#ifdef NOT_DEBUG
//		poly.my_fprint("poly-data-out/my_fprint.txt");
//	#endif
//		poly.my_fprint(stdout);
//
//		/////// Prism /////////
//		sprintf(file_name, "../poly-data-out/prism - %d-%d-%d %d:%d:%d.ply",
//				timeinfo->tm_year + 1900,
//				timeinfo->tm_mon + 1,
//				timeinfo->tm_mday,
//				timeinfo->tm_hour,
//				timeinfo->tm_min,
//				timeinfo->tm_sec);
//
//		poly.poly_prism(10, 3., 1.);
//	#ifdef NOT_DEBUG
//		poly.fprint_ply_scale(1e3, file_name,	"created-by-polyhedron");
//	#endif
//
//		sprintf(file_name, "../poly-data-out/prism - %d-%d-%d %d:%d:%d.txt",
//				timeinfo->tm_year + 1900,
//				timeinfo->tm_mon + 1,
//				timeinfo->tm_mday,
//				timeinfo->tm_hour,
//				timeinfo->tm_min,
//				timeinfo->tm_sec);
//		poly.preprocess_polyhedron();
//	#ifdef NOT_DEBUG
//		poly.fprint_my_format(file_name);
//	#endif
//		printf("End!\n");
//}
//
//
