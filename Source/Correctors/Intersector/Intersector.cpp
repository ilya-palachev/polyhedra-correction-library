/*
 * Intersector.cpp
 *
 *  Created on: 18.06.2013
 *      Author: ilya
 */

#include "PolyhedraCorrectionLibrary.h"

Intersector::Intersector() :
		PCorrector()
{
	DEBUG_START;
	DEBUG_END;
}

Intersector::Intersector(Polyhedron* input) :
		PCorrector(input)
{
	DEBUG_START;
	DEBUG_END;
}

Intersector::~Intersector()
{
	DEBUG_START;
	DEBUG_END;
}

void my_fprint_Vector3d(Vector3d& v, FILE* file)
{
	DEBUG_START;
	REGULAR_PRINT(file, "(%lf, %lf, %lf)\n", v.x, v.y, v.z);
	DEBUG_END;
}

bool Intersector::intersectFacet(Facet* facet, Plane iplane, FutureFacet& ff,
		int& n_components)
{
	DEBUG_START;
	FacetIntersector* facetIntersector = new FacetIntersector(this, facet);
	bool ret = facetIntersector->run(iplane, ff, n_components);
	delete facetIntersector;
	DEBUG_END;
	return ret;
}

static void min3scalar(double s, double& s0, double& s1, double &s2, int i, int& i0,
		int& i1, int& i2)
{
	DEBUG_START;

	if (i0 == -1)
	{
		s0 = s;
		i0 = i;
		DEBUG_END;
		return;
	}
	if (i1 == -1)
	{
		if (s < s0)
		{
			s1 = s0;
			i1 = i0;
			s0 = s;
			i0 = i;
		}
		else
		{
			s1 = s;
			i1 = i;
		}
		DEBUG_END;
		return;
	}
	if (i2 == -1)
	{
		if (s < s0)
		{
			s2 = s1;
			i2 = i1;
			s1 = s0;
			i1 = i0;
			s0 = s;
			i0 = i;
		}
		else if (s < s1)
		{
			s2 = s1;
			i2 = i1;
			s1 = s;
			i1 = i;
		}
		else
		{
			s2 = s;
			i2 = i;
		}
		DEBUG_END;
		return;
	}
	if (s < s0)
	{
		s2 = s1;
		i2 = i1;
		s1 = s0;
		i1 = i0;
		s0 = s;
		i0 = i;
	}
	else if (s < s1)
	{
		s2 = s1;
		i2 = i1;
		s1 = s;
		i1 = i;
	}
	else if (s < s2)
	{
		s2 = s;
		i2 = i;
	}
	DEBUG_END;
}

static int sign(int i0, int i1, int i2)
{
	DEBUG_START;
	bool num_inv = (i0 > i1) + (i0 > i2) + (i1 > i2);
	DEBUG_END;
	return 1 - 2 * num_inv;
}

int Intersector::prepareEdgeList(Facet* facet, Plane iplane)
{
	DEBUG_START;
	int n_intrsct;
	int i, i_next, i_prev;
	int sign_curr, sign_next, sign_prev;
	double scalar;

	int i0, i1, i2;
	double s0, s1, s2;

	int i_step, i_curr, i_help;
	int up_down, in_out;
	int next_d, next_f;
	int n_positive;

	Vector3d dir;
	Vector3d point;
	Vector3d v_curr;
	Vector3d v_next;
	Vector3d v_intrsct;

	EdgeList* el = &(edgeLists[facet->id]);

	DEBUG_PRINT("facet[%d].prepare_edge_list ", facet->id);

	// 1. Вычисляем напр вектор пересечения грани с плоскостью
	bool if_intersect = intersection(iplane, facet->plane, dir, point);
	if (!if_intersect)
	{
//        DEBUG_PRINT("===Facet::prepare_edge_list : cannot prepare list for facet %d because planes are parallel or equal...", id);
		// Написано исключительно для программы слияния хорошо сопрягающихся граней.
		// Здесь предполагается, что в такой грани может быть нарушена плоскостность,
		// то есть вершины могут немного не лежать в плоскости грани.
		// В грани могут быть только отрицательные и нулевые вершины (этого мы добиваемся
		// в процессе поднятия вершин - см. функцию Polyhedron::join).

		DEBUG_PRINT("Facet::prepare_edge_list [%d]: special case", facet->id);

		for (i = 0; i < facet->numVertices; ++i)
		{
			i_next = (i + 1) % facet->numVertices;
			i_prev = (facet->numVertices + i - 1) % facet->numVertices;
			sign_curr = facet->signum(i, iplane);
			sign_next = facet->signum(i_next, iplane);
			sign_prev = facet->signum(i_prev, iplane);
			DEBUG_PRINT(" %d", sign_curr);

			if (sign_curr == 0 && sign_next == -1)
			{
				el->add_edge(facet->indVertices[i], facet->indVertices[i], i,
						i, facet->indVertices[facet->numVertices + 1 + i], 1,
						(double) i);
			}
			else if (sign_curr == 0 && sign_next == 0)
			{
				el->add_edge(facet->indVertices[i], facet->indVertices[i], i,
						i, facet->id, 1, (double) i);
			}

		}
		DEBUG_PRINT("");

		DEBUG_END;
		return 0;
	}

	// 2. Строим упорядоченный список вершин и ребер, пересекающихся с
	//    плоскостью
	n_intrsct = 0;
	n_positive = 0;
	i0 = i1 = i2 = -1;
	for (i = 0; i < facet->numVertices; ++i)
	{

		i_next = (i + 1) % facet->numVertices;
		i_prev = (facet->numVertices + i - 1) % facet->numVertices;
		sign_curr = facet->signum(i, iplane);
		sign_next = facet->signum(i_next, iplane);
		sign_prev = facet->signum(i_prev, iplane);
		n_positive += sign_curr == 1;

		//Если вершина лежит на плоскости
		if (sign_curr == 0)
		{
			//Отбрасываем висячие вершины:
			if (sign_prev * sign_next == 1)
				continue;

			v_intrsct = polyhedron->vertices[facet->indVertices[i]];
			scalar = dir * (v_intrsct - point);
			min3scalar(scalar, s0, s1, s2, i, i0, i1, i2);
			el->add_edge(facet->indVertices[i], facet->indVertices[i], i, i,
					scalar);
			++n_intrsct;
		}

		//Если ребро пересекает плоскость
		if (sign_curr * sign_next == -1)
		{
			v_curr = polyhedron->vertices[facet->indVertices[i]];
			v_next = polyhedron->vertices[facet->indVertices[i_next]];
			intersection(iplane, v_next - v_curr, v_curr, v_intrsct);
			scalar = dir * (v_intrsct - point);
			min3scalar(scalar, s0, s1, s2, i, i0, i1, i2);
			el->add_edge(facet->indVertices[i], facet->indVertices[i_next],
					i, i_next, scalar);
			++n_intrsct;
		}
	}
#ifdef DEBUG
	//	this->my_fprint_all(stdout);
	//	DEBUG_PRINT(
	//			"i0 = %d, s0 = %.2lf, i1 = %d, s1 = %.2lf, i2 = %d, s2 = %.2lf",
	//			i0, s0, i1, s1, i2, s2);
#endif

	//Утверждение. n_intrsct == 0 || n_intrsct > 1 , т. к. первый цикл отбрасывает
	//висячие вершины
	if (n_intrsct == 0)
	{
		DEBUG_END;
		return 0;
	}

	if (n_intrsct == 2)
	{
		DEBUG_PRINT("------For facet %d i_step = 1", facet->id);
		i_step = 1;
	}
	else
	{
		//Утверждение. i0 != -1 && i1 != -1 && i2 != -1
		i_step = sign(i0, i1, i2);
		// Проверка, что начальным выбран задний конец ребра
		sign_curr = facet->signum(i0, iplane);
		if (i_step == -1 && sign_curr != 0)
			i0 = (i0 + 1) % facet->numVertices;
	}

	i_curr = i0;
	i_next = (i0 + i_step + facet->numVertices) % facet->numVertices;
	i_prev = (i0 - i_step + facet->numVertices) % facet->numVertices;

	sign_prev = facet->signum(i_prev, iplane);
	up_down = sign_prev;
	in_out = -1;

	el->goto_header();

	for (i = 0; i < facet->numVertices; ++i)
	{

		sign_curr = facet->signum(i_curr, iplane);
		sign_next = facet->signum(i_next, iplane);

		if (sign_curr == 0 || sign_curr * sign_next == -1)
		{
			if (sign_next == -up_down || (sign_curr == 0 && in_out == -1)
					|| (sign_curr * sign_next == -1) // 2011-04-04
					)
			{
				in_out *= -1;
				up_down *= -1;
				next_d = in_out;
				if (sign_curr == 0)
					next_f =
							polyhedron->
							vertexInfos[facet->
							            indVertices[i_curr]].
							            intersection_find_next_facet(
									iplane, facet->id);
				else
				{
					i_help = i_step == 1 ? i_curr : i_next;
					next_f = facet->indVertices[i_help + facet->numVertices + 1];
				}
			}
			else
			{
				DEBUG_PRINT("-----For facet %d we have critical situation...",
						facet->id);
				break;
			}
			if (sign_curr * sign_next == -1)
				el->search_and_set_info(facet->indVertices[i_curr],
						facet->indVertices[i_next], next_d, next_f);
			else
				el->search_and_set_info(facet->indVertices[i_curr],
						facet->indVertices[i_curr], next_d, next_f);
		}

		i_curr = i_next;
		i_next = (i_next + i_step + facet->numVertices) % facet->numVertices;
	}

	i_step *= -1;
	i_curr = i0;
	i_next = (i0 + i_step + facet->numVertices) % facet->numVertices;
	i_prev = (i0 - i_step + facet->numVertices) % facet->numVertices;

	sign_prev = facet->signum(i_prev, iplane);
	up_down = sign_prev;
	in_out = -1;

	el->goto_header();

	for (i = 0; i < facet->numVertices; ++i)
	{

		sign_curr = facet->signum(i_curr, iplane);
		sign_next = facet->signum(i_next, iplane);

		if (sign_curr == 0 || sign_curr * sign_next == -1)
		{
			if (sign_next == -up_down || (sign_curr == 0 && in_out == -1)
					|| (sign_curr * sign_next == -1) // 2011-04-04
					)
			{
				in_out *= -1;
				up_down *= -1;
				next_d = in_out;
				if (sign_curr == 0)
					next_f =
							polyhedron->
							vertexInfos[facet->
							            indVertices[i_curr]].
							            intersection_find_next_facet(
									iplane, facet->id);
				else
				{
					i_help = i_step == 1 ? i_curr : i_next;
					next_f = facet->indVertices[i_help + facet->numVertices + 1];
				}
			}
			else
			{
				DEBUG_PRINT(
						"-----For facet %d we have critical situation AGAIN!!!...",
						facet->id);
				next_d = 0;
				next_f = facet->id;
			}
			if (sign_curr * sign_next == -1)
				el->search_and_set_info(facet->indVertices[i_curr],
						facet->indVertices[i_next], next_d, next_f);
			else
				el->search_and_set_info(facet->indVertices[i_curr],
						facet->indVertices[i_curr], next_d, next_f);
		}

		i_curr = i_next;
		i_next = (i_next + i_step + facet->numVertices) % facet->numVertices;
	}

	DEBUG_END;
	return n_intrsct;
}

void Intersector::run(Plane iplane)
{
	DEBUG_START;

	int i, j, k, j_begin;
	int nume;
	int res, total_edges;
	int* num_edges;
	int* lenff;
	int num_components_new;
	int id_v_new;
	int v0;
	int v1;
	int fid, fid_curr, fid_next;
	int drctn;
	int v0_first;
	int v1_first;

	int v0_prev;
	int v1_prev;

	int num_components_local;
	int num_components_old;
	int len_comp;

	int num_saved_facets;
	bool* ifSaveFacet;

	int numf_new;
	int numf_res;

	int numv_new;
	int numv_res;

	Vector3d vec0, vec1, vec;

	DEBUG_PRINT(
			"======================================================");
	DEBUG_PRINT("Intersection the polyhedron by plane : ");
	DEBUG_PRINT("(%lf) * x + (%lf) * y + (%lf) * z + (%lf) = 0",
			iplane.norm.x, iplane.norm.y, iplane.norm.z, iplane.dist);

	num_edges = new int[polyhedron->numFacets];
	lenff = new int[polyhedron->numFacets];
	edgeLists = new EdgeList[polyhedron->numFacets];

	nume = 0;
	for (i = 0; i < polyhedron->numFacets; ++i)
	{
		nume += polyhedron->facets[i].get_nv();
	}
	FutureFacet buffer_old(nume);
	nume /= 2;
	EdgeSetIntersected edge_set(nume);
	EdgeSetIntersected total_edge_set(nume);

	// 1. Подготовка списков ребер

	int n_0 = 0, n_2 = 0, n_big = 0;

	total_edges = 0;
	for (i = 0; i < polyhedron->numFacets; ++i)
	{
		edgeLists[i] = EdgeList(i, polyhedron->facets[i].get_nv(), polyhedron);
		res = prepareEdgeList(&(polyhedron->facets[i]), iplane);
		edgeLists[i].send(&total_edge_set);
		edgeLists[i].send_edges(&edge_set);
#ifndef NDEBUG
		//		if (edge_list[i].get_num() == 2)
		edgeLists[i].my_fprint(stdout);
		switch (edgeLists[i].get_num())
		{
			case 2:
				++n_2;
				break;
			case 0:
				++n_0;
				break;
			default:
				++n_big;
				break;
		}
#endif
		num_edges[i] = res;
		edgeLists[i].set_poly(polyhedron);
	}
#ifndef NDEBUG
	DEBUG_PRINT("n_2 = %d, n_0 = %d, n_big = %d", n_2, n_0, n_big);
#endif
	total_edges = total_edge_set.get_num();

	// 2. Нахождение компонент сечения
	FutureFacet buffer_new(nume);

	num_components_new = 0;
	bool flag = true;
	while (total_edges > 0 && flag)
	{

		for (i = 0; i < polyhedron->numFacets; ++i)
		{
			if (num_edges[i] > 0)
				break;
		}
		fid_curr = i;
		if (fid_curr == polyhedron->numFacets)
		{
			continue;
		}
		edgeLists[fid_curr].get_first_edge(v0, v1);
		fid_next = fid_curr;
		v0_first = v0;
		v1_first = v1;
		lenff[num_components_new] = 0;
		do
		{
			if (edgeLists[fid_next].get_num() < 1)
			{
				if (fid_next != fid_curr)
					drctn = 0;
			}

			fid_curr = fid_next;
			--total_edges;
			--num_edges[fid_curr];
			++lenff[num_components_new];
			if (v0 != v1)
			{
				edge_set.add_edge(v0, v1);
			}

			buffer_new.add_edge(v0, v1, fid);
			if (buffer_new.get_nv() >= nume)
			{
				ERROR_PRINT("Error. Stack overflow  in buffer_new");
				DEBUG_END;
				return;
			}

			polyhedron->facets[fid_curr].my_fprint_all(stdout);
			edgeLists[fid_curr].my_fprint(stdout);

			v0_prev = v0;
			v1_prev = v1;

			edgeLists[fid_curr].get_next_edge(iplane, v0, v1, fid_next, drctn);
			if ((v0_prev == v0 && v1_prev == v1)
					|| (v0_prev == v1 && v1_prev == v0))
			{
				DEBUG_PRINT("Endless loop!!!!!");
				flag = false;
				break;
			}
			if (fid_next != fid_curr)
				--num_edges[fid_curr];

			if (v0 == -1 || v1 == -1)
			{
				ERROR_PRINT("Warning. v0 = %d, v1 = %d", v0, v1);
				DEBUG_END;
				return;
			}

		} while (v0 != v0_first || v1 != v1_first);
		++num_components_new;
	}
	DEBUG_PRINT("====   SUPER-STEP 2 ENDED  =====");

	//3. Расщепление компонент сечения
	FutureFacet* future_facet_new;
	future_facet_new = new FutureFacet[num_components_new];
	for (i = 0, k = 0; i < num_components_new; ++i)
	{
		future_facet_new[i] = FutureFacet(lenff[i]);
		future_facet_new[i].set_id(i);
		for (j = 0; j < lenff[i]; ++j)
		{
			buffer_new.get_edge(k++, v0, v1, fid, id_v_new);
			future_facet_new[i].add_edge(v0, v1, fid);
		}
	}
	//	scanf("%d", &i);

#ifdef OUTPUT
	DEBUG_PRINT("%d new polyhedron->facets : ", num_components_new);
	for (i = 0; i < num_components_new; ++i)
	{
		future_facet_new[i].my_fprint(stdout);
	}
#endif

	//4. Нахождение компонент рассечения старых граней
	DEBUG_PRINT("4. Нахождение компонент рассечения старых граней");
	num_components_old = 0;
	num_saved_facets = 0;
	ifSaveFacet = new bool[polyhedron->numFacets];
	for (i = 0; i < polyhedron->numFacets; ++i)
	{
		DEBUG_PRINT("\tГрань %d", i);
		ifSaveFacet[i] = intersectFacet(&(polyhedron->facets[i]), iplane,
				buffer_old, num_components_local);
		if (!ifSaveFacet[i])
			DEBUG_PRINT("Facet %d is candidate for deleting", i);
		if (polyhedron->facets[i].numVertices < 3)
			ifSaveFacet[i] = false;  //2012-03-10
		num_saved_facets += ifSaveFacet[i];
		num_components_old += num_components_local;
	}

	//5. Расщепление компонент рассечения старых граней
	FutureFacet* future_facet_old;
	future_facet_old = new FutureFacet[num_components_old];

	j_begin = 0;
	int fid_prev = -1;

	bool* ifMultiComponent;
	ifMultiComponent = new bool[num_components_old];

	for (i = 0; i < num_components_old; ++i)
	{
		for (j = j_begin; j < buffer_old.get_nv(); ++j)
		{
			buffer_old.get_edge(j, v0, v1, fid, id_v_new);
			if (v0 == -1 && v1 == -1)
				break;
		}
		len_comp = j - j_begin;
		if (len_comp <= 0)
			break;
		future_facet_old[i] = FutureFacet(len_comp);
		future_facet_old[i].set_id(i);
		for (j = j_begin; j < j_begin + len_comp; ++j)
		{
			buffer_old.get_edge(j, v0, v1, fid, id_v_new);
			future_facet_old[i].add_edge(v0, v1, fid);
		}
		j_begin = j_begin + len_comp + 1;
		if (fid == fid_prev && i > 0)
		{
			ifMultiComponent[i] = true;
			ifMultiComponent[i - 1] = true;
		}
		else
		{
			ifMultiComponent[i] = false;
		}
		fid_prev = fid;
	}

#ifdef OUTPUT
	DEBUG_PRINT("ifSaveFacet : ");
	for (i = 0; i < polyhedron->numFacets; ++i)
		if (!ifSaveFacet[i])
			DEBUG_PRINT("Facet %d is deleted", i);
	DEBUG_PRINT("");
#endif

	//6. Генерирование новых граней
	Facet* facet_new;
	numf_new = num_components_new + num_components_old;
	facet_new = new Facet[numf_new];
	for (i = 0; i < num_components_new; ++i)
	{
		future_facet_new[i].generate_facet(facet_new[i], polyhedron->numFacets + i, iplane,
				polyhedron->numVertices, &edge_set);
		facet_new[i].set_poly(polyhedron);
		facet_new[i].set_rgb(255, 0, 0);
	}
	for (i = 0; i < num_components_old; ++i)
	{
		future_facet_old[i].generate_facet(facet_new[i + num_components_new],
				polyhedron->numFacets + num_components_new + i, iplane, //TODO. Это неверно!!!
				polyhedron->numVertices, &edge_set);
		facet_new[i + num_components_new].set_poly(polyhedron);
		if (ifMultiComponent[i])
		{
			facet_new[i + num_components_new].set_rgb(0, 255, 0);
			DEBUG_PRINT("Multi-component: %d", i);
		}
		else
			facet_new[i + num_components_new].set_rgb(100, 0, 0);
	}

#ifdef OUTPUT
	DEBUG_PRINT("%d generated polyhedron->facets : ",
			num_components_new + num_components_old);
	for (i = 0; i < numf_new; ++i)
		facet_new[i].my_fprint(stdout);
#endif

	//7. Создание массива граней нового многогранника. Старые грани удаляются
	Facet* facet_res;

	numf_res = num_saved_facets + numf_new;
	DEBUG_PRINT("numf_res = %d = %d + %d = num_saved_facets + numf_new", numf_res,
			num_saved_facets, numf_new);
	facet_res = new Facet[numf_res];

	j = 0;
	for (i = 0; i < polyhedron->numFacets; ++i)
	{
		if (ifSaveFacet[i])
		{
			facet_res[j] = polyhedron->facets[i];
			facet_res[j].set_id(j);
			++j;
		}
	}
	for (i = 0; i < numf_new; ++i)
	{
		facet_res[j] = facet_new[i];
		facet_res[j].set_id(j);
		++j;
	}

#ifdef OUTPUT
	DEBUG_PRINT("%d resulting polyhedron->facets : ", numf_res);
	for (i = 0; i < numf_res; ++i)
		facet_res[i].my_fprint(stdout);
#endif

	//8. Создание массива новых вершин. Отрицательные вершины удаляются
	numv_new = edge_set.get_num();
	Vector3d* vertex_new;
	vertex_new = new Vector3d[numv_new];
	for (i = 0; i < numv_new; ++i)
	{
		edge_set.get_edge(i, v0, v1);
		vec0 = polyhedron->vertices[v0];
		vec1 = polyhedron->vertices[v1];
		intersection(iplane, vec1 - vec0, vec0, vec);
		vertex_new[i] = vec;
	}

#ifdef OUTPUT
	DEBUG_PRINT("%d new vertexes: ", numv_new);
	for (i = 0; i < numv_new; ++i)
		my_fprint_Vector3d(vertex_new[i], stdout);
#endif

	numv_res = 0;
	for (i = 0; i < polyhedron->numVertices; ++i)
	{
		numv_res += polyhedron->signum(polyhedron->vertices[i], iplane) >= 0;
	}
	numv_res += numv_new;
	Vector3d* vertex_res;
	vertex_res = new Vector3d[numv_res];

	int sign;
	j = 0;
	for (i = 0; i < polyhedron->numVertices; ++i)
	{
		sign = polyhedron->signum(polyhedron->vertices[i], iplane);
		if (sign >= 0)
		{
			vertex_res[j] = polyhedron->vertices[i];
			for (k = 0; k < numf_res; ++k)
				facet_res[k].find_and_replace_vertex(i, j);
			++j;
		}
	}
	for (i = 0; i < numv_new; ++i)
	{
		vertex_res[j] = vertex_new[i];
		for (k = 0; k < numf_res; ++k)
			facet_res[k].find_and_replace_vertex(polyhedron->numVertices + i, j);
		++j;
	}

	polyhedron->numVertices = numv_res;
	polyhedron->numFacets = numf_res;
	if (polyhedron->vertices != NULL)
		delete[] polyhedron->vertices;
	if (polyhedron->facets != NULL)
		delete[] polyhedron->facets;
	polyhedron->vertices = vertex_res;
	polyhedron->facets = facet_res;

	for (i = 0; i < polyhedron->numFacets; ++i)
	{
		if (polyhedron->facets[i].numVertices < 1)
		{
			DEBUG_PRINT("===Facet %d is empty...", i);
			for (j = i; j < polyhedron->numFacets - 1; ++j)
				polyhedron->facets[j] = polyhedron->facets[j + 1];
			--polyhedron->numFacets;
		}
	}

	DEBUG_PRINT("After test there are %d polyhedron->facets:", polyhedron->numFacets);
	for (i = 0; i < polyhedron->numFacets; ++i)
		polyhedron->facets[i].my_fprint(stdout);

	if (num_edges != NULL)
		delete[] num_edges;
	if (lenff != NULL)
		delete[] lenff;
	if (future_facet_new != NULL)
		delete[] future_facet_new;
	if (future_facet_old != NULL)
		delete[] future_facet_old;
	if (ifSaveFacet != NULL)
		delete[] ifSaveFacet;
	if (facet_new != NULL)
		delete[] facet_new;
	if (ifMultiComponent != NULL)
		delete[] ifMultiComponent;

	DEBUG_END;

}

void Intersector::runCoalesceMode(Plane iplane, int jfid)
{
	DEBUG_START;

	int i, j, k, j_begin;
	int nume;
	int res, total_edges;
	int* num_edges;
	int* lenff;
	int num_components_new;
	int id_v_new;
	int v0;
	int v1;
	int fid, fid_curr, fid_next;
	int drctn;
	int v0_first;
	int v1_first;

	int v0_prev;
	int v1_prev;

	int num_components_local;
	int num_components_old;
	int len_comp;

	int num_saved_facets;
	bool* ifSaveFacet;

	int numf_new;
	int numf_res;

	int numv_new;
	int numv_res;

	Vector3d vec0, vec1, vec;
	Vector3d MC = polyhedron->facets[jfid].find_mass_centre();
	DEBUG_PRINT("MC[%d] = (%lf, %lf, %lf)", jfid, MC.x, MC.y, MC.z);
	Vector3d A0, A1, n, nn;
	double mc;

	n = polyhedron->facets[jfid].plane.norm;

	DEBUG_PRINT("\n======================================================\n");
	DEBUG_PRINT("Intersection the polyhedron by plane : \n");
	DEBUG_PRINT("(%lf) * x + (%lf) * y + (%lf) * z + (%lf) = 0\n",
			iplane.norm.x, iplane.norm.y, iplane.norm.z, iplane.dist);

	num_edges = new int[polyhedron->numFacets];
	lenff = new int[polyhedron->numFacets];
	edgeLists = new EdgeList[polyhedron->numFacets];

	polyhedron->delete_empty_facets();

	nume = 0;
	for (i = 0; i < polyhedron->numFacets; ++i)
	{
		nume += polyhedron->facets[i].get_nv();
	}
	FutureFacet buffer_old(nume);
	nume /= 2;
	EdgeSetIntersected edge_set(nume);
	EdgeSetIntersected total_edge_set(nume);

	// 1. Подготовка списков ребер

	total_edges = 0;
	for (i = 0; i < polyhedron->numFacets; ++i)
	{
//        if (i == jfid)
//            continue;
		edgeLists[i] = EdgeList(i, polyhedron->facets[i].get_nv(), polyhedron);
		res = prepareEdgeList(&(polyhedron->facets[i]), iplane);
		edgeLists[i].send(&total_edge_set);
		edgeLists[i].send_edges(&edge_set);
		num_edges[i] = res;
		edgeLists[i].set_poly(polyhedron);
	}
	total_edges = total_edge_set.get_num();
//    total_edges += facet[jfid].nv;//2012-03-10

	edgeLists[jfid].my_fprint(stdout);

	// 2. Нахождение компонент сечения
	FutureFacet buffer_new(nume);

	num_components_new = 0;
	do
	{
		DEBUG_PRINT("total_edges = %d\n", total_edges);

		for (i = 0; i < polyhedron->numFacets; ++i)
		{
			if (num_edges[i] > 0 && i != jfid)
				break;
		}
		fid_curr = i;
		if (fid_curr == polyhedron->numFacets)
		{
			continue;
		}
		edgeLists[fid_curr].get_first_edge(v0, v1);
		v0_first = v0;
		v1_first = v1;
		fid_next = fid_curr;
		edgeLists[fid_curr].get_next_edge(iplane, v0, v1, fid_next, drctn);
		DEBUG_PRINT(
				"Firstly fid_curr = %d, v0_first = %d, v1_first = %d, fid_next = %d, v0 = %d, v1 = %d\n",
				fid_curr, v0_first, v1_first, fid_next, v0, v1);

		if (v0_first == v1_first)
		{
			A0 = polyhedron->vertices[v0_first];
		}
		else
		{
			if (fabs(iplane % polyhedron->vertices[v0_first])
					> fabs(iplane % polyhedron->vertices[v1_first]))
			{
				A0 = polyhedron->vertices[v1_first];
			}
			else
			{
				A0 = polyhedron->vertices[v0_first];
			}
		}
		if (v0 == v1)
		{
			A1 = polyhedron->vertices[v0];
		}
		else
		{
			if (fabs(iplane % polyhedron->vertices[v0]) > fabs(iplane % polyhedron->vertices[v1]))
			{
				A1 = polyhedron->vertices[v1];
			}
			else
			{
				A1 = polyhedron->vertices[v0];
			}
		}
		nn = ((A0 - MC) % (A1 - MC));
		DEBUG_PRINT("nn = (%lf, %lf, %lf)\n", nn.x, nn.y, nn.z);
		DEBUG_PRINT("n = (%lf, %lf, %lf)\n", n.x, n.y, n.z);
		mc = nn * n;
		DEBUG_PRINT("mc = %lf\n", mc);
		if (mc < 0)
		{
			DEBUG_PRINT("Negative direction detected...\n");
			edgeLists[fid_curr].set_isUsed(v0, v1, false);
			edgeLists[fid_curr].set_isUsed(v0_first, v1_first, false);
			v0_first = v0;
			v1_first = v1;
			fid_next = fid_curr;
//            drctn = 1;

		}
//        if (drctn == -1) {
//            DEBUG_PRINT("Negative direction detected...\n");
//            v0_first = v0;
//            v1_first = v1;
//            drctn = 1;
//            edge_list[fid_curr].set_isUsed(v0, v1, false);
//        }
//
//        v0_first = v0;
//        v1_first = v1;
		DEBUG_PRINT("\t\t\tv0_first = %d, v1_first = %d\n", v0_first, v1_first);
		lenff[num_components_new] = 0;
		do
		{
			DEBUG_PRINT("\t\t\tfid_curr = %d, fid_next = %d, v0 = %d, v1 = %d\n",
					fid_curr, fid_next, v0, v1);
//            edge_list[fid_curr].my_fprint(stdout);
			//Начало написанного 2012-03-10
//            int i0, i1, nnv;
//            if (fid_curr == jfid) {
//
//                nnv = facet[jfid].nv;
//
//                i0 = facet[jfid].find_vertex(v0);
//                i1 = facet[jfid].find_vertex(v1);
//
//                if (i0 == -1 && i1 == -1) {
//                    ERROR_PRINT("Error. Cannot find neigher %d nor %d in main facet %d\n", v0, v1, jfid);
//                    return;
//                }
//
//                if (i0 == -1)
//                    i0 = i1;
//
//                i0 = (nnv + i0 + 1) % nnv;
//                if (signum(vertex[facet[jfid].index[i0]], iplane) == 0) {
//                    fid_next = jfid;
//                }
//            }
			//Конец написанного 2012-03-10
			if (edgeLists[fid_next].get_num() < 1)
			{
				if (fid_next != fid_curr)
					drctn = 0;
			}

			fid_curr = fid_next;
			--total_edges;
			--num_edges[fid_curr];
			++lenff[num_components_new];
			if (v0 != v1)
			{
				edge_set.add_edge(v0, v1);
			}
			buffer_new.add_edge(v0, v1, fid);
			if (buffer_new.get_nv() >= nume)
			{
				ERROR_PRINT("Error. Stack overflow  in buffer_new\n");
				return;
			}
			v0_prev = v0;
			v1_prev = v1;

			edgeLists[fid_curr].get_next_edge(iplane, v0, v1, fid_next, drctn);
			DEBUG_PRINT("drctn = %d\n", drctn);

			if ((v0_prev == v0 && v1_prev == v1)
					|| (v0_prev == v1 && v1_prev == v0))
			{
				ERROR_PRINT("Endless loop!!!!!\n");
				break;
			}

			if (fid_next != fid_curr)
				--num_edges[fid_curr];

			if (v0 == -1 || v1 == -1)
			{
				ERROR_PRINT("Warning. v0 = %d, v1 = %d\n", v0, v1);
				DEBUG_END;
				return;
			}

		} while (v0 != v0_first || v1 != v1_first);
		++num_components_new;
	} while (0);
	DEBUG_PRINT("====   SUPER-STEP 2 ENDED  =====");

	//3. Расщепление компонент сечения
	FutureFacet* future_facet_new;
	future_facet_new = new FutureFacet[num_components_new];
	for (i = 0, k = 0; i < num_components_new; ++i)
	{
		future_facet_new[i] = FutureFacet(lenff[i]);
		future_facet_new[i].set_id(i);
		for (j = 0; j < lenff[i]; ++j)
		{
			buffer_new.get_edge(k++, v0, v1, fid, id_v_new);
			future_facet_new[i].add_edge(v0, v1, fid);
		}
	}
	//	scanf("%d", &i);

#ifdef OUTPUT
	DEBUG_PRINT("%d new facets : \n", num_components_new);
	for (i = 0; i < num_components_new; ++i)
	{
		future_facet_new[i].my_fprint(stdout);
	}
#endif

	//4. Нахождение компонент рассечения старых граней
	DEBUG_PRINT("4. Нахождение компонент рассечения старых граней\n");
	num_components_old = 0;
	num_saved_facets = 0;
	ifSaveFacet = new bool[polyhedron->numFacets];
	for (i = 0; i < polyhedron->numFacets; ++i)
	{
		ifSaveFacet[i] = intersectFacet(&(polyhedron->facets[i]), iplane, buffer_old,
				num_components_local);
		DEBUG_PRINT("\tГрань %d", i);
		DEBUG_PRINT(" - %d компонент\n", num_components_local);
		if (num_components_local > 1)
			DEBUG_PRINT("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		if (!ifSaveFacet[i])
			DEBUG_PRINT("Facet %d is candidate for deleting\n", i);
		if (polyhedron->facets[i].numVertices < 3)
			ifSaveFacet[i] = false;  //2012-03-10
		num_saved_facets += ifSaveFacet[i];
		num_components_old += num_components_local;
	}

	//5. Расщепление компонент рассечения старых граней
	FutureFacet* future_facet_old;
	future_facet_old = new FutureFacet[num_components_old];

	j_begin = 0;
	int fid_prev = -1;

	bool* ifMultiComponent;
	ifMultiComponent = new bool[num_components_old];

	for (i = 0; i < num_components_old; ++i)
	{
		for (j = j_begin; j < buffer_old.get_nv(); ++j)
		{
			buffer_old.get_edge(j, v0, v1, fid, id_v_new);
			if (v0 == -1 && v1 == -1)
				break;
		}
		len_comp = j - j_begin;
		if (len_comp <= 0)
			break;
		future_facet_old[i] = FutureFacet(len_comp);
		future_facet_old[i].set_id(i);
		for (j = j_begin; j < j_begin + len_comp; ++j)
		{
			buffer_old.get_edge(j, v0, v1, fid, id_v_new);
			future_facet_old[i].add_edge(v0, v1, fid);
		}
		j_begin = j_begin + len_comp + 1;
		if (fid == fid_prev && i > 0)
		{
			ifMultiComponent[i] = true;
			ifMultiComponent[i - 1] = true;
		}
		else
		{
			ifMultiComponent[i] = false;
		}
		fid_prev = fid;
	}

#ifdef OUTPUT
	DEBUG_PRINT("ifSaveFacet :");
	for (i = 0; i < polyhedron->numFacets; ++i)
		if (!ifSaveFacet[i])
			DEBUG_PRINT("Facet %d is deleted", i);
#endif

	//6. Генерирование новых граней
	Facet* facet_new;
	numf_new = num_components_new + num_components_old;
	facet_new = new Facet[numf_new];
	for (i = 0; i < num_components_new; ++i)
	{
		future_facet_new[i].generate_facet(facet_new[i], polyhedron->numFacets + i, iplane,
				polyhedron->numVertices, &edge_set);
		facet_new[i].set_poly(polyhedron);
		facet_new[i].set_rgb(255, 0, 0);
	}
	for (i = 0; i < num_components_old; ++i)
	{
		future_facet_old[i].generate_facet(facet_new[i + num_components_new],
				polyhedron->numFacets + num_components_new + i, iplane, //TODO. Это неверно!!!
				polyhedron->numVertices, &edge_set);
		facet_new[i + num_components_new].set_poly(polyhedron);
		if (ifMultiComponent[i])
		{
			facet_new[i + num_components_new].set_rgb(0, 255, 0);
			DEBUG_PRINT("Multi-component: %d\n", i);
		}
		else
			facet_new[i + num_components_new].set_rgb(100, 0, 0);
	}

#ifdef OUTPUT
	DEBUG_PRINT("%d generated facets : \n",
			num_components_new + num_components_old);
	for (i = 0; i < numf_new; ++i)
		facet_new[i].my_fprint(stdout);
#endif

	//7. Создание массива граней нового многогранника. Старые грани удаляются
	Facet* facet_res;

	numf_res = num_saved_facets + numf_new;
	DEBUG_PRINT("numf_res = %d = %d + %d = num_saved_facets + numf_new", numf_res,
			num_saved_facets, numf_new);
	facet_res = new Facet[numf_res];

//    for (i = 0; i < numf; ++i) {
//        if (facet[i])
//    }

	j = 0;
	for (i = 0; i < polyhedron->numFacets; ++i)
	{
		if (ifSaveFacet[i])
		{
			facet_res[j] = polyhedron->facets[i];
			facet_res[j].set_id(j);
//            DEBUG_PRINT("Грани %d переприсвоен новый номер: %d\n", i, j);
			++j;
		}
	}
	for (i = 0; i < numf_new; ++i)
	{
		facet_res[j] = facet_new[i];
		facet_res[j].set_id(j);
		++j;
	}

#ifdef OUTPUT
	DEBUG_PRINT("\n\n%d resulting facets : \n", numf_res);
	for (i = 0; i < numf_res; ++i)
		facet_res[i].my_fprint(stdout);
#endif

	//8. Создание массива новых вершин. Отрицательные вершины удаляются
	numv_new = edge_set.get_num();
	Vector3d* vertex_new;
	vertex_new = new Vector3d[numv_new];
	for (i = 0; i < numv_new; ++i)
	{
		edge_set.get_edge(i, v0, v1);
		vec0 = polyhedron->vertices[v0];
		vec1 = polyhedron->vertices[v1];
		intersection(iplane, vec1 - vec0, vec0, vec);
		vertex_new[i] = vec;
	}

//#ifdef OUTPUT
//    DEBUG_PRINT("%d new vertexes: \n", numv_new);
//    for (i = 0; i < numv_new; ++i)
//        my_fprint_Vector3d(vertex_new[i], stdout);
//#endif

	numv_res = 0;
	for (i = 0; i < polyhedron->numVertices; ++i)
	{
		numv_res += polyhedron->signum(polyhedron->vertices[i], iplane) >= 0;
	}
	numv_res += numv_new;
	Vector3d* vertex_res;
	vertex_res = new Vector3d[numv_res];

	int sign;
	j = 0;
	for (i = 0; i < polyhedron->numVertices; ++i)
	{
		sign = polyhedron->signum(polyhedron->vertices[i], iplane);
		if (sign >= 0)
		{
			vertex_res[j] = polyhedron->vertices[i];
			for (k = 0; k < numf_res; ++k)
				facet_res[k].find_and_replace_vertex(i, j);
			++j;
		}
	}
	for (i = 0; i < numv_new; ++i)
	{
		vertex_res[j] = vertex_new[i];
		for (k = 0; k < numf_res; ++k)
			facet_res[k].find_and_replace_vertex(polyhedron->numVertices + i, j);
		++j;
	}

	polyhedron->numVertices = numv_res;
	polyhedron->numFacets = numf_res;
	if (polyhedron->vertices != NULL)
		delete[] polyhedron->vertices;
	if (polyhedron->facets != NULL)
		delete[] polyhedron->facets;
	polyhedron->vertices = vertex_res;
	polyhedron->facets = facet_res;

	polyhedron->delete_empty_facets();

	DEBUG_PRINT("After test there are %d facets:\n", polyhedron->numFacets);
	for (i = 0; i < polyhedron->numFacets; ++i)
		polyhedron->facets[i].my_fprint(stdout);

	if (num_edges != NULL)
		delete[] num_edges;
	if (lenff != NULL)
		delete[] lenff;
	if (future_facet_new != NULL)
		delete[] future_facet_new;
	if (future_facet_old != NULL)
		delete[] future_facet_old;
	if (ifSaveFacet != NULL)
		delete[] ifSaveFacet;
	if (facet_new != NULL)
		delete[] facet_new;
	if (ifMultiComponent != NULL)
		delete[] ifMultiComponent;

	DEBUG_END;
}
