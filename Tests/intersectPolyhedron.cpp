/*
 * Copyright (c) 2009-2016 Ilya Palachev <iliyapalachev@gmail.com>
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

void test(const char *name, int type, double a, double b, double c, double d);

int main(int argc, char **argv)
{
	DEBUG_START;
	test("poly-small", 0, 0., 0., 1., 3.);
	test("poly-med", 0, 0., 0., 1., 1.);

	test("poly-big", 0, 1., 0., 0., 3.);
	test("poly-big", 0, 1., 0., 0., 3.1);
	test("poly-big", 0, 1., 0., 0., 3.3);
	test("poly-big", 0, 1., 0., 0., 3.4);
	test("poly-big", 0, 1., 0., 0., 3.5);

	Vector3d origin, normal;
	double a, b, c, d;

	origin = Vector3d(-3308.14593415889, -85.2348088534985, -2744.2585328124) * 0.001;
	normal = Vector3d(0.9907942064402, 0.134257976187036, -0.017367680175984);
	a = normal.x;
	b = normal.y;
	c = normal.z;
	d = -origin * normal;
	test("poly-big", 0, a, b, c, d);
	DEBUG_END;
}

void test(const char *name, int type, double a, double b, double c, double d)
{
	DEBUG_START;
	double xmin, xmax, ymin, ymax, zmin, zmax;
	Plane iplane;
	Polyhedron poly;

	char *file_name_in, *file_name_out0, *file_name_out;
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
		if (file_name_out0 != NULL)
			delete[] file_name_out0;
		if (file_name_out != NULL)
			delete[] file_name_out;
		DEBUG_END;
		return;
		break;
	}

	poly.fprint_ply_scale(1000., file_name_out0, "made-by-Polyhedron_join_facets_2");

	poly.get_boundary(xmin, xmax, ymin, ymax, zmin, zmax);
	fprintf(stdout, "boundary : \n");
	fprintf(stdout, "x [%lf,  %lf]\n", xmin, xmax);
	fprintf(stdout, "y [%lf,  %lf]\n", ymin, ymax);
	fprintf(stdout, "z [%lf,  %lf]\n", zmin, zmax);

	poly.preprocessAdjacency();
	poly.intersect(Plane(Vector3d(a, b, c), d));

	time_t seconds = time(NULL);
	tm *timeinfo = localtime(&seconds);

	sprintf(file_name_out, "../poly-data-out/%s - %d-%d-%d %d:%d:%d.ply", name, timeinfo->tm_year + 1900,
			timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

	poly.fprint_ply_scale(1000., file_name_out, "made-by-Polyhedron_join_facets_2");

	if (file_name_in != NULL)
		delete[] file_name_in;
	if (file_name_out0 != NULL)
		delete[] file_name_out0;
	if (file_name_out != NULL)
		delete[] file_name_out;
	DEBUG_END;
}
