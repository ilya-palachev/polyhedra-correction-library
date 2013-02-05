#include "Polyhedron.h"

void Polyhedron::intersection(Plane iplane) {

	int i, j, k;
	int nume;
	int res, total_edges;
	int* num_edges;
	int* lenff;
	int num_components;
	int id_v_new;
	int num_new_v;
	int v0;
	int v1;
	int fid, fid_curr, fid_next;
	int drctn;
	int v0_first;
	int v1_first;

	fprintf(stdout, "======================================================\n");
	fprintf(stdout, "Intersection the polyhedron by plane : \n");
	fprintf(stdout,
		"(%lf) * x + (%lf) * y + (%lf) * z + (%lf) = 0\n",
		iplane.norm.x, iplane.norm.y, iplane.norm.z, iplane.dist);


	num_edges = new int[numf];
	lenff = new int[numf];
	edge_list = new EdgeList[numf];

	nume = 0;
	for (i = 0; i < numf; ++i) {
		nume += facet[i].get_nv();
	}
	nume /= 2;
	EdgeSet edge_set(nume);
	EdgeSet total_edge_set(nume);

	// 1. Подготовка списков ребер
	total_edges = 0;
	for (i = 0; i < numf; ++i) {
		edge_list[i] = EdgeList(i, facet[i].get_nv(), this);
		res = facet[i].prepare_edge_list(iplane);
		edge_list[i].send_to_edge_set(&total_edge_set);
//		edge_list[i].my_fprint(stdout);
		num_edges[i] = res;
//		total_edges += res;
	}
//	total_edges /= 2;
	total_edges = total_edge_set.get_num();
#ifdef DEBUG1
	for (i = 0; i < numf; ++i) {
		fprintf(stdout,
				"num_edges[%d] = %d\n",
				i, num_edges[i]);
	}
#endif

	// 2. Подсчет числа компонент сечения
	FutureFacet superff(nume);

	num_new_v = 0;
	num_components = 0;
	while (total_edges > 0) {
#ifdef DEBUG1
		fprintf(stdout,
				"======================= S e a r c h i n g   c o m p o n e n t   %d ===================\n",
				num_components);
		fprintf(stdout,
				"total_edges = %d\n",
				total_edges);
		for (i = 0; i < numf; ++i) {
			fprintf(stdout,
					"num_edges[%d] = %d\n",
					i, num_edges[i]);
		}
//		return;
#endif

		for (i = 0; i < numf; ++i) {
			if (num_edges[i] > 0)
				break;
		}
		fid_curr = i;
		if (fid_curr == numf) {
			continue;
		}
		edge_list[fid_curr].get_first_edge(v0, v1, fid_next, drctn);
//		edge_list[fid_curr].set_isUsed(true);
		v0_first = v0;
		v1_first = v1;
#ifdef DEBUG1
			fprintf(stdout,
					"v0_first = %d, v1_first = %d\n",
					v0_first, v1_first);
#endif
		lenff[num_components] = 0;
		do {
			fid_curr = fid_next;
			--total_edges;
			--num_edges[fid_curr];
			++lenff[num_components];
			if (v0 != v1) {
				edge_set.add_edge(v0, v1);
			}
			superff.add_edge(v0, v1, fid);
#ifdef DEBUG1
			fprintf(stdout,
					"v0_first = %d, v1_first = %d, v0 = %d, v1 = %d, fid_curr = %d, drctn = %d\n",
					v0_first, v1_first, v0, v1, fid_curr, drctn);
#endif
			edge_list[fid_curr].get_next_edge(v0, v1, fid_next, drctn);
//			edge_list[fid_curr].set_isUsed(true);
			if (fid_next != fid_curr)
				--num_edges[fid_curr];

		} while (v0 != v0_first || v1 != v1_first);
//		edge_list[fid_next].set_isUsed(true);
//		for (i = 0; i < numf; ++i)
//			edge_list[i].my_fprint(stdout);
		++num_components;
	}

	#ifdef DEBUG1
		fprintf(stdout,
				"==============================================\ntotal_edges = %d\n",
				total_edges);
		for (i = 0; i < numf; ++i) {
			fprintf(stdout,
					"num_edges[%d] = %d\n",
					i, num_edges[i]);
		}
#endif

	//3. Расщепление компонент сечения
	FutureFacet* future_facet;
	future_facet = new FutureFacet[num_components];
	for (i = 0, k = 0; i < num_components; ++i) {
		future_facet[i] = FutureFacet(lenff[i]);
		future_facet[i].set_id(i);
		for (j = 0; j < lenff[i]; ++j) {
			superff.get_edge(k++, v0, v1, fid, id_v_new);
			future_facet[i].add_edge(v0, v1, fid);
		}
	}
#ifdef DEBUG
	fprintf(stdout, "%d components detected : \n", num_components);
	for (i = 0; i < num_components; ++i) {
		future_facet[i].my_fprint(stdout);
	}
#endif

	if (num_edges != NULL)
		delete[] num_edges;
	if (lenff != NULL)
		delete[] lenff;
	if (future_facet != NULL)
		delete[] future_facet;
}
