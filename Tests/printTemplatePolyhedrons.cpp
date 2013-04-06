#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <string.h>

#include "Polyhedron.h"

void test_figures();

int main(int argc, char** argv) {
	test_figures();
}

void test_figures() {
	char *curr_time, *file_name;

	file_name = new char[255];

	time_t seconds = time(NULL);
	tm* timeinfo = localtime(&seconds);
	curr_time = asctime(timeinfo);

	Polyhedron poly;

/////// Pyramid /////////
	sprintf(file_name, "../poly-data-out/pyramid - %d-%d-%d %d:%d:%d.ply",
			timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
			timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

	poly.poly_pyramid(10, 3., 1.);

#ifdef NOT_DEBUG
	poly.fprint_ply_scale(1e3, file_name, "created-by-polyhedron");
#endif

	sprintf(file_name, "../poly-data-out/pyramid - %d-%d-%d %d:%d:%d.txt",
			timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
			timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
	poly.preprocess_polyhedron();
	poly.fprint_my_format(file_name);
#ifdef NOT_DEBUG
	poly.my_fprint("poly-data-out/my_fprint.txt");
#endif
	poly.my_fprint(stdout);

/////// Prism /////////
	sprintf(file_name, "../poly-data-out/prism - %d-%d-%d %d:%d:%d.ply",
			timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
			timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

	poly.poly_prism(10, 3., 1.);
#ifdef NOT_DEBUG
	poly.fprint_ply_scale(1e3, file_name, "created-by-polyhedron");
#endif

	sprintf(file_name, "../poly-data-out/prism - %d-%d-%d %d:%d:%d.txt",
			timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
			timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
	poly.preprocess_polyhedron();
#ifdef NOT_DEBUG
	poly.fprint_my_format(file_name);
#endif
	printf("End!\n");
}
