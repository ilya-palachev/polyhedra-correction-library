#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Polyhedron.h"
#include "list_squares_method.h"

#include "array_operations.h"



void Polyhedron::join_facets(int facet_id0, int facet_id1)
{
	int i;
	int nv, *vertex_list, *edge_list, *vertex_cut, nvertex_cut;
	int join_nv, *join_index;
	int len_vertex;

	Plane plane;

	nv = this->join_facets_count_nv(facet_id0, facet_id1);
	if(nv < 0)
		return;

	//debug
	//print faces:
//	printf("main facet %d\n", facet_id0);
//	for(i = 0; i < facet[facet_id0].nv; ++i)
//	{
//		printf("\t-- %d %d: ",
//				 facet[facet_id0].index[i], facet[facet_id0].index[i + 1]);
//		int facet_id = facet[facet_id0].index[i + facet[facet_id0].nv + 1];
//		facet[facet_id].print_v(facet_id);
//	}
//	printf("main facet %d\n", facet_id1);
//	for(i = 0; i < facet[facet_id1].nv; ++i)
//	{
//		printf("\t-- %d %d: ",
//				 facet[facet_id1].index[i], facet[facet_id1].index[i + 1]);
//		int facet_id = facet[facet_id1].index[i + facet[facet_id1].nv + 1];
//		facet[facet_id].print_v(facet_id);
//	}
	//end debug

	vertex_list = new int[nv];
	edge_list = new int[nv];
	vertex_cut = new int[nv]; //TODO
	this->join_facets_create_vertex_list(facet_id0, facet_id1, nv, vertex_list, edge_list);
	this->list_squares_method(nv, vertex_list, &plane);
//	plane = - plane;
	nvertex_cut = this->join_facets_cut_vertex_list(nv, vertex_list, edge_list, vertex_cut);
	nv -= nvertex_cut;

	this->test_intersection_facet_create_edge_list(plane);

	//debug:
	printf("signum:\n");
	for(int i = 0; i < nv; ++i)
	{
		int sign0 = signum(vertex_list[i], plane);
		int i_next = i < nv - 1 ? i + 1 : 0;
		int sign1 = signum(vertex_list[i_next], plane);
		printf("\tsign(%d) = %d (facet %d)\n", vertex_list[i], sign0, edge_list[i]);
		if(sign0 != sign1)
		{
			Vector3d intrsct = intersection_edge(i, i_next, plane);
			printf("\t(%lf, %lf, %lf)\n", intrsct.x, intrsct.y, intrsct.z);
		}
	}
	//end debug

	//debug
//	int facet_id_new, v0_new, v1_new;
//	intersection_facet_step(plane, 15, 202, 171, facet_id_new, v0_new, v1_new);
//	printf("facet_id = 15, v0 = 202, v1 = 171 >>> \n");
//	printf("facet_id_new = %d, v0_new = %d, v1_new = %d\n",
//			 facet_id_new, v0_new, v1_new);
//	intersection_facet_step(plane, 37, 275, 172, facet_id_new, v0_new, v1_new);
//	printf("facet_id = 37, v0 = 275, v1 = 172 >>> \n");
//	printf("facet_id_new = %d, v0_new = %d, v1_new = %d\n",
//			 facet_id_new, v0_new, v1_new);
	//end debug

//debug:
//	printf("joined facet: nv = %d\n", nv);
//	for(i = 0; i < nv; ++i)
//		printf("%d ", vertex_list[i]);
//	printf("\n");
//	for(i = 0; i < nv; ++i)
//		printf("%d ", edge_list[i]);
//	printf("\n");

	join_index = new int[3 * numv]; //Большой массив
	join_nv = 0;
	len_vertex = numv; //Количество новых точек

	join_facets_cycle(join_index,
							join_nv,
							len_vertex,
							nv,
							vertex_list,
							edge_list,
							plane,
							facet_id0,
							facet_id1);

	printf("join_facets : cycle ended\n");

	Facet *facet0 = new Facet[numf + 1];
	for(i = 0; i < numf; ++i)
		facet0[i] = facet[i];
	//if(facet != NULL) delete[] facet;
	facet = facet0;
	facet[numf] = Facet(join_index, join_nv, plane);

	facet[numf].print_v(numf);

	facet[numf].rgb[0] = 255;
	facet[numf].rgb[1] = 0;
	facet[numf].rgb[2] = 0;

	facet[facet_id0] = facet[numf];
	facet[facet_id1] = facet[numf];

	++numf;
	numv = len_vertex;

	printf("before delete\n");
	if(vertex_list != NULL) delete[] vertex_list;
	if(edge_list != NULL) delete[] edge_list;
	if(vertex_cut != NULL) delete[] vertex_cut;
}

void Polyhedron::join_facets_cycle(
		int* join_index,
		int& join_nv,
		int& len_vertex,
		int nv,
		int* vertex_list,
		int* edge_list,
		Plane plane,
		int facet_id0,
		int facet_id1
		)
{
	int i, j, k;
	int v_curr;
	int sign_curr;
	int *index_v;
	int nf, f_id_begin, f_id_end;
	int *index0, *index1, nv0, nv1, id0, id1;
	int f, pos, nv_;

	int f_id_curr, f_id_next;
	int v0_curr, v1_curr, v0_next, v1_next;
	int id2, id3, id, ida, idb;

	int *edge;
	int nintrsct, new_vertex, len_vertex_prev;


	Vector3d new_point;

	edge_list[0] = edge_list[0];

	Vector3d *vertex0 = new Vector3d[2 * numv]; //Удваиваем массив
	for(i = 0; i < numv; ++i)
		vertex0[i] = vertex[i];
	if(vertex != NULL) delete[] vertex;
	vertex = vertex0;

	for(i = 0; i < nv; )
	{
		v_curr = vertex_list[i];
		sign_curr = signum(v_curr, plane);
		printf("\n#######vertex %d sign_curr == %d#######\n", v_curr, sign_curr);
		switch(sign_curr)
		{
			/////////////////////////////////////////////////
			/////////// ВЕРШИНА ЛЕЖИТ НА ПЛОСКОСТИ //////////
			/////////////////////////////////////////////////
		case 0:
			join_index[join_nv] = v_curr;
			++join_nv;

			printf("\tadding %d\n", v_curr);
			++i;
			break;


			/////////////////////////////////////////////////
			/////////// ВЕРШИНА ЛЕЖИТ ВЫШЕ ПЛОСКОСТИ ////////
			/////////////////////////////////////////////////


		case 1:
		{
			f_id_begin = edge_list[i > 0 ? i - 1 : nv - 1];

			f_id_curr = f_id_begin;

			//Находим первое ребро пересечения
			edge = new int[2 * facet[f_id_curr].nv];
			nintrsct = intersection_facet_create_edge_list(f_id_curr, plane, edge);
			if(nintrsct == 0)
			{
				printf("join_facet_cycle : Warning : Deep case!\n");
				break;
			}
			id0 = find_vertex(f_id_curr, edge[0]);
			id1 = find_vertex(f_id_curr, edge[1]);
			id2 = find_vertex(f_id_curr, edge[2 * nintrsct - 2]);
			id3 = find_vertex(f_id_curr, edge[2 * nintrsct - 1]);

//			printf("id0 = %d, id1 = %d\n", id0, id1);
//			printf("id2 = %d, id3 = %d\n", id2, id3);

			id0 = min_int(id0, id1);
			id1 = id0;
			id2 = min_int(id2, id3);

			id = find_vertex(f_id_curr, v_curr);

//			printf("id = %d\n", id);
			ida = -1;
			idb = -1;
			while(id1 != id2)
			{
				id1 = id1 < facet[f_id_curr].nv - 1 ? id1 + 1 : 0;
				if(id1 == id)
				{
					ida = find_vertex(f_id_curr, edge[2 * nintrsct - 2]);
					idb = find_vertex(f_id_curr, edge[2 * nintrsct - 1]);
				}
			}
			if(ida == -1 && idb == -1)
			{
				ida = find_vertex(f_id_curr, edge[0]);
				idb = find_vertex(f_id_curr, edge[1]);
			}
//			printf("ida = %d, idb = %d\n", ida, idb);
			v0_curr = facet[f_id_curr].index[ida];
			v1_curr = facet[f_id_curr].index[idb];
			//Нашли первое ребро пересечения

			printf("\tFirst edge found : %d %d\n", v0_curr, v1_curr);

			if(signum(v0_curr, plane) == 0)
			{
				new_vertex = v0_curr;
			}

			else if(signum(v1_curr, plane) == 0)
			{
				new_vertex = v1_curr;
			}

			else
			{
				intersection(plane, vertex[v1_curr] - vertex[v0_curr], vertex[v0_curr],
								 new_point);
				vertex[len_vertex] = new_point;
				new_vertex = len_vertex;
				++len_vertex;
			}

			int j = i;

			while(f_id_curr != facet_id0 && f_id_curr != facet_id1)
			{
				printf("----------------------------------------\n");
				printf("f_id_curr = %d, v0_curr = %d, v1_curr = %d\n",
						 f_id_curr, v0_curr, v1_curr);
				facet[f_id_curr].rgb[0] = 0;
				facet[f_id_curr].rgb[1] = 10 * (j++);
				facet[f_id_curr].rgb[2] = 0;


				join_index[join_nv] = new_vertex;
				++join_nv;

				len_vertex_prev = len_vertex;

				//debug
				facet[f_id_curr].print_v(f_id_curr);

				intersection_facet_step_cut(
						plane,
						f_id_curr, v0_curr, v1_curr,
						f_id_next, v0_next, v1_next,
						new_vertex, len_vertex);
				//debug
				facet[f_id_curr].print_v(f_id_curr);

				if(len_vertex > len_vertex_prev)
				{
					intersection(plane, vertex[v1_curr] - vertex[v0_curr], vertex[v0_curr],
									 new_point);
					vertex[len_vertex_prev] = new_point;
					new_vertex = len_vertex - 1;
				}

				if(signum(v0_next, plane) == 0)
				{
					new_vertex = v0_next;
				}

				else if(signum(v1_next, plane) == 0)
				{
					new_vertex = v1_next;
				}

				f_id_curr = f_id_next;
				v0_curr = v0_next;
				v1_curr = v1_next;
			}

			//Пропуск срезанных вершин
			int sign;
			do
			{
				sign = signum(vertex_list[i], plane);
				printf("sign(%d) == %d\n", vertex_list[i], sign);
				++i;
			}
			while( sign == 1 );
			--i;
		}
			break;

			/////////////////////////////////////////////////
			/////////// ВЕРШИНА ЛЕЖИТ НИЖЕ ПЛОСКОСТИ ////////
			/////////////////////////////////////////////////

		case -1:
			index_v = vertexinfo[v_curr].index;
			nf = vertexinfo[v_curr].nf;
			//Поиск номеров изменяемых плоскостей
			//TODO: возможно это сделать через edge_list быстрее
			//TODO: нарушается структура vertexinfo
			for(j = 0; j < nf; ++j)
			{
				if(index_v[j] == facet_id0 || index_v[j] == facet_id1)
				{
					f_id_begin = index_v[j > 0 ? j - 1 : nf - 1];
					f_id_end = index_v[j + 1];
//					f_id_end = index_v[j > 0 ? j - 1 : nf - 1];
//					f_id_begin = index_v[j + 1];
					break;
				}
			}
			if(j == nf)
			{
				printf("join_facets : Error 1.\n");
			}
			printf("f_id_begin = %d, f_id_end = %d\n", f_id_begin, f_id_end);
			intersection(plane, facet[f_id_begin].plane, facet[f_id_end].plane, new_point);
//			if(0)
			if(nf == 3)
			{
				printf("\t--triple\n");
				vertex[v_curr] = new_point;
				join_index[join_nv] = v_curr;
				++join_nv;
				printf("\tadding %d\n", v_curr);

			}
			else
			{
				printf("\t--multiple\n");
				vertex[len_vertex] = new_point;
				join_index[join_nv] = len_vertex;
				++join_nv;
				printf("\tadding %d under %d\n", len_vertex, v_curr);

//				nv0 = facet[f_id_begin].nv;
//				index0 = new int[3 * nv0 + 4];
//				memcpy(index0, facet[f_id_begin].index, (3 * nv0 + 1) * sizeof(int));
//				id0 = find_vertex(f_id_begin, v_curr);

//				nv1 = facet[f_id_end].nv;
//				index1 = new int[3 * nv1 + 4];
//				memcpy(index1, facet[f_id_end].index, (3 * nv1 + 1) * sizeof(int));
//				id1 = find_vertex(f_id_end, v_curr);

				nv0 = facet[f_id_end].nv;
				index0 = new int[3 * nv0 + 4];
				memcpy(index0, facet[f_id_end].index, (3 * nv0 + 1) * sizeof(int));
				id0 = find_vertex(f_id_end, v_curr);

				nv1 = facet[f_id_begin].nv;
				index1 = new int[3 * nv1 + 4];
				memcpy(index1, facet[f_id_begin].index, (3 * nv1 + 1) * sizeof(int));
				id1 = find_vertex(f_id_begin, v_curr);

				//Вставляем новую вершину в грань f_id_end
				printf("inserting %d (pos %d) after %d (pos %d) at facet %d\n",
						 len_vertex,
						 id0 + 1,
						 v_curr,
						 id0,
						 f_id_end);
				insert_int(index0,
							  3 * nv0 + 1,
							  id0 + 1 + 1 + 2 * nv0,
							  id1);
				insert_int(index0,
							  3 * nv0 + 2,
							  id0 + 1 + 1 + nv0,
							  f_id_begin);
				insert_int(index0,
							  3 * nv0 + 3,
							  id0 + 1,
							  len_vertex);
				++nv0;

				//Сообщаем о вставке соседним граням
				for(k = id0 + 2; k < nv0; ++k)
				{
					f = index0[k + 1 + nv0];
					pos = index0[k + 1 + 2 * nv0];
					nv_ = facet[f].nv;
					++(facet[f].index[pos + 1 + 2 * nv_]);
				}

				//Вставляем новую вершину в грань f_id_begin
				printf("inserting %d (pos %d) before %d (pos %d) at facet %d\n",
						 len_vertex,
						 (id1 > 0 ? id1 - 1 : nv1 - 1),
						 v_curr,
						 id1,
						 f_id_begin);
				insert_int(index1,
							  3 * nv1 + 1,
							  id1 + 1 + 2 * nv1,
							  id0 + 1);
				insert_int(index1,
							  3 * nv1 + 2,
							  id1 + 1 + nv1,
							  f_id_end);
				insert_int(index1,
							  3 * nv1 + 3,
							  id1,
							  len_vertex);
				++nv1;

				//Сообщаем о вставке соседним граням
				for(k = id1; k < nv1; ++k)
				{
					f = index1[k + 1 + nv1];
					pos = index1[k + 1 + 2 * nv1];
					nv_ = facet[f].nv;
					++(facet[f].index[pos + 1 + 2 * nv_]);
				}


				facet[f_id_end].nv = nv0;
				facet[f_id_end].index = index0;
				facet[f_id_begin].nv = nv1;
				facet[f_id_begin].index = index1;

				facet[f_id_end].print_v(f_id_end);
				facet[f_id_begin].print_v(f_id_begin);


				++len_vertex;
			}
			++i;

			break;
		default:
			break;
		}
	}
}

int Polyhedron::join_facets_count_nv(int facet_id0, int facet_id1)
{
	int nv0, nv1, nv_common, *index0, *index1, i, j, nv;

	if(facet_id0 == facet_id1)
	{
		fprintf(stderr, "join_facets_...: Error. facet_id0 == facet_id1.\n");
		return 0;
	}

	nv0 = facet[facet_id0].nv;
	nv1 = facet[facet_id1].nv;
	index0 = facet[facet_id0].index;
	index1 = facet[facet_id1].index;

	for(i = 0; i < nv0; ++i)
	{
		if(index0[nv0 + 1 + i] == facet_id1)
			break;
	}
	if(i == nv0)
	{
		fprintf(stderr, "join_facets_...: Error. No common vertices.\n");
		return 0;
	}

	//Подсчет количества общих вершин
	nv_common = 0;
	for(j = 0; j < nv0; i = (i < nv0 - 1 ? i + 1 : 0), ++j)
	{
		++nv_common;
		if(index0[nv0 + 1 + i] != facet_id1)
			break;
	}

	nv = nv0 + nv1 - nv_common;
	return nv;
}

void Polyhedron::join_facets_create_vertex_list(int facet_id0, int facet_id1, int nv,
																int *vertex_list, int* edge_list)
{
	int nv0, nv1, *index0, *index1, i, j, k;

	nv0 = facet[facet_id0].nv;
	nv1 = facet[facet_id1].nv;
	index0 = facet[facet_id0].index;
	index1 = facet[facet_id1].index;

	for(i = 0; i < nv0; ++i)
	{
		if(index0[nv0 + 1 + i] == facet_id1)
			break;
	}

	for(j = 0; j < nv0; i = (i < nv0 - 1 ? i + 1 : 0), ++j)
	{
		if(index0[nv0 + 1 + i] != facet_id1)
			break;
	}

	//Сбор вершин нулевой грани
	for(j = 0; j < nv0; i = (i < nv0 - 1 ? i + 1 : 0), ++j)
	{
		vertex_list[j] = index0[i];
		if(index0[nv0 + 1 + i] == facet_id1)
			break;
		edge_list[j] = index0[nv0 + 1 + i];
	}
	//Утверждение. index0[nv0 + 1 + i] == facet_id1
	i = index0[2*nv0 + 1 + i];
	//Сбор вершин первой грани
	for(k = 0; k < nv1; i = (i < nv1 - 1 ? i + 1 : 0), ++k, ++j)
	{
		if(index1[nv1 + 1 + i] == facet_id0)
			break;
		vertex_list[j] = index1[i];
		edge_list[j] = index1[nv1 + 1 + i];
	}

	nv = nv;
	//debug
//	printf("join_facets_create_*vertex_list:\n");
//	printf("facet %d: nv = %d\n\t", facet_id0, nv0);
//	for(i = 0; i < nv0; ++i)
//		printf("%d ", index0[i]);
//	printf("\n");
//	printf("facet %d: nv = %d\n\t", facet_id1, nv1);
//	for(i = 0; i < nv1; ++i)
//		printf("%d ", index1[i]);
//	printf("\n");
//	printf("joined facet: nv = %d\n", nv);
//	for(i = 0; i < nv; ++i)
//		printf("%d ", vertex_list[i]);
//	printf("\n");
//	for(i = 0; i < nv; ++i)
//		printf("%d ", edge_list[i]);
//	printf("\n");
}


void Polyhedron::list_squares_method(int nv, int *vertex_list, Plane *plane)
{
	int i, id;
	double *x, *y, *z, a, b, c, d;

	x = new double[nv];
	y = new double[nv];
	z = new double[nv];

	for(i = 0; i < nv; ++i)
	{
		id = vertex_list[i];
		x[i] = vertex[id].x;
		y[i] = vertex[id].y;
		z[i] = vertex[id].z;
	}

	aprox(nv, x, y, z, a, b, c, d);

	plane->norm.x = a;
	plane->norm.y = b;
	plane->norm.z = c;
	plane->dist = d;

	//debug
	//printf("list_squares_method: a = %lf, b = %lf, c = %lf, d = %lf\n", a, b, c, d);

	if(x != NULL) delete[] x;
	if(y != NULL) delete[] y;
	if(z != NULL) delete[] z;
}

int Polyhedron::join_facets_cut_vertex_list(int nv, int *vertex_list, int *edge_list,
															int *vertex_cut)
{
	int i, nvertex_cut, i_next;

	int facet_id, nv_, *index, id;

	nvertex_cut = 0;
	for(i = 0; i < nv; ++i)
	{
		i_next = i < nv - nvertex_cut - 1 ? i + 1 : 0;
		if(edge_list[i] == edge_list[i_next])
		{
//			printf("cut: detected same facet %d at positions %d and %d\n",
//					 edge_list[i], i, i_next);
			//Сначала удаляем вершину из соседней грани
			facet_id = edge_list[i];
			nv_ = facet[facet_id].nv;
			index = facet[facet_id].index;
			id = find_vertex(facet_id, vertex_list[i_next]);
			if(id == 0)
			{
				index[nv_] = index[1];
			}
			cut_int(index, 3 * nv_ + 1, id + 1 + 2 * nv_);
			cut_int(index, 3 * nv_, id + 1 + nv_);
			cut_int(index, 3 * nv_ - 1, id);
			facet[facet_id].nv = facet[facet_id].nv - 1;

			//Затем удаляем ее из списков
			vertex_cut[nvertex_cut] = vertex_list[i_next];
			cut_int(vertex_list, nv - nvertex_cut, i_next);
			cut_int(edge_list, nv - nvertex_cut, i_next);
			++nvertex_cut;
		}
	}
	return nvertex_cut;
}



