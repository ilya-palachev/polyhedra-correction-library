#include "PolyhedraCorrectionLibrary.h"

void my_fprint_Vector3d(Vector3d& v, FILE* file)
{
	fprintf(file, "(%lf, %lf, %lf)\n", v.x, v.y, v.z);
}

void Polyhedron::set_isUsed(int v0, int v1, bool val)
{
	int i;
	for (i = 0; i < numFacets; ++i)
	{
		edgeLists[i].set_isUsed(v0, v1, val);
	}
}

void Polyhedron::intersect(Plane iplane)
{

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

	fprintf(stdout,
			"\n======================================================\n");
	fprintf(stdout, "Intersection the polyhedron by plane : \n");
	fprintf(stdout, "(%lf) * x + (%lf) * y + (%lf) * z + (%lf) = 0\n",
			iplane.norm.x, iplane.norm.y, iplane.norm.z, iplane.dist);

	num_edges = new int[numFacets];
	lenff = new int[numFacets];
	edgeLists = new EdgeList[numFacets];

	nume = 0;
	for (i = 0; i < numFacets; ++i)
	{
		nume += facets[i].get_nv();
	}
	FutureFacet buffer_old(nume);
	nume /= 2;
	EdgeSet edge_set(nume);
	EdgeSet total_edge_set(nume);

	// 1. Подготовка списков ребер

	total_edges = 0;
	for (i = 0; i < numFacets; ++i)
	{
		edgeLists[i] = EdgeList(i, facets[i].get_nv(), this);
		res = facets[i].prepare_edge_list(iplane);
		edgeLists[i].send(&total_edge_set);
		edgeLists[i].send_edges(&edge_set);
#ifdef DEBUG
		//		if (edge_list[i].get_num() == 2)
		edgeLists[i].my_fprint(stdout);
		switch (edge_list[i].get_num())
		{
			case 2:
			++n_2;
			break;
			case 0:
			++n_0;
			break;
			default:
			++n_big;
		}
#endif
		num_edges[i] = res;
		edgeLists[i].set_poly(this);
	}
#ifdef DEBUG
	fprintf(stdout, "n_2 = %d, n_0 = %d, n_big = %d", n_2, n_0, n_big);
#endif
	total_edges = total_edge_set.get_num();

	// 2. Нахождение компонент сечения
	FutureFacet buffer_new(nume);

	num_components_new = 0;
	bool flag = true;
	while (total_edges > 0 && flag)
	{

		for (i = 0; i < numFacets; ++i)
		{
			if (num_edges[i] > 0)
				break;
		}
		fid_curr = i;
		if (fid_curr == numFacets)
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
#ifdef DEBUG
			//			if (v0 == 7352) {
			//				scanf("%d", &i);
			////				facet[fid_curr].my_fprint_all(stdout);
			////				vertexinfo[v0].my_fprint_all(stdout);
			//				edge_list[fid_curr].my_fprint(stdout);
			//			}
			//			fprintf(stdout, "v0 = %d, v1 = %d, fid = %d\n", v0, v1, fid_curr);
#endif
			buffer_new.add_edge(v0, v1, fid);
			if (buffer_new.get_nv() >= nume)
			{
				fprintf(stdout, "Error. Stack overflow  in buffer_new\n");
				return;
			}

			//            facet[fid_curr].my_fprint_all(stdout);
			//            edge_list[fid_curr].my_fprint(stdout);

			v0_prev = v0;
			v1_prev = v1;

			edgeLists[fid_curr].get_next_edge(iplane, v0, v1, fid_next, drctn);
			if ((v0_prev == v0 && v1_prev == v1)
					|| (v0_prev == v1 && v1_prev == v0))
			{
				fprintf(stdout, "Endless loop!!!!!\n");
				flag = false;
				break;
			}
			if (fid_next != fid_curr)
				--num_edges[fid_curr];

			if (v0 == -1 || v1 == -1)
			{
				fprintf(stdout, "Warning. v0 = %d, v1 = %d\n", v0, v1);
				return;
			}

		} while (v0 != v0_first || v1 != v1_first);
		++num_components_new;
	}
	fprintf(stdout, "====   SUPER-STEP 2 ENDED  =====");

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
	fprintf(stdout, "%d new facets : \n", num_components_new);
	for (i = 0; i < num_components_new; ++i)
	{
		future_facet_new[i].my_fprint(stdout);
	}
#endif

	//4. Нахождение компонент рассечения старых граней
	printf("4. Нахождение компонент рассечения старых граней\n");
	num_components_old = 0;
	num_saved_facets = 0;
	ifSaveFacet = new bool[numFacets];
	for (i = 0; i < numFacets; ++i)
	{
		printf("\tГрань %d\n", i);
		ifSaveFacet[i] = facets[i].intersect(iplane, buffer_old,
				num_components_local);
		if (!ifSaveFacet[i])
			printf("Facet %d is candidate for deleting\n", i);
		if (facets[i].numVertices < 3)
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
	fprintf(stdout, "ifSaveFacet : \n");
	for (i = 0; i < numFacets; ++i)
		if (!ifSaveFacet[i])
			fprintf(stdout, "Facet %d is deleted\n", i);
	fprintf(stdout, "\n");
#endif

	//6. Генерирование новых граней
	Facet* facet_new;
	numf_new = num_components_new + num_components_old;
	facet_new = new Facet[numf_new];
	for (i = 0; i < num_components_new; ++i)
	{
		future_facet_new[i].generate_facet(facet_new[i], numFacets + i, iplane,
				numVertices, &edge_set);
		facet_new[i].set_poly(this);
		facet_new[i].set_rgb(255, 0, 0);
	}
	for (i = 0; i < num_components_old; ++i)
	{
		future_facet_old[i].generate_facet(facet_new[i + num_components_new],
				numFacets + num_components_new + i, iplane, //TODO. Это неверно!!!
				numVertices, &edge_set);
		facet_new[i + num_components_new].set_poly(this);
		if (ifMultiComponent[i])
		{
			facet_new[i + num_components_new].set_rgb(0, 255, 0);
			fprintf(stdout, "Multi-component: %d\n", i);
		}
		else
			facet_new[i + num_components_new].set_rgb(100, 0, 0);
	}

#ifdef OUTPUT
	fprintf(stdout, "%d generated facets : \n",
			num_components_new + num_components_old);
	for (i = 0; i < numf_new; ++i)
		facet_new[i].my_fprint(stdout);
#endif

	//7. Создание массива граней нового многогранника. Старые грани удаляются
	Facet* facet_res;

	numf_res = num_saved_facets + numf_new;
	printf("numf_res = %d = %d + %d = num_saved_facets + numf_new", numf_res,
			num_saved_facets, numf_new);
	facet_res = new Facet[numf_res];

//    for (i = 0; i < numf; ++i) {
//        if (facet[i])
//    }

	j = 0;
	for (i = 0; i < numFacets; ++i)
	{
		if (ifSaveFacet[i])
		{
			facet_res[j] = facets[i];
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
	fprintf(stdout, "\n\n%d resulting facets : \n", numf_res);
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
		vec0 = vertices[v0];
		vec1 = vertices[v1];
		intersection(iplane, vec1 - vec0, vec0, vec);
		vertex_new[i] = vec;
	}

#ifdef OUTPUT
	fprintf(stdout, "%d new vertexes: \n", numv_new);
	for (i = 0; i < numv_new; ++i)
		my_fprint_Vector3d(vertex_new[i], stdout);
#endif

	numv_res = 0;
	for (i = 0; i < numVertices; ++i)
	{
		numv_res += signum(vertices[i], iplane) >= 0;
	}
	numv_res += numv_new;
	Vector3d* vertex_res;
	vertex_res = new Vector3d[numv_res];

	int sign;
	j = 0;
	for (i = 0; i < numVertices; ++i)
	{
		sign = signum(vertices[i], iplane);
		if (sign >= 0)
		{
			vertex_res[j] = vertices[i];
			for (k = 0; k < numf_res; ++k)
				facet_res[k].find_and_replace_vertex(i, j);
			++j;
		}
	}
	for (i = 0; i < numv_new; ++i)
	{
		vertex_res[j] = vertex_new[i];
		for (k = 0; k < numf_res; ++k)
			facet_res[k].find_and_replace_vertex(numVertices + i, j);
		++j;
	}

	numVertices = numv_res;
	numFacets = numf_res;
	if (vertices != NULL)
		delete[] vertices;
	if (facets != NULL)
		delete[] facets;
	vertices = vertex_res;
	facets = facet_res;

	for (i = 0; i < numFacets; ++i)
	{
		if (facets[i].numVertices < 1)
		{
			printf("===Facet %d is empty...\n", i);
			for (j = i; j < numFacets - 1; ++j)
				facets[j] = facets[j + 1];
			--numFacets;
		}
	}

	printf("After test there are %d facets:\n", numFacets);
	for (i = 0; i < numFacets; ++i)
		facets[i].my_fprint(stdout);

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
}
