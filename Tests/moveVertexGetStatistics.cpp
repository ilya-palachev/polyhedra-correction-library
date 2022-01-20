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

void get_statistics_deform_linear(const char *name, int type);
void get_statistics_deform_linear_test(const char *name, int type);

int main(int argc, char **argv)
{
	DEBUG_START;
	get_statistics_deform_linear_test("poly-small", 0);

	get_statistics_deform_linear("poly-small", 0);
	get_statistics_deform_linear("poly-med", 0);
	get_statistics_deform_linear("poly-big", 0);
	get_statistics_deform_linear("polyhedron-2010-11-25", 2);
	get_statistics_deform_linear("polyhedron-2010-12-19", 2);
	DEBUG_END;
}

#define num_len 10
#define step_len 10
#define num_rand 100

void get_statistics_deform_linear(const char *name, int type)
{
	DEBUG_START;

	int i_len, i_vertex, numv, i, count;
	double dist, norm, a, b, c, d, part, norm0;
	Vector3d delta;

	srand((unsigned)time(NULL));

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
			i_vertex = (int)(rand() / (RAND_MAX + 1.) * numv);
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
		//        for (i_vertex = 0; i_vertex < 50 && i_vertex < numv;
		//        ++i_vertex) {
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
		part = (double)count;
		//        part /= numv;
		part /= num_rand;
		printf("\\hline\n\t%lf &\t%lf\\\\\n", norm0, part);
		//        poly1.preprocess_polyhedron();
	}

	DEBUG_END;
}

void get_statistics_deform_linear_test(const char *name, int type)
{
	DEBUG_START;

	int i_vertex, numv, i;
	double dist, norm, a, b, c, d;
	Vector3d delta;

	int num_steps0, num_steps1;
	double norm_sum0, norm_sum1;

	srand((unsigned)time(NULL));

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

	for (i = 0; i < num_rand; ++i)
	{
		i_vertex = (int)(rand() / (RAND_MAX + 1.) * numv);
		//            printf("%d ", i_vertex);
		dist = poly.distToNearestNeighbour(i_vertex);
		norm = dist * 0.0001;

		a = rand();
		b = rand();
		c = rand();
		d = sqrt(a * a + b * b + c * c);
		a /= d;
		b /= d;
		c /= d;
		delta = norm * Vector3d(a, b, c);

		poly.shiftPointLinearTest(i_vertex, delta, 0, num_steps0, norm_sum0);
		poly.copyCoordinates(poly1);
		num_steps1 = -1;
		poly.shiftPointLinearTest(i_vertex, delta, 1, num_steps1, norm_sum1);
		poly.copyCoordinates(poly1);
		if (num_steps1 != -1)
		{
			printf("\\hline\n%d &\t(%.2le, %.2le, %.2le) &\t%d &\t%le &\t%d "
				   "&\t%le \\\\\n",
				   i_vertex, delta.x, delta.y, delta.z, num_steps0, norm_sum0, num_steps1, norm_sum1);
		}
	}
	DEBUG_END;
}
