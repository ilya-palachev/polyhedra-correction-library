#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Polyhedron.h"


void Polyhedron::preprocess_polyhedron()
{
	int i;
	preprocess_free_facet();
	//printf("pre0\n");
	for(i = 0; i < numf; ++i)
	{
		facet[i].index[facet[i].nv] = facet[i].index[0]; //cycling list
	}
	for(i = 0; i < numf; ++i)
	{
		preprocess_facet(i);
		facet[i].rgb[0] = 100;
		facet[i].rgb[1] = 100;
		facet[i].rgb[2] = 100;
		//printf("pre1 %d\n", i);
	}
	//printf("pre1.1\n");
	if(vertexinfo != NULL) delete[] vertexinfo;
	vertexinfo = new VertexInfo[numv];
	//printf("pre1.2\n");
	for(i = 0; i < numv; ++i)
	{
		//printf("pre2 %d - begin\n", i);
		preprocess_vertexinfo(i);
		//printf("pre2 %d - end\n", i);
	}
}

void Polyhedron::preprocess_free_facet()
{
	int i, j, nv, *index;
	for(i = 0; i < numf; ++i)
	{
		nv = facet[i].nv;
		index = facet[i].index;
		for(j = nv + 1; j < 3*nv + 1; ++j)
			index[j] = -1;
	}
}

void Polyhedron::preprocess_facet(int id)
{
	int *index, nv, i, v0, v1;

	index = facet[id].index;
	nv = facet[id].nv;

	for(i = 0; i < nv; ++i)
	{
		v0 = index[i];
		v1 = index[i + 1];
		preprocess_edge(v0, v1, id, i);
	}
}

void Polyhedron::preprocess_edge(int v0, int v1, int facet_id, int v0_id)
{
	int i, *index, nv, j;

	index = facet[facet_id].index;
	nv = facet[facet_id].nv;
	if(index[v0_id + nv + 1] != -1)
		return;

	//printf("preprocess_edge %d %d face %d id %d\n", v0, v1, facet_id, v0_id);

	for(i = 0; i < numf; ++i)
	{
		if(i == facet_id)
			continue;
		index = facet[i].index;
		nv = facet[i].nv;
		for(j = 0; j < nv; ++j)
		{
			if(index[j] == v1 && index[j + 1] == v0)
			{
				//j = j < nv - 1 ? j + 1 : 0;
				//printf("preprocess_edge: found face %d id %d\n", i, j);
				facet[facet_id].index[v0_id + facet[facet_id].nv + 1] = i;
				////printf("\t\tfacet[%d].index[%d] = %d\n", facet_id, v0_id + facet[facet_id].nv + 1, i);
				facet[facet_id].index[v0_id + 2*facet[facet_id].nv + 1] = j < nv - 1 ? j + 1 : 0;
				////printf("\t\tfacet[%d].index[%d] = %d\n", facet_id, v0_id + 2*facet[facet_id].nv + 1, j);
				index[j + nv + 1] = facet_id;
				////printf("\t\tfacet[%d].index[%d] = %d\n", i, j + nv + 1, facet_id);
				index[j + 2*nv + 1] = v0_id < facet[facet_id].nv - 1 ? v0_id + 1 : 0;
				////printf("\t\tfacet[%d].index[%d] = %d\n", i, j + 2*nv + 1, v0_id);
				i = numf; //stop for(i)
				break;
			}
		}
	}
}

void Polyhedron::preprocess_vertexinfo(int id)
{
	int i, j, nv, *index, *index1, nf, id_f_next, v0_next, v1_next,
		id_f_curr, v0_curr, v1_curr, id_f_first;

	//calculate nf
	//printf("\tpreprocess vertexinfo[%d]\n", id);
	for(i = 0; i < numf; ++i)
	{
		j = find_vertex(i, id);
		if(j > -1)
		{
			nv = facet[i].nv;
			index = facet[i].index;

			id_f_first = id_f_next = i;
			v1_next = index[j];
			v0_next = index[j > 0 ? j - 1 : nv - 1];
			i = numf;
			break;
		}
		/*
		nv = facet[i].nv;
		index = facet[i].index;
		for(j = 0; j < nv; ++j)
			if(index[j] == id)
			{
				id_f_next = i;
				v1_next = index[j];
				v0_next = index[j > 0 ? j - 1 : nv - 1];
				i = numf;
				break;
			}*/
	}
	nf = 0;
	do
	{
		//printf("\t\tnext: facet %d, edge %d %d\n", id_f_next, v0_next, v1_next);
		++nf;
		id_f_curr = id_f_next;
		v0_curr = v0_next;
		v1_curr = v1_next;
		id_f_next = find_edge(v1_curr, v0_curr);

		j = find_vertex(id_f_next, v1_curr);
		nv = facet[id_f_next].nv;
		index = facet[id_f_next].index;

		v0_next = j > 0 ? index[j - 1] : index[nv - 1];
		v1_next = v1_curr;
		if(id_f_next == -1)
			break;
	}while(id_f_next != id_f_first);

	index1 = new int[2*nf + 1];
	i = 0;
	do
	{
		id_f_curr = id_f_next;
		v0_curr = v0_next;
		v1_curr = v1_next;
		id_f_next = find_edge(v1_curr, v0_curr);
		index1[i] = id_f_next;

		j = find_vertex(id_f_next, v1_curr);
		index1[nf + 1 + i] = j;
		++i;

		nv = facet[id_f_next].nv;
		index = facet[id_f_next].index;

		v0_next = j > 0 ? index[j - 1] : index[nv - 1];
		v1_next = v1_curr;
		if(id_f_next == -1)
			break;
	}while(id_f_next != id_f_first);
	index1[nf] = index1[0]; //cycling list
	vertexinfo[id] = VertexInfo(index1, nf);
}

int Polyhedron::find_edge(int v0, int v1)
{
	int i, *index, nv, j;

	for(i = 0; i < numf; ++i)
	{
		index = facet[i].index;
		nv = facet[i].nv;
		for(j = 0; j < nv; ++j)
		{
			if(index[j] == v0 && index[j + 1] == v1)
				return i;
		}
	}
	fprintf(stderr, "Polyhedron::find_edge. Error. Cannot find %d %d\n", v0, v1);
	return -1;
}

int Polyhedron::find_vertex(int f_id, int v)
{
	int i, nv, *index;
	nv = facet[f_id].nv;
	index = facet[f_id].index;
	//printf("find_vertex\n");
	for(i = 0; i < nv; ++i)
	{
		//printf("%d ", index[i]);
		if(index[i] == v)
		{
			//printf("Polhedron::find_vertex: vertex %d found in facet %d i = %d\n", v, f_id, i);
			return i;
		}
	}
	//fprintf(stderr, "Polyhedron::find_vertex. Error. Cannot find %d at %d\n", v, f_id);
	return -1;
}


