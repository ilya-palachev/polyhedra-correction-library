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

#include <cmath>

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "Vector3d.h"
#include "array_operations.h"
#include "Correctors/Intersector/EdgeList/EdgeList.h"
#include "Polyhedron/Facet/Facet.h"

void EdgeList::add_edge(int v0, int v1, int i0, int i1, double sm)
{
	DEBUG_START;
	add_edge(v0, v1, i0, i1, -1, -2, sm);
	DEBUG_END;
}

void EdgeList::add_edge(int v0, int v1, int i0, int i1, int next_f, int next_d,
		double sm)
{
	DEBUG_START;
	DEBUG_PRINT("add_edge(v0 = %d, v1 = %d, next_f = %d, next_d = %d, \
			sm = %lf)\n", v0, v1, next_f, next_d, sm);

	if (v0 > v1)
	{
		int tmp = v0;
		v0 = v1;
		v1 = tmp;
	}

	int first = 0; // Первый элемент в массиве
	int last = num; // Последний элемент в массиве

	while (first < last)
	{
		int mid = (first + last) / 2;
		if (sm <= scalar_mult[mid])
		{
			last = mid;
		}
		else
		{
			first = mid + 1;
		}
	}
	insert_int(edge0, num, last, v0);
	insert_int(edge1, num, last, v1);
	insert_int(ind0, num, last, i0);
	insert_int(ind1, num, last, i1);
	insert_int(next_facet, num, last, next_f);
	insert_int(next_direction, num, last, next_d);
	insert_double(scalar_mult, num, last, sm);
	insert_int(id_v_new, num, last, -1);
	insert_bool(isUsed, num, last, false);

	++num;
	DEBUG_END;
}

void EdgeList::send(EdgeSetIntersected* edge_set)
{
	DEBUG_START;
	int i;
	for (i = 0; i < num; ++i)
	{
		edge_set->add_edge(edge0[i], edge1[i]);
	}
	DEBUG_END;
}

void EdgeList::send_edges(EdgeSetIntersected* edge_set)
{
	DEBUG_START;
	int i;
	for (i = 0; i < num; ++i)
	{
		if (edge0[i] != edge1[i])
			edge_set->add_edge(edge0[i], edge1[i]);
	}
	DEBUG_END;
}

void EdgeList::set_curr_info(int next_d, int next_f)
{
	DEBUG_START;
	next_direction[pointer] = next_d;
	next_facet[pointer] = next_f;
	DEBUG_END;
}

void EdgeList::search_and_set_info(int v0, int v1, int next_d, int next_f)
{
	DEBUG_START;
	int i, tmp;
	if (v0 > v1)
	{
		tmp = v0;
		v0 = v1;
		v1 = tmp;
	}
	for (i = 0; i < num; ++i)
	{
		if (edge0[i] == v0 && edge1[i] == v1)
		{
			next_direction[i] = next_d;
			next_facet[i] = next_f;
			DEBUG_END;
			return;
		}
	}
	ERROR_PRINT("Error. Edge (%d, %d) not found...", v0, v1);
	DEBUG_END;
	return;
}

void EdgeList::null_isUsed()
{
	DEBUG_START;
	for (int i = 0; i < num; ++i)
	{
		isUsed[i] = false;
	}
	DEBUG_END;
}

//bool EdgeList::get_first_edge(int& v0, int& v1) {
//	if (num < 1) {
//		v0 = v1 = - 1;
//		return false;
//	}
//	for (int i = 0; i < num; ++i) {
//		if (next_direction[i] != 0 && isUsed[i] == false) {
//			v0 = edge0[i];
//			v1 = edge1[i];
////			set_pointer(i);
//			isUsed[i] = true;
//			return true;
//		}
//	}
//	v0 = v1 = - 1;
//	return false;
//}

void EdgeList::get_first_edge(int& v0, int& v1, int& next_f, int& next_d)
{
	DEBUG_START;

	if (num < 1)
	{
		v0 = v1 = -1;
		return;
	}
	for (int i = 0; i < num; ++i)
	{
		if (next_direction[i] != 0 && isUsed[i] == false)
		{
			//			set_pointer(i);
			isUsed[i] = true;
			v0 = edge0[i];
			v1 = edge1[i];
			next_f = next_facet[i];
			next_d = next_direction[i];
			DEBUG_END;
			return;
		}
	}
	//	set_pointer(0);
	isUsed[0] = true;
	v0 = edge0[0];
	v1 = edge1[0];
	next_f = next_facet[0];
	next_d = -1;
	DEBUG_END;
}

void EdgeList::get_first_edge(int& v0, int& v1)
{
	int next_f, next_d;
	get_first_edge(v0, v1, next_f, next_d);
}

void EdgeList::get_next_edge(Plane iplane, int& v0, int& v1, int& i0, int& i1,
		int& next_f, int& next_d)
{
	DEBUG_START;
	int i, tmp, i_next = -1;
	int id0;
	int id1;
	int sign0;
	int sign1;
	int incr = 1;
	int nv;
	Plane plane;
	plane = poly->facets[next_f].plane;

	double err;
	err = qmod(plane.norm - iplane.norm)
			+ (plane.dist - iplane.dist) * (plane.dist - iplane.dist);
	if (num < 1 && (fabs(err) > 1e-16))
	{
		err = qmod(plane.norm + iplane.norm)
				+ (plane.dist + iplane.dist) * (plane.dist + iplane.dist);
	}

	if (num < 1 && fabs(err) <= 0.0000000000000001)
	{
		DEBUG_PRINT("\t\t --- SPECIAL FACET ---\n\n");

		sign0 = poly->signum(poly->vertices[v0], iplane);
		sign1 = poly->signum(poly->vertices[v1], iplane);
		if (1)
		{
			id0 = poly->facets[next_f].find_vertex(v0);
			id1 = poly->facets[next_f].find_vertex(v1);
			if (id0 == -1 || id1 == -1)
			{
				ERROR_PRINT("\tError: cannot find vertexes %d (%d) "
						"and %d (%d) facet %d.\n",
						v0, id0, v1, id1, next_f);
				DEBUG_END;
				return;
			}
			nv = poly->facets[next_f].numVertices;
			if (sign0 >= 0 && sign1 <= 0)
			{
				if (id1 == id0 + 1 || (id1 == 0 && id0 == nv - 1))
					incr = 1;
				else if (id1 == id0 - 1 || (id0 == 0 && id1 == nv - 1))
					incr = -1;
				id0 = id1;
				v0 = v1;
				id1 = (id0 + incr + nv) % nv;
				v1 = poly->facets[next_f].indVertices[id1];
			}
			else if (sign0 <= 0 && sign1 >= 0)
			{
				if (id1 == id0 + 1 || (id1 == 0 && id0 == nv - 1))
					incr = -1;
				else if (id1 == id0 - 1 || (id0 == 0 && id1 == nv - 1))
					incr = 1;
				id1 = (id0 + incr + nv) % nv;
				v1 = poly->facets[next_f].indVertices[id1];
			}
			else if (sign0 <= 0 && sign1 <= 0 && (next_d == -1 || next_d == 1))
			{
				incr = next_d;
				if ((id1 - id0 + nv) % nv == next_d)
				{
					id0 = id1;
					v0 = v1;
					id1 = (id0 + incr + nv) % nv;
					v1 = poly->facets[next_f].indVertices[id1];
				}
				else
				{
					id1 = (id0 + incr + nv) % nv;
					v1 = poly->facets[next_f].indVertices[id1];
				}
			}
			else
			{
				ERROR_PRINT("Error. Cannot define the direction.");
				ERROR_PRINT("sign(%d) = %d, sign(%d) = %d, drctn = %d",
						v0, sign0, v1, sign1, next_d);
				DEBUG_END;
				return;
			}

			sign0 = poly->signum(poly->vertices[v0], iplane);
			sign1 = poly->signum(poly->vertices[v1], iplane);
			if (sign1 == 1)
			{
				//next_f = :
				poly->facets[next_f].find_next_facet(incr == 1 ? v0 : v1,
						next_f);
				next_d = 0;
			}
			else
			{
				next_d = incr;
			}

		}
		v1 = v0;
#ifdef DEBUG1
		DEBUG_PRINT("\tRESULTING NEXT EDGE : %d %d - GO TO FACET %d\n",
				v0, v1, next_f);
#endif

		DEBUG_END;
		return;
	}
	else if (num < 1)
	{
		ERROR_PRINT("Error. num < 1,  err = %.16lf, if = %d\n", err,
				fabs(err) < 1e-16);
		DEBUG_END;
		return;
	}
	if (v0 > v1)
	{
		tmp = v0;
		v0 = v1;
		v1 = tmp;
	}
#ifdef DEBUG1
	DEBUG_PRINT("EdgeList::get_next_edge %d\n",
			id);
	//        this->my_fprint(stdout);
#endif
	for (i = 0; i < num; ++i)
	{
		//#ifdef DEBUG1
		//        DEBUG_PRINT("   edge0[%d] = %d, edge1[%d] = %d\n",
		//                i, edge0[i], i, edge1[i]);
		//#endif
		if (edge0[i] == v0 && edge1[i] == v1)
		{
			//			set_pointer(i);
			isUsed[i] = true;
			switch (next_direction[i])
			{
			case 1:
				i_next = i < num - 1 ? i + 1 : 0;
				break;
			case -1:
				i_next = i > 0 ? i - 1 : num - 1;
				break;
			case 0:
				switch (next_d)
				{
				case 1:
					i_next = i < num - 1 ? i + 1 : 0;
					break;
				case -1:
					i_next = i > 0 ? i - 1 : num - 1;
					break;
				default:
					break;
				}
				break;
			default:
				break;
			}
			v0 = edge0[i_next];
			v1 = edge1[i_next];
			i0 = ind0[i_next];
			i1 = ind1[i_next];
			next_f = next_facet[i_next];
			if (next_direction[i] != 0)
				next_d = next_direction[i];
			isUsed[i_next] = true;
#ifdef DEBUG1
			DEBUG_PRINT("\tRESULTING NEXT EDGE : %d %d - GO TO FACET %d\n",
					v0, v1, next_f);
#endif

			DEBUG_END;
			return;
		}
	}
	//	set_pointer(0);
	v0 = v1 = next_f = -1;
	next_d = -2;
	DEBUG_END;
}

void EdgeList::get_next_edge(Plane iplane, int& v0, int& v1, int& next_f,
		int& next_d)
{
	DEBUG_START;
	int i0, i1;
	get_next_edge(iplane, v0, v1, i0, i1, next_f, next_d);
	DEBUG_END;
}

int EdgeList::get_pointer()
{
	DEBUG_START;
	DEBUG_END;
	return pointer;
}

void EdgeList::set_isUsed(int v0, int v1, bool val)
{
	DEBUG_START;
	int i;

	for (i = 0; i < num; ++i)
	{
		if (edge0[i] == v0 && edge1[i] == v1)
		{
			isUsed[i] = val;
			break;
		}
	}
	DEBUG_END;
}

void EdgeList::set_poly(PolyhedronPtr poly_orig)
{
	DEBUG_START;
	poly = poly_orig;
	DEBUG_END;
}
