#include "PolyhedraCorrectionLibrary.h"

void test_deform_linear(
		const char* name,
		int type,
		int id,
		Vector3d delta);

int main(
		int argc,
		char** argv) {
	test_deform_linear("poly-small", 0, 0, Vector3d(-1e-1, 1e-1, 1e-1));
	test_deform_linear("poly-small", 0, 1, Vector3d(-1e-1, 1e-1, 1e-1));
	test_deform_linear("poly-small", 0, 2, Vector3d(-1e-1, 1e-1, 1e-1));
	double d = 0.1;
	test_deform_linear("poly-small", 0, 3, Vector3d(-d, d, d));
	test_deform_linear("poly-small", 0, 4, Vector3d(-1e-1, 1e-1, 1e-1));
	test_deform_linear("poly-small", 0, 5, Vector3d(-1e-1, 1e-1, 1e-1));
	test_deform_linear("poly-small", 0, 6, Vector3d(-1e-1, 1e-1, 1e-1));
	test_deform_linear("poly-small", 0, 7, Vector3d(-1e-1, 1e-1, 1e-1));
	test_deform_linear("poly-small", 0, 8, Vector3d(-1e-1, 1e-1, 1e-1));
	test_deform_linear("poly-small", 0, 9, Vector3d(-1e-1, 1e-1, 1e-1));
}

void test_deform_linear(
		const char* name,
		int type,
		int id,
		Vector3d delta) {
	double xmin, xmax, ymin, ymax, zmin, zmax;
	Polyhedron poly;

	char *file_name_in, *file_name_out0, *file_name_out;

	fprintf(stdout, "******************************************************\n");
	fprintf(stdout, "********************   Deform linear    ****************\n");
	fprintf(stdout, "********************     [%d]        *****************\n",
			id);
	fprintf(stdout, "*********** (%lf, %lf, %lf) *****************\n", delta.x,
			delta.y, delta.z);
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
	poly.test_consections(true);

//    printf("simpify_vertex:\n");
//    double eps = 1e-16;
//    int ndel;
//    for (int i = 0; i < 18; ++i) {
//        ndel = poly.simplify_vertex(eps);
//        printf("\t%le\t%d\n", eps, ndel);
//        eps *= 5.;
//        ndel = poly.simplify_vertex(eps);
//        printf("\t%le\t%d\n", eps, ndel);
//        eps *= 2.;
//    }

	poly.deform_linear2(id, delta);
	poly.test_consections(false);

	time_t seconds = time(NULL);
	tm* timeinfo = localtime(&seconds);

	sprintf(file_name_out, "../poly-data-out/%s - %d-%d-%d %d:%d:%d - %d.ply",
			name, timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
			timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, id);

	poly.fprint_ply_scale(1000., file_name_out,
			"made-by-Polyhedron_join_facets_2");

	if (file_name_in != NULL)
		delete[] file_name_in;
	if (file_name_out0 != NULL)
		delete[] file_name_out0;
	if (file_name_out != NULL)
		delete[] file_name_out;

}

