#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <string.h>

#include "Polyhedron.h"

void test_join_facets(
		const char* name,
		int type,
		int fid0,
		int fid1);

int main(
		int argc,
		char** argv) {

	test_join_facets("polyhedron-2010-11-25", 2, 37, 249);
}

void test_join_facets(
		const char* name,
		int type,
		int fid0,
		int fid1) {
	double xmin, xmax, ymin, ymax, zmin, zmax;
	Plane iplane;
	Polyhedron poly;

	char *file_name_in, *file_name_out0, *file_name_out;

	fprintf(stdout, "******************************************************\n");
	fprintf(stdout, "********************   %d       %d    ****************\n",
			fid0, fid1);
	fprintf(stdout, "******************************************************\n");

	file_name_in = new char[255];
	file_name_out0 = new char[255];
	file_name_out = new char[255];

	sprintf(file_name_in, "../poly-data-in/%s.dat", name);
	sprintf(file_name_out0, "../poly-data-out/%s.ply", name);

	switch (type) {
	case 0:
		poly.fscan_default_0(file_name_in);
		break;
	case 1:
		poly.fscan_default_1(file_name_in);
		break;
	case 2:
		poly.fscan_default_1_1(file_name_in);
		break;
	default:
		if (file_name_in != NULL)
			delete[] file_name_in;
		if (file_name_out != NULL)
			delete[] file_name_out;
		return;
		break;
	}

	poly.fprint_ply_scale(1000., file_name_out0,
			"made-by-Polyhedron_join_facets_2");

	poly.get_boundary(xmin, xmax, ymin, ymax, zmin, zmax);
	fprintf(stdout, "boundary : \n");
	fprintf(stdout, "x [%lf,  %lf]\n", xmin, xmax);
	fprintf(stdout, "y [%lf,  %lf]\n", ymin, ymax);
	fprintf(stdout, "z [%lf,  %lf]\n", zmin, zmax);

	poly.preprocess_polyhedron();
	poly.join_facets(fid0, fid1);
	//	poly.join_create_first_facet(fid0, fid1);

	time_t seconds = time(NULL);
	tm* timeinfo = localtime(&seconds);

	sprintf(file_name_out,
			"../poly-data-out/%s - %d-%d-%d %d:%d:%d - %d - %d.ply", name,
			timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
			timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, fid0, fid1);

	poly.fprint_ply_scale(1000., file_name_out,
			"made-by-Polyhedron_join_facets_2");

	if (file_name_in != NULL)
		delete[] file_name_in;
	if (file_name_out0 != NULL)
		delete[] file_name_out0;
	if (file_name_out != NULL)
		delete[] file_name_out;
}
