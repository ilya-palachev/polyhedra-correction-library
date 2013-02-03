#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Polyhedron.h"

#include "array_operations.h"

int Polyhedron::signum(int i, Plane plane)
{
	double d = plane % vertex[i];
	if (fabs(d) < 1e-14)
		return 0;
	return d > 0 ? 1 : (d < 0 ? -1 : 0);
}


Vector3d Polyhedron::intersection_edge(int i0, int i1, Plane plane)
{
	Vector3d res;
	Vector3d v0 = vertex[i0];
	Vector3d v1 = vertex[i1];
	Vector3d dir = v1 - v0;
	intersection(plane, dir, v0, res);
	return res;
}

int Polyhedron::intersection_facet_create_edge_list(
		int facet_id, Plane plane, int* edge_list)
{
	int i;
	int nv;
	int* index;
	double* scalar_mult;

	int sign0, sign1;

	Vector3d dir;
	Vector3d point;
	Vector3d v0;
	Vector3d v1;
	Vector3d v_intrsct;

	int nintrsct;
	double scalar;

	nv = facet[facet_id].nv;
	index = facet[facet_id].index;

	//	scalar_mult = new double[(int) 0.5 * sizeof(edge_list) / sizeof(edge_list[0])];
	scalar_mult = new double[nv];

	//Вычисляем напр вектор пересечения грани с плоскостью
	bool if_intersect = intersection(plane, facet[facet_id].plane, dir, point);
	if(!if_intersect)
	{
		printf("intersection_facet_create_edge_list : Parallel facet\n");
		return 0;
	}

	nintrsct = 0;
	for(i = 0; i < nv; ++i)
	{
		sign0 = signum(index[i], plane);
		sign1 = signum(index[i + 1], plane);

		//Если вершина лежит на плоскости
		if(sign0 == 0)
		{
			scalar = dir * (vertex[index[i]] - point);
			insert_binary(nintrsct, edge_list, scalar_mult, index[i], index[i], scalar);
			++nintrsct;
		}

		//Если ребро лежит на плоскости
		if(sign0 * sign1 == -1)
		{
			v0 = vertex[index[i]];
			v1 = vertex[index[i + 1]];
			intersection(plane, v1 - v0, v0, v_intrsct);
			scalar = dir * (v_intrsct - point);
			insert_binary(nintrsct, edge_list, scalar_mult, index[i], index[i + 1], scalar);
			++nintrsct;
		}
	}

	//debug
//	if(nintrsct != 0)
//	{
//		printf("intersection_facet_create_edge_list : facet %d, intrsct = %d\n",
//				 facet_id, nintrsct);
//		for(i = 0; i < nintrsct; ++i)
//			printf("\t(%d, %d, %lf)\n",
//					 edge_list[2 * i], edge_list[2 * i + 1], scalar_mult[i]);
//	}
	if(scalar_mult != NULL) delete[] scalar_mult;

	return nintrsct;
}

void Polyhedron::test_intersection_facet_create_edge_list(Plane plane)
{
	int i;
	int* edge_list;
//	int nv;

	edge_list = (int*) new int[20000]; //TODO
	for(i = 0; i < numf; ++i)
	{
//		printf("facet %d\n", i);
//		nv = facet[i].nv;
//		if(edge_list != NULL) delete[] edge_list;
//		edge_list = new int[2 * nv];
		intersection_facet_create_edge_list(i, plane, edge_list);
//		printf("facet %d end\n", i);
	}
	delete[] edge_list;
}

void Polyhedron::intersection_facet_step(
		Plane plane,
		int facet_id, int v0, int v1,
		int& facet_id_new, int& v0_new, int& v1_new)
{
	int nv, *index, *edge_list, nintrsct;
	int if_inside;
	int i, j;
	int v0_curr, v1_curr;
	int id;
	int nf;
	int* index1;
	int f_id;
	int pos;
	int v0_new_prev, v0_new_next;
	int sign0, sign1;
	int v0_curr_prev, v0_curr_next;


	nv = facet[facet_id].nv;
	index = facet[facet_id].index;
	edge_list = new int[2 * nv];
	nintrsct = intersection_facet_create_edge_list(facet_id, plane, edge_list);

	if_inside = -1;
	for(i = 0; i < nintrsct; ++i)
	{
		v0_curr = edge_list[2 * i];
		v1_curr = edge_list[2 * i + 1];

		//В случае если текущее ребро есть начальное, определяем следующее ребро и
		//грань и выходим из цикла
		if((v0_curr == v0 && v1_curr == v1) || (v0_curr == v1 && v1_curr == v0))
		{

			//Определение следующего ребра {v0_new, v1_new}
			if(if_inside == -1)
			{
				if(i == nintrsct - 1)
				{
					printf("intersection_facet_step : Error 1.\n");
					return;
				}
				v0_new = edge_list[2 * i + 2];
				v1_new = edge_list[2 * i + 3];
			}
			else if(if_inside == 1)
			{

				if(i == 0)
				{
					printf("intersection_facet_step : Error 1.\n");
					return;
				}
				v0_new = edge_list[2 * i - 2];
				v1_new = edge_list[2 * i - 1];
			}
			//Определение следующей грани facet_id_new

			//Случай перехода по ребру
			if(v0_new != v1_new)
			{
				id = find_vertex(facet_id, v0_new);
				if(index[id + 1] == v1_new)
				{
					facet_id_new = index[id + nv + 1];
				}
				else if(index[id > 0 ? id - 1: nv - 1] == v1_new)
				{
					facet_id_new = index[(id > 0 ? id - 1: nv - 1) + nv + 1];
				}
			}
			//Случай перехода по вершине
			else if(v0_new == v1_new)
			{
				id = find_vertex(facet_id, v0_new);
				//Ищем, какая из граней, содержащих вершину, будет следующей
				nf = vertexinfo[v0_new].nf;
				index1 = vertexinfo[v0_new].index;
				for(j = 0; j < nf; ++j)
				{
					f_id = index1[j];
					if(f_id == facet_id)
					{
						continue;
					}
					pos = index1[j + nf + 1];
					//Для этого нужно, чтобы соседние вершины в этой грани для этой вершины
					//лежали по разные стороны от плоскости или одна них лежала на плоскости
					v0_new_prev = facet[f_id].index[pos > 0 ? pos - 1 : facet[f_id].nv - 1];
					v0_new_next = facet[f_id].index[pos + 1];
					sign0 = signum(v0_new_prev, plane);
					sign1 = signum(v0_new_prev, plane);
					if(sign0 * sign1 == -1 || sign0 == 0 || sign0 == 1)
					{
						facet_id_new = f_id;
						break;
					}
				}
				if(j == nf)
				{
					printf("intersection_facet_step : Error 2.\n");
					return;
				}

			}
			break;
		}

		//Если текущее ребро не начальное, определяем где мы: внутри или вне грани
		if(v0_curr != v1_curr)
		{
			if_inside *= -1;
		}
		//В случае вершины это определяется по знакам соседних с ней вершин
		else if(v0_curr == v1_curr)
		{
			id = find_vertex(facet_id, v0_curr);
			v0_curr_prev = index[id > 0 ? id - 1 : nv - 1];
			v0_curr_next = index[id + 1];
			sign0 = signum(v0_curr_prev, plane);
			sign1 = signum(v0_curr_next, plane);
			if(sign0 * sign1 == -1 || (sign0 == 0 && sign1 != 0) ||
				(sign0 != 0 && sign1 == 0))
			{
				if_inside *= -1;
			}
		}
	}
}

void Polyhedron::intersection_facet_step_cut(
		Plane plane,
		int facet_id, int v0, int v1,
		int& facet_id_new, int& v0_new, int& v1_new,
		int new_vertex, int &len_vertex)
{
	int nv;
	int id0, id1, id2, id3;
	int *index;
	int i, ii;


	nv = facet[facet_id].nv;
	index = facet[facet_id].index;

	intersection_facet_step(plane, facet_id, v0, v1, facet_id_new, v0_new, v1_new);

	//debug
	printf("intersection_facet_step_cut: facet %d\n", facet_id);
	printf("\tv0 = %d, v1 = %d, v0_new = %d, v1_new = %d\n", v0, v1, v0_new, v1_new);


	//Удаляем вершины между текущим и новым ребром в текущей грани
	id0 = find_vertex(facet_id, v0);
	id1 = find_vertex(facet_id, v1);
	id2 = find_vertex(facet_id, v0_new);
	id3 = find_vertex(facet_id, v1_new);

//	printf("id0 = %d, id1 = %d, id2 = %d, id3 = %d\n",
//			 id0, id1, id2, id3);


//	if(id0 > id1)
//	{
//		swap_int(id0, id1);
//	}
//	//id1 >= id0
//	if(id2 > id3)
//	{
//		swap_int(id2, id3);
//	}
//	//id3 >= id2
//	if(id0 >= id3)
//	{
//		swap_int(id0, id3);
//		swap_int(id1, id2);
//	}
//	//id3 >= id2 >= id1 >= id0
//	if(id0 == id1)
//	{
//		++id1;
//	}

//	if(id2 == id3)
//	{
//		--id2;
//	}

//	//2010 - 12 -16
//	for()


	//2010 - 12 - 09

	if(signum(v0, plane) == 1)
		id1 = id0;
	else if(signum(v1, plane) == 1)
		id1 = id1;
	if(v0 != v1)
	{
		insert_int(index, nv++, id1, new_vertex);
		printf("adding %d to facet %d\n", new_vertex, facet_id);
	}
	else if(signum(v0, plane) == 0 && signum(v1, plane) == 0)
		id1 = id0;

	if(signum(v0_new, plane) == 1)
		id2 = id2;
	else if(signum(v1_new, plane) == 1)
		id2 = id3;
	if(v0_new != v1_new)
	{
		insert_int(index, nv++, id2, len_vertex);
		printf("adding %d to facet %d\n", len_vertex, facet_id);
		++len_vertex;
	}
	else if(signum(v0_new, plane) == 0 && signum(v1_new, plane) == 0)
		id2 = id2;

	//debug
	for(i = 0; i < nv; ++i)
	{
		printf("\t%d\tsignum = %d\n", index[i], signum(index[i], plane));
	}
	printf("\n");


	for(i = 0, ii = 0; i < nv; ++i)
	{
		if(nv < 0)
			break;
		if(signum(index[ii], plane) == 1 &&
			(index[ii] != new_vertex || v0 == v1) &&
			(index[ii] != len_vertex || v0_new == v1_new) &&
			(index[ii] < numv))
		{
			printf("\tcutting %d\tsignum = %d\n", index[ii], signum(index[ii], plane));
//			printf("i = %d, nv = %d\n", i, facet[facet_id].nv);
			cut_int(index, nv, ii);
			--nv;
		}
		else
			++ii;
	}
}

//	if(signum(index[id1 < nv - 1 ? id1 + 1 : 0], plane) == 1)
//	{
//		if(id1 == 0)
//		{
//			index[nv] = index[1];
//		}

//		if(v0 != v1)
//		{
//			index[id1] = new_vertex;
//			id1 = (id1 < nv - 1 ? id1 + 1 : 0);

////			cut_int(index, 3 * nv + 1, id1 + 1 + 2 * nv);
////			cut_int(index, 3 * nv, id1 + 1 + nv);
////			cut_int(index, 3 * nv - 1, id1);
////			--nv;
//		}
//		if(v0_new != v1_new)
//		{
//			index[id2] = len_vertex++;
//			id2 = (id2 > 0 ? id2 - 1 : nv - 1);

////			cut_int(index, 3 * nv + 1, id1 + 1 + 2 * nv);
////			cut_int(index, 3 * nv, id1 + 1 + nv);
////			cut_int(index, 3 * nv - 1, id1);
////			--nv;
//		}
//		for(i = id1; i <= id2; i = (i < nv - 1 ? i + 1 : 0))
//		{
//			if(nv < 0)
//				break;
//			printf("i = %d, nv = %d\n", i, facet[facet_id].nv);
//			cut_int(index, 3 * nv + 1, id1 + 1 + 2 * nv);
//			cut_int(index, 3 * nv, id1 + 1 + nv);
//			cut_int(index, 3 * nv - 1, id1);
//			--nv;
//		}
//	}

//	else
//	{
//		if(id2 == 0)
//		{
//			index[nv] = index[1];
//		}

//		if(v0 != v1)
//		{
//			index[id1] = new_vertex;
//			id1 = (id1 > 0 ? id1 - 1 : nv - 1);

////			cut_int(index, 3 * nv + 1, id2 + 1 + 2 * nv);
////			cut_int(index, 3 * nv, id2 + 1 + nv);
////			cut_int(index, 3 * nv - 1, id2);
////			--nv;
//		}
//		if(v0_new != v1_new)
//		{
//			index[id2] = len_vertex++;
//			id2 = (id2 < nv - 1 ? id2 + 1 : 0);

////			cut_int(index, 3 * nv + 1, id2 + 1 + 2 * nv);
////			cut_int(index, 3 * nv, id2 + 1 + nv);
////			cut_int(index, 3 * nv - 1, id2);
////			--nv;
//		}
//		for(i = id2; i <= id1; i = (i < nv - 1 ? i + 1 : 0))
//		{
//			if(nv < 0)
//				break;
//			printf("i = %d, nv = %d\n", i, facet[facet_id].nv);
//			cut_int(index, 3 * nv + 1, id2 + 1 + 2 * nv);
//			cut_int(index, 3 * nv, id2 + 1 + nv);
//			cut_int(index, 3 * nv - 1, id2);
//			--nv;
//		}
//	}

	//TODO: signum == 0

	//Теперь нужно удалить вершины id1 , ... , id2
//	if(id1 == 0)
//	{
//		index[nv] = index[1];
//	}

//	printf("intr : v0_new == %d, v1_new = %d\n", v0_new, v1_new);
//	printf("id1 == %d, id2 = %d\n", id1, id2);
//	int id1_old = id1;
//	for(i = id1_old; i <= id2; ++i)
//	{
//		//В пересекающемся с плоскостью ребре одну из вершин заменяем точкой, по которой
//		//ребро пересекает плоскость
//		if(i == id1_old && v0 != v1)
//		{
//			index[id1] = new_vertex;
//			++id1;
//			continue;
//		}
//		if(i == id2 && v0_new != v1_new)
//		{
//			index[id1] = len_vertex;
//			++len_vertex;

//			printf("++len_vertex = %d\n", len_vertex);

//			continue;
//		}
//		cut_int(index, 3 * nv + 1, id1 + 1 + 2 * nv);
//		cut_int(index, 3 * nv, id1 + 1 + nv);
//		cut_int(index, 3 * nv - 1, id1);
//		--nv;
//	}

