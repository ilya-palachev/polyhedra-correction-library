#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <string.h>

#include "Polyhedron.h"

using namespace std;

int main(int argc, char *argv[])
{
	argc = argc;
	argv = argv;
	char *curr_time, *file_name;

	file_name = new char[255];

	time_t seconds = time(NULL);
	tm* timeinfo = localtime(&seconds);
	curr_time = asctime(timeinfo);

//	sprintf(file_name, "poly-data-out/cube - %s.ply", curr_time);

//	system("ls 'Рабочий стол/Прогр 8 сем/Polyhedron_join_facets/'");
//	system("cd 'home/iliya/Рабочий стол/Прогр 8 сем/Polyhedron_join_facets/"\
//		"Polyhedron_join_facets_1/Polyhedron_join_facets_1/'");
	Polyhedron poly;

	sprintf(file_name, "poly-data-out/pyramid - %d-%d-%d %d:%d:%d.ply",
			timeinfo->tm_year + 1900,
			timeinfo->tm_mon + 1,
			timeinfo->tm_mday,
			timeinfo->tm_hour,
			timeinfo->tm_min,
			timeinfo->tm_sec);

	poly.poly_pyramid(10, 3., 1.);
	poly.fprint_ply_scale(1e3, file_name,	"created-by-polyhedron");

	sprintf(file_name, "poly-data-out/prism - %d-%d-%d %d:%d:%d.ply",
			timeinfo->tm_year + 1900,
			timeinfo->tm_mon + 1,
			timeinfo->tm_mday,
			timeinfo->tm_hour,
			timeinfo->tm_min,
			timeinfo->tm_sec);

	poly.poly_prism(10, 3., 1.);
	poly.fprint_ply_scale(1e3, file_name,	"created-by-polyhedron");

	return 0;
//	poly.fscan_default_1_1("poly-data-in/polyhedron-2010-11-25.dat");
//	poly.preprocess_polyhedron();
//	printf("join begins...\n");
////	poly.join_facets(37, 249);
//	poly.join_facets(0, 250);

////	poly.fprint_ply_1e16("poly-data-out/polyhedron-2010-11-25.ply", "poly-data-in/polyhedron-2010-11-25.dat");
////	poly.fscan_default_0("poly-data-in/poly-med.dat");
////	poly.preprocess_polyhedron();
////	printf("join begins...\n");
////	poly.join_facets(0, 3);

////	int col[3] = {37, 249, 258};
////	int col[3] = {0, 250, 258};
////	poly.fprint_ply_1e16(
////			"poly-data-out/polyhedron-2010-12-09-multiple.ply",
////			"poly-data-in/polyhedron-2010-11-25.dat",
////			3,
////			col);
//	poly.fprint_ply_1e16(
//			"poly-data-out/polyhedron-2010-12-09-multiple.ply",
//			"poly-data-in/polyhedron-2010-11-25.dat");



}
