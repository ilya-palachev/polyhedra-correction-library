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

#include "PolyhedraCorrectionLibrary.h"
#define EPSILON 5e-3

using namespace std;

void test_J11(const char* name, int type);
void test_Jtwo(const char* name1, const char* name2, int type);

void test_cluster(const char* name, int type);
void test_viev(const char* name, int type);
double norm_J(double Jxx1, double Jyy1, double Jzz1, double Jxy1, double Jyz1,
		double Jxz1);

void test_BuildTreeNorm(const char* name, int type);
void test_BuildClaster();

int main(int argc, char** argv)
{
	DEBUG_START;

//    test_J11("poly-small", 0);
	//  test_BuildTreeNorm("poly-small", 0);
	//
//    test_BuildTreeNorm("2009_08_06_round_0_76ct_Lexus_Radius_no_1_no_5_missing_RBC_2", 1);

//    test_BuildTreeNorm("2009_08_06_round_0_76ct_Lexus_Radius_no_1_no_5_missing_RBC_2", 1);
//    test_BuildTreeNorm("2009_08_06_round_0_93ct_Lexus_Radius_no_1_no_5_missing_RBC_1", 1);
//    test_BuildTreeNorm("2009_08_06_round_1_01ct_Lexus_Radius_no_1_no_5_missing_RBC_3", 1);
//    test_BuildTreeNorm("2009_08_06_round_1_04ct_Lexus_Radius_no_1_no_5_missing_RBC_4", 1);
	//   test_J11("2009_08_17_brilliant_1.27ct_lexus_400c_wol_2", 1);
	//   test_BuildTreeNorm("2009_08_17_brilliant_1.27ct_lexus_400c_wol_2", 1);
//    test_J11("2009_08_17_brilliant_1.27ct_lexus_400c_wol_3", 1);
//    test_BuildTreeNorm("2009_08_17_brilliant_1.27ct_lexus_400c_wol_3", 1);
//    test_J11("2009_08_17_brilliant_1.27ct_lexus_400c_Wol_1", 1);
//    test_BuildTreeNorm("2009_08_17_brilliant_1.27ct_lexus_400c_Wol_1", 1);

	test_BuildClaster();

//    test_J11("poly-cube x 3", 0);
//   test_BuildTreeNorm("poly-cube x 3", 0);
	//   test_J11("H-cube", 0);
//    test_BuildTreeNorm("H-cube", 0);
//    test_J11("poly-cube", 0);
//    test_BuildTreeNorm("poly-cube", 0);
	//     test_J11("Pi-cube", 0);

//      test_Jtwo("Pi-cube", "H-cube", 0);
//      test_Jtwo("2009_08_17_brilliant_1.27ct_lexus_400c_wol_2"
//                , "2009_08_17_brilliant_1.27ct_lexus_400c_wol_3", 1);
//      test_BuildTreeNorm("Pi-cube", 0);
	//   test_cluster("poly-cube", 0);
	//test_viev("poly-cube", 0);

//    Polyhedron s;
//
//    double g;
//    s.poly_cube(2,0,0,0);
//    g = s.volume();
//    cout<<g<<endl;
	DEBUG_END;
	return 0;
}

void test_Jtwo(const char* name1, const char* name2, int type)
{
	DEBUG_START;

	Polyhedron poly1;
	Polyhedron poly2;

	char *file_name1 = NULL;
	char *file_name2 = NULL;
	file_name1 = new char[255];
	file_name2 = new char[255];

	sprintf(file_name1, "./poly-data-in/%s.dat", name1);
	sprintf(file_name2, "./poly-data-in/%s.dat", name2);

//    poly1.fscan_default_0(file_name1);
//    poly2.fscan_default_0(file_name2);

	switch (type)
	{
	case 0:
		poly1.fscan_default_0(file_name1);
		poly2.fscan_default_0(file_name2);
		break;
	case 1:
		poly1.fscan_default_1(file_name1);
		poly2.fscan_default_1(file_name2);
		break;
	case 2:
		poly1.fscan_default_1_1(file_name1);
		poly2.fscan_default_1_1(file_name2);
		break;
	default:
		if (file_name1 != NULL)
		{
			delete[] file_name1;
			file_name1 = NULL;
		}
		if (file_name2 != NULL)
		{
			delete[] file_name2;
			file_name2 = NULL;
		}
		DEBUG_END;
		return;
		break;
	}

	poly1.preprocessAdjacency();
	poly2.preprocessAdjacency();

	double Jxx1, Jyy1, Jzz1, Jxy1, Jyz1, Jxz1;
	double Jxx2, Jyy2, Jzz2, Jxy2, Jyz2, Jxz2;
	poly1.J(Jxx1, Jyy1, Jzz1, Jxy1, Jyz1, Jxz1);
	poly2.J(Jxx2, Jyy2, Jzz2, Jxy2, Jyz2, Jxz2);
	double l01, l11, l21;
	double l02, l12, l22;

	DEBUG_VARIABLE double v1 = poly1.volume();
	DEBUG_VARIABLE double v2 = poly1.volume();
	DEBUG_VARIABLE double s1 = poly2.areaOfSurface();
	DEBUG_VARIABLE double s2 = poly2.areaOfSurface();

	Vector3d v01;
	Vector3d v11;
	Vector3d v21;

	Vector3d v02;
	Vector3d v12;
	Vector3d v22;

	poly1.inertia(l01, l11, l21, v01, v11, v21);
	poly2.inertia(l02, l12, l22, v02, v12, v22);

	printf("%s\n", name1);
	//   printf("volume = %lf, area = %lf\n", v2, s2);
	//   printf("l0 = %lf, l1 = %lf, l2 = %lf\n", l01, l11, l21);

	printf("\t||\t%.16lf\t%.16lf\t%.16lf\t||\n", Jxx1, Jxy1, Jxz1);
	printf("J =\t||\t%.16lf\t%.16lf\t%.16lf\t||\n", Jxy1, Jyy1, Jyz1);
	printf("\t||\t%.16lf\t%.16lf\t%.16lf\t||\n", Jxz1, Jyz1, Jzz1);

//    printf("v0 = (%lf, %lf, %lf)\n", v01.x, v01.y, v01.z);
//    printf("v1 = (%lf, %lf, %lf)\n", v11.x, v11.y, v11.z);
//    printf("v2 = (%lf, %lf, %lf)\n\n", v21.x, v21.y, v21.z);

	printf("%s\n", name2);

	//  printf("volume = %lf, area = %lf\n", v2, s2);
	//  printf("l0 = %lf, l1 = %lf, l2 = %lf\n", l02, l12, l22);

	printf("\t||\t%.16lf\t%.16lf\t%.16lf\t||\n", Jxx2, Jxy2, Jxz2);
	printf("J =\t||\t%.16lf\t%.16lf\t%.16lf\t||\n", Jxy2, Jyy2, Jyz2);
	printf("\t||\t%.16lf\t%.16lf\t%.16lf\t||\n", Jxz2, Jyz2, Jzz2);

//    printf("v0 = (%lf, %lf, %lf)\n", v02.x, v02.y, v02.z);
//    printf("v1 = (%lf, %lf, %lf)\n", v12.x, v12.y, v12.z);
//    printf("v2 = (%lf, %lf, %lf)\n\n", v22.x, v22.y, v22.z);
//

	Jxx1 -= Jxx2;
	Jyy1 -= Jyy2;
	Jzz1 -= Jzz2;
	Jxy1 -= Jxy1;
	Jyz1 -= Jyz1;
	Jxz1 -= Jxz1;

	double norm1 = norm_J(Jxx1, Jyy1, Jzz1, Jxy1, Jyz1, Jxz1);
	printf("norm = %lf", norm1);

	if (file_name1 != NULL)
	{
		delete[] file_name1;
		file_name1 = NULL;
	}
	if (file_name2 != NULL)
	{
		delete[] file_name2;
		file_name2 = NULL;
	}
	DEBUG_END;
}

void test_J11(const char* name, int type)
{
	DEBUG_START;
	DEBUG_VARIABLE double vol;

	Polyhedron poly;

	char *file_name_in;
	char *file_name_out;
	file_name_in = new char[255];
	file_name_out = new char[255];

	sprintf(file_name_in, "./poly-data-in/%s.dat", name);
	sprintf(file_name_out, "./poly-data-out/%s.ply", name);

	printf("%s\n", name);
	switch (type)
	{
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
		DEBUG_END;
		return;
		break;
	}
	poly.preprocessAdjacency();
//    vol = poly.volume();

	double Jxx, Jyy, Jzz, Jxy, Jyz, Jxz;
	poly.J(Jxx, Jyy, Jzz, Jxy, Jyz, Jxz);
	double l0, l1, l2;

	double v = poly.volume();
	double s = poly.areaOfSurface();
	printf("volume = %lf, area = %lf\n", v, s);
	Vector3d v0;
	Vector3d v1;
	Vector3d v2;
	poly.inertia(l0, l1, l2, v0, v1, v2);

	printf("l0 = %lf, l1 = %lf, l2 = %lf\n", l0, l1, l2);
	printf("v0 = (%lf, %lf, %lf)\n", v0.x, v0.y, v0.z);
	printf("v1 = (%lf, %lf, %lf)\n", v1.x, v1.y, v1.z);
	printf("v2 = (%lf, %lf, %lf)\n\n", v2.x, v2.y, v2.z);

	poly.preprocessAdjacency();
	poly.fprint_ply_scale(1000., file_name_out, "J11");
//
//    double xc, yc, zc;

//    poly.get_center(xc, yc, zc);
//    printf("||\t%lf\t%lf\t%lf||\n", Jxx, Jxy, Jxz);
//    printf("||\t%lf\t%lf\t%lf||\n", Jxy, Jyy, Jyz);
//    printf("||\t%lf\t%lf\t%lf||\n", Jxz, Jyz, Jzz);

//    printf("center = (%lf,%lf,%lf) for %s.dat\n", xc, yc, zc,name);
//    printf("vol = %lf\n", vol);
	//double s =
	//poly.clusterisation2(0.03);// .volume();
	//poly.clusterisation2(0.001);
	//printf("%10.50lf\n", s);

	if (file_name_in != NULL)
		delete[] file_name_in;
	if (file_name_out != NULL)
		delete[] file_name_out;

	DEBUG_END;
}

void test_cluster(const char* name, int type)
{
	DEBUG_START;
	DEBUG_VARIABLE double vol;

	Polyhedron poly;

	char *file_name_in;
	file_name_in = new char[255];

	sprintf(file_name_in, "../poly-data-in/%s.dat", name);

	switch (type)
	{
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
		DEBUG_END;
		return;
		break;
	}

	poly.preprocessAdjacency();

	double p = EPSILON;
//    for (int i = 0; i < 20; ++i) {
//        int ncluster = poly.clusterisation(p);
//        DEBUG_PRINT("p = %le, \tncalster = %d\n", p, ncluster);
//        p *= 10.;
//    }

	poly.clusterize2(p);
	if (file_name_in != NULL)
		delete[] file_name_in;

	DEBUG_END;
}

void test_viev(const char* name, int type)
{
	DEBUG_START;
	DEBUG_VARIABLE double vol;

	Polyhedron poly;

	char *file_name_in;
	file_name_in = new char[255];

	sprintf(file_name_in, "../poly-data-in/%s.dat", name);

	switch (type)
	{
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
		DEBUG_END;
		return;
		break;
	}

	poly.preprocessAdjacency();
	double v = poly.volume();
	double a = poly.areaOfSurface();
	printf("volume = %lf\tarea = %lf\n", v, a);

	if (file_name_in != NULL)
		delete[] file_name_in;

	DEBUG_END;
}

#define num_len 10
#define step_len 10
#define num_rand 100

void get_statistics_deform_linear(const char* name, int type)
{
	DEBUG_START;

	int i_len, i_vertex, numv, i, count;
	double dist, norm, a, b, c, d, part, norm0;
	Vector3d delta;

	srand((unsigned) time(NULL));

	Polyhedron poly;
	Polyhedron poly1;
	char *file_name_in;
	file_name_in = new char[255];

	sprintf(file_name_in, "../poly-data-in/%s.dat", name);
	switch (type)
	{
	case 0:
		poly.fscan_default_0(file_name_in);
		poly1.fscan_default_0(file_name_in);
		break;
	case 1:
		poly.fscan_default_1(file_name_in);
		poly1.fscan_default_1(file_name_in);
		break;
	case 2:
		poly.fscan_default_1_1(file_name_in);
		poly1.fscan_default_1_1(file_name_in);
		break;
	default:
		if (file_name_in != NULL)
			delete[] file_name_in;
		DEBUG_END;
		return;
		break;
	}
	//    printf("Scanning ended...\n");
	poly.preprocessAdjacency();
	poly1.preprocessAdjacency();
	//    printf("Preprocessing ended...\n");
	numv = poly.numVertices;

	printf("NAME: %s\\", name);
	printf("\\hline\n\tnorm0 &\tpart\\\\\n");

	norm0 = 5.;
	for (i_len = 0; i_len < num_len; ++i_len)
	{
		count = 0;
		if (i_len % 2 == 0)
			norm0 *= 0.2;
		else
			norm0 *= 0.5;

		for (i = 0; i < num_rand; ++i)
		{
			i_vertex = (int) (rand() / (RAND_MAX + 1.) * numv);
//            printf("%d ", i_vertex);
			dist = poly.distToNearestNeighbour(i_vertex);
			norm = dist * norm0;

			a = rand();
			b = rand();
			c = rand();
			d = sqrt(a * a + b * b + c * c);
			a /= d;
			b /= d;
			c /= d;
			delta = norm * Vector3d(a, b, c);

			poly.shiftPointLinearPartial(i_vertex, delta, 10);

			if (poly.countConsections(true) > 0)
			{
				++count;
			}
			poly.copyCoordinates(poly1);
		}
		//        for (i_vertex = 0; i_vertex < 50 && i_vertex < numv; ++i_vertex) {
		//            dist = poly.min_dist(i_vertex);
		//            norm = dist * norm0;
		//
		//            for (i = 0; i < num_rand; ++i) {
		//                a = rand();
		//                b = rand();
		//                c = rand();
		//                d = sqrt(a * a + b * b + c * c);
		//                a /= d;
		//                b /= d;
		//                c /= d;
		//                delta = norm * Vector3d(a, b, c);
		//
		//                poly.deform_linear(i_vertex, delta);
		//
		//                if (poly.test_consections() > 0) {
		//                    ++count;
		//                }
		//                poly.import_coordinates(poly1);
		//            }
		//        }
		part = (double) count;
//        part /= numv;
		part /= num_rand;
		printf("\\hline\n\t%lf &\t%lf\\\\\n", norm0, part);
//        poly1.preprocess_polyhedron();
	}

	DEBUG_END;
}

void test_deform(const char* name, int type, int id, Vector3d delta)
{
	DEBUG_START;

	double xmin, xmax, ymin, ymax, zmin, zmax;
	Polyhedron poly;

	char *file_name_in, *file_name_out0, *file_name_out;

	fprintf(stdout, "******************************************************\n");
	fprintf(stdout, "********************   Deformation    ****************\n");
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

	switch (type)
	{
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
		DEBUG_END;
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

	poly.preprocessAdjacency();
	poly.shiftPointWeighted(id, delta);

	time_t seconds = time(NULL);
	tm* timeinfo = localtime(&seconds);

	sprintf(file_name_out, "../poly-data-out/%s - %d-%d-%d %d:%d:%d - %d.ply",
			name, timeinfo->tm_year + 1900, timeinfo->tm_mon + 1,
			timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min,
			timeinfo->tm_sec, id);

	poly.fprint_ply_scale(1000., file_name_out,
			"made-by-Polyhedron_join_facets_2");

	if (file_name_in != NULL)
		delete[] file_name_in;
	if (file_name_out0 != NULL)
		delete[] file_name_out0;
	if (file_name_out != NULL)
		delete[] file_name_out;

	DEBUG_END;
}

void test_deform_linear(const char* name, int type, int id, Vector3d delta)
{
	DEBUG_START;

	double xmin, xmax, ymin, ymax, zmin, zmax;
	Polyhedron poly;

	char *file_name_in, *file_name_out0, *file_name_out;

	fprintf(stdout, "******************************************************\n");
	fprintf(stdout,
			"********************   Deform linear    ****************\n");
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

	switch (type)
	{
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
		DEBUG_END;
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

	poly.preprocessAdjacency();
	poly.countConsections(true);
	poly.shiftPointLinearGlobal(id, delta);
	poly.countConsections(true);

	time_t seconds = time(NULL);
	tm* timeinfo = localtime(&seconds);

	sprintf(file_name_out, "../poly-data-out/%s - %d-%d-%d %d:%d:%d - %d.ply",
			name, timeinfo->tm_year + 1900, timeinfo->tm_mon + 1,
			timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min,
			timeinfo->tm_sec, id);

	poly.fprint_ply_scale(1000., file_name_out,
			"made-by-Polyhedron_join_facets_2");

	if (file_name_in != NULL)
		delete[] file_name_in;
	if (file_name_out0 != NULL)
		delete[] file_name_out0;
	if (file_name_out != NULL)
		delete[] file_name_out;

	DEBUG_END;
}

double norm_J(double Jxx, double Jyy, double Jzz, double Jxy, double Jyz,
		double Jxz)
{
	DEBUG_START;
	double st, nd, rd;
	st = fabs(Jxx) + fabs(Jxy) + fabs(Jxz);
	nd = fabs(Jxy) + fabs(Jyy) + fabs(Jyz);
	rd = fabs(Jxz) + fabs(Jyz) + fabs(Jzz);
	double max = st;
	if (nd > max)
		nd = max;
	if (rd > max)
		rd = max;
	DEBUG_END;
	return max;
}

void test_BuildClaster()
{
	DEBUG_START;
	Polyhedron poly1;
	Polyhedron poly2;
	Polyhedron poly3;

	char *file_name_in1 = NULL;
	char *file_name_out1 = NULL;
	char *file_name_in2 = NULL;
	char *file_name_out2 = NULL;
	char *file_name_in3 = NULL;
	char *file_name_out3 = NULL;
	char *name = NULL;
	file_name_in1 = new char[255];
	file_name_out1 = new char[255];
	file_name_in2 = new char[255];
	file_name_out2 = new char[255];
	file_name_in3 = new char[255];
	file_name_out3 = new char[255];
	name = new char[255];

	strcpy(name, "2009_08_17_brilliant_1.27ct_lexus_400c_wol_2");
	sprintf(file_name_in2, "../poly-data-in/%s.dat", name);
	sprintf(file_name_out2, "../poly-data-out/%s - cluster.ply", name);

	strcpy(name, "2009_08_17_brilliant_1.27ct_lexus_400c_wol_3");
	sprintf(file_name_in3, "../poly-data-in/%s.dat", name);
	sprintf(file_name_out3, "../poly-data-out/%s - cluster.ply", name);

	strcpy(name, "2009_08_17_brilliant_1.27ct_lexus_400c_Wol_1");
	sprintf(file_name_in1, "../poly-data-in/%s.dat", name);
	sprintf(file_name_out1, "../poly-data-out/%s - cluster.ply", name);

	poly1.fscan_default_1(file_name_in1);
	poly2.fscan_default_1(file_name_in2);
	poly3.fscan_default_1(file_name_in3);

	poly1.preprocessAdjacency();
	poly2.preprocessAdjacency();
	poly3.preprocessAdjacency();

	double v = poly1.volume();
	double s = poly1.areaOfSurface();
	printf("volume = %10.50lf, area = %10.50lf\n", v, s);

	TreeClusterNormNode *nodeArray;        // = new TreeClusterNormNode;
	nodeArray = new TreeClusterNormNode[poly1.numFacets];
	MatrixDistNorm matrix(poly1.numFacets);
	poly1.giveClusterNodeArray(nodeArray, matrix); //.fprint_dendrogamma_lev(stdout);

	TreeClusterNormNode *nodeArray1;        // = new TreeClusterNormNode;
	nodeArray1 = new TreeClusterNormNode[poly1.numFacets];
	MatrixDistNorm matrix1(poly1.numFacets);

	TreeClusterNormNode *nodeArray2;        // = new TreeClusterNormNode;
	nodeArray2 = new TreeClusterNormNode[poly2.numFacets];
	MatrixDistNorm matrix2(poly2.numFacets);

	TreeClusterNormNode *nodeArray3;        // = new TreeClusterNormNode;
	nodeArray3 = new TreeClusterNormNode[poly3.numFacets];
	MatrixDistNorm matrix3(poly3.numFacets);

	poly1.reClusterNodeArray(nodeArray, matrix, nodeArray1, matrix1);
	poly2.reClusterNodeArray(nodeArray, matrix, nodeArray2, matrix2);
	poly3.reClusterNodeArray(nodeArray, matrix, nodeArray3, matrix3);

//    MatrixDistNorm matrix(poly1.numf);

//    matrix.build(poly1.numf, nodeArray);

	printf("==========  Clusterization for 1 ==============\n");
	matrix.fprint_clusters2(stdout, nodeArray1);
	printf("==========  Clusterization for 2 ==============\n");
	matrix.fprint_clusters2(stdout, nodeArray2);
	printf("==========  Clusterization for 3 ==============\n");
	matrix.fprint_clusters2(stdout, nodeArray3);

	matrix.setColors2(nodeArray1);
	matrix.setColors2(nodeArray2);
	matrix.setColors2(nodeArray3);

	double n12 = matrix.sqNorm(nodeArray1, nodeArray2);
	double n13 = matrix.sqNorm(nodeArray1, nodeArray3);
	double n23 = matrix.sqNorm(nodeArray2, nodeArray3);
	printf("n12 = %lf\n", n12);
	printf("n13 = %lf\n", n13);
	printf("n23 = %lf\n", n23);

	poly1.fprint_ply_scale(1000., file_name_out1, "cluster");
	poly2.fprint_ply_scale(1000., file_name_out2, "cluster");
	poly3.fprint_ply_scale(1000., file_name_out3, "cluster");

	if (file_name_in1 != NULL)
		delete[] file_name_in1;
	if (file_name_out2 != NULL)
		delete[] file_name_out2;
	if (file_name_in3 != NULL)
		delete[] file_name_in3;
	if (file_name_out1 != NULL)
		delete[] file_name_out1;
	if (file_name_in2 != NULL)
		delete[] file_name_in2;
	if (file_name_out3 != NULL)
		delete[] file_name_out3;
	if (name != NULL)
		delete[] name;

	DEBUG_END;
}

void test_BuildTreeNorm(const char* name, int type)
{
	DEBUG_START;
	Polyhedron poly;

	char *file_name_in;
	char *file_name_out;
	file_name_in = new char[255];
	file_name_out = new char[255];

	sprintf(file_name_in, "./poly-data-in/%s.dat", name);
	sprintf(file_name_out, "../poly-data-out/%s - cluster.ply", name);

	printf("%s\n", name);
	switch (type)
	{
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
		DEBUG_END;
		return;
		break;
	}
	poly.preprocessAdjacency();

	double v = poly.volume();
	double s = poly.areaOfSurface();
	printf("volume = %10.50lf, area = %10.50lf\n", v, s);

	poly.build_TreeClusterNorm();        //.fprint_dendrogamma_lev(stdout);

	poly.fprint_ply_scale(1000., file_name_out, "cluster");

	if (file_name_in != NULL)
		delete[] file_name_in;
	if (file_name_out != NULL)
		delete[] file_name_out;
	DEBUG_END;
}

