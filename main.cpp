#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <string.h>

#include "Polyhedron.h"

//#define NOT_DEBUG

using namespace std;
void test_edge_list();
void test_figures();


int main(int argc, char** argv) {
	argc = argc;
	argv = argv;
	test_edge_list();
	return 0;
}

void test_edge_list() {
	int i;
	char *curr_time, *file_name;
	Plane iplane;

	file_name = new char[255];

	time_t seconds = time(NULL);
	tm* timeinfo = localtime(&seconds);
	curr_time = asctime(timeinfo);

	Polyhedron poly;

	sprintf(file_name, "poly-data-out/cube_cutted - %d-%d-%d %d:%d:%d.ply",
			timeinfo->tm_year + 1900,
			timeinfo->tm_mon + 1,
			timeinfo->tm_mday,
			timeinfo->tm_hour,
			timeinfo->tm_min,
			timeinfo->tm_sec);
	poly.poly_cube_cutted();
#ifdef NOT_DEBUG
//	poly.fscan_default_0("poly-data-in/cube_cutted.dat");
	poly.fprint_ply_scale(1e3, "poly-data-out/cube_cutted.ply", "poly");
#endif
	poly.preprocess_polyhedron();
#ifdef DEBUG
	printf("preprocessing ended\n");
#endif
//	poly.my_fprint(stdout);
	iplane = Plane(Vector3d(0., 0., 1), -1.);
	poly.intersection(iplane);
	iplane = Plane(Vector3d(0., 0., 1), -2.);
	poly.intersection(iplane);
	iplane = Plane(Vector3d(0., 0., 1), -3.);
	poly.intersection(iplane);
}


void test_figures() {
	char *curr_time, *file_name;

		file_name = new char[255];

		time_t seconds = time(NULL);
		tm* timeinfo = localtime(&seconds);
		curr_time = asctime(timeinfo);

		Polyhedron poly;

		/////// Pyramid /////////
		sprintf(file_name, "poly-data-out/pyramid - %d-%d-%d %d:%d:%d.ply",
				timeinfo->tm_year + 1900,
				timeinfo->tm_mon + 1,
				timeinfo->tm_mday,
				timeinfo->tm_hour,
				timeinfo->tm_min,
				timeinfo->tm_sec);

		poly.poly_pyramid(10, 3., 1.);

	#ifdef NOT_DEBUG
		poly.fprint_ply_scale(1e3, file_name,	"created-by-polyhedron");
	#endif

		sprintf(file_name, "poly-data-out/pyramid - %d-%d-%d %d:%d:%d.txt",
				timeinfo->tm_year + 1900,
				timeinfo->tm_mon + 1,
				timeinfo->tm_mday,
				timeinfo->tm_hour,
				timeinfo->tm_min,
				timeinfo->tm_sec);
		poly.preprocess_polyhedron();
		poly.fprint_my_format(file_name);
	#ifdef NOT_DEBUG
		poly.my_fprint("poly-data-out/my_fprint.txt");
	#endif
		poly.my_fprint(stdout);

		/////// Prism /////////
		sprintf(file_name, "poly-data-out/prism - %d-%d-%d %d:%d:%d.ply",
				timeinfo->tm_year + 1900,
				timeinfo->tm_mon + 1,
				timeinfo->tm_mday,
				timeinfo->tm_hour,
				timeinfo->tm_min,
				timeinfo->tm_sec);

		poly.poly_prism(10, 3., 1.);
	#ifdef NOT_DEBUG
		poly.fprint_ply_scale(1e3, file_name,	"created-by-polyhedron");
	#endif

		sprintf(file_name, "poly-data-out/prism - %d-%d-%d %d:%d:%d.txt",
				timeinfo->tm_year + 1900,
				timeinfo->tm_mon + 1,
				timeinfo->tm_mday,
				timeinfo->tm_hour,
				timeinfo->tm_min,
				timeinfo->tm_sec);
		poly.preprocess_polyhedron();
	#ifdef NOT_DEBUG
		poly.fprint_my_format(file_name);
	#endif
		printf("End!\n");
}



