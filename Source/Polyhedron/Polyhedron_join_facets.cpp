#include "PolyhedraCorrectionLibrary.h"

#define EPS_PARALL 1e-16
#define MAX_MIN_DIST 1e+1

void Polyhedron::coalesce_facets(int fid0, int fid1)
{
	Coalescer* coalescer = new Coalescer(this);
	coalescer->run(fid0, fid1);
	delete coalescer;
}



void Polyhedron::coalesce_facets_calculate_plane(int fid0, int fid1,
		Facet& join_facet, Plane& plane)
{

}

void Polyhedron::multi_coalesce_facets_calculate_plane(int n, int* fid,
		Facet& join_facet, Plane& plane)
{

	int *index, nv;
	int ninvert, i;

	join_facet.my_fprint_all(stdout);

	index = join_facet.indVertices;
	nv = join_facet.numVertices;

	list_squares_method(nv, index, &plane);

	//Проверка, что нормаль построенной плокости направлена извне многогранника

	ninvert = 0;
	for (i = 0; i < n; ++i)
	{
		ninvert += (plane.norm * facets[fid[i]].plane.norm < 0);
	}
	if (ninvert == n)
	{
		printf("...Меняем направление вычисленной МНК плоскости...\n");
		plane.norm *= -1.;
		plane.dist *= -1.;
	}
	else if (ninvert > 0)
	{
		printf(
				"Warning. Cannot define direction of facet (%d positive and %d negative)\n",
				n - ninvert, n);
	}
	printf(
			"За перемену направления плоскости проголосовало %d граней, а против - %d граней\n",
			ninvert, n - ninvert);
//    if (plane.norm * facet[fid[0]].plane.norm < 0) {
//        plane = -plane;
//        printf("...Меняем направление вычисленной МНК плоскости...\n");
//    }

	printf(
			"Caluclated plane: \n(%.16lf)x  +  (%.16lf)y  + (%.16lf)z  +  (%.16lf)  =  0\n",
			plane.norm.x, plane.norm.y, plane.norm.z, plane.dist);
	printf("Privious planes:\n");
	for (i = 0; i < n; ++i)
	{
		printf("\t(%.16lf)x  +  (%.16lf)y  + (%.16lf)z  +  (%.16lf)  =  0\n",
				plane.norm.x, plane.norm.y, plane.norm.z, plane.dist);
	}

}


void Polyhedron::multi_coalesce_facets_build_index(int n, int* fid,
		Facet& join_facet, int& nv)
{
	int i, j, *index, *nfind, nnv, nv_safe;
	int v_first, v;
	int i_next;
	bool* del;

	nv_safe = 0;
	for (i = 0; i < n; ++i)
	{
		nv_safe += facets[fid[i]].numVertices;
	}
	index = new int[3 * nv_safe + 1];
	nfind = new int[numVertices];
	for (i = 0; i < numVertices; ++i)
	{
		nfind[i] = 0;
	}
	for (i = 0; i < n; ++i)
	{
		for (j = 0; j < facets[fid[i]].numVertices; ++j)
		{
			++(nfind[facets[fid[i]].indVertices[j]]);
		}
	}

	for (i = 0; i < n; ++i)
	{
		printf("facet  fid[%d] = %d : \n\n", i, fid[i]);
		nnv = facets[fid[i]].numVertices;
		for (j = 0; j < nnv; ++j)
		{
			printf("\t%d", facets[fid[i]].indVertices[j]);
		}
		printf("\n");
		for (j = 0; j < nnv; ++j)
		{
			printf("\t%d", nfind[facets[fid[i]].indVertices[j]]);
		}
		printf("\n\n");
		facets[fid[i]].my_fprint_all(stdout);
	}

	nv = 0;
	j = 0;
	nnv = facets[fid[0]].numVertices;
	while (nfind[facets[fid[0]].indVertices[j]] > 1)
	{
		j = (nnv + j + 1) % nnv;
	}
	v_first = facets[fid[0]].indVertices[j];

	i = 0;
	v = v_first;
	printf("v = %d\n", v);
	printf("New index : ");
	do
	{
		printf("\n next facet %d\n", fid[i]);
		nnv = facets[fid[i]].numVertices;
		while (nfind[facets[fid[i]].indVertices[j]] == 1)
		{
			if (facets[fid[i]].indVertices[j] == v_first && nv > 0)
			{
				break;
			}
			index[nv] = facets[fid[i]].indVertices[j];

			printf("%d ", index[nv]);
			++nv;
			j = (nnv + j + 1) % nnv;
		}
		v = facets[fid[i]].indVertices[j];
		index[nv] = facets[fid[i]].indVertices[j];
		printf("%d\n", index[nv]);
		++nv;
		if (v == v_first)
			break;
		for (i = 0; i < n; ++i)
		{
			j = facets[fid[i]].find_vertex(v);
			printf("\tVertex %d was found in facet %d at position %d\n", v,
					fid[i], j);
			if (j != -1)
			{
				nnv = facets[fid[i]].numVertices;
				j = (nnv + j + 1) % nnv;
				printf("\tnfind[%d] = %d\n", facets[fid[i]].indVertices[j],
						nfind[facets[fid[i]].indVertices[j]]);
				if (nfind[facets[fid[i]].indVertices[j]] == 1)
				{
					break;
				}
			}
		}

		v = facets[fid[i]].indVertices[j];
		printf("v = %d\n", v);
	} while (v != v_first);
	printf("\n\n");
	--nv;

	join_facet = Facet(fid[0], nv, facets[fid[0]].plane, index, this, true);

	facets[fid[0]] = join_facet;
	for (i = 1; i < n; ++i)
	{
		facets[fid[i]].numVertices = 0;
		printf("Внимание! Грань %d пуста\n", fid[i]);
	}
	facets[fid[0]].my_fprint_all(stdout);

	preprocessAdjacency();
	printf("------End of preprocess_polyhedron...------\n");
	index = facets[fid[0]].indVertices;
	del = new bool[numVertices];

	for (i = 0; i < numVertices; ++i)
	{
		del[i] = false;
	}

	for (i = 0; i < nv; ++i)
	{
		i_next = (nv + i + 1) % nv;
		if (index[nv + 1 + i] == index[nv + 1 + i_next])
		{
			del[index[i_next]] = true;
		}
	}
	for (i = 0; i < numVertices; ++i)
	{
		if (del[i])
		{
			delete_vertex_polyhedron(i);
		}
	}

	if (del != NULL)
		delete[] del;
}

void Polyhedron::coalesce_facets_rise_find_step(int fid0, int i, double& d)
{


void Polyhedron::coalesce_facets_rise_point(int fid0, int imin)
{


}

void Polyhedron::delete_vertex_polyhedron(int v)
{
	printf("delete_vertex_polyhedron(%d)\n", v);
	for (int i = 0; i < numFacets; ++i)
		facets[i].delete_vertex(v);
}

int Polyhedron::appendVertex(Vector3d& vec)
{
	Vector3d* vertex1;
	vertex1 = new Vector3d[numVertices + 1];
	for (int i = 0; i < numVertices; ++i)
		vertex1[i] = vertices[i];
	vertex1[numVertices] = vec;
	if (vertices != NULL)
		delete[] vertices;
	vertices = vertex1;
	++numVertices;
	return numVertices - 1;
}

void Polyhedron::changeVertex(int position, Vector3d vec)
{
	if (position >= numVertices)
	{
		ERROR_PRINT("Cannot set %d-th vertex, because number of vertices",
				position);
		ERROR_PRINT("is bounded with %d", numVertices);
		return;
	}
	vertices[position] = vec;
}

void Polyhedron::printVertex(int i)
{
	printf("vertex %d : (%lf , %lf , %lf)\n", i, vertices[i].x, vertices[i].y,
			vertices[i].z);
}

void Polyhedron::find_and_replace_vertex(int from, int to)
{
	int i;
	for (i = 0; i < numFacets; ++i)
	{
		facets[i].find_and_replace_vertex(from, to);
	}
	for (i = 0; i < numVertices; ++i)
	{
		vertexInfos[i].find_and_replace_vertex(from, to);
	}
}

void Polyhedron::find_and_replace_facet(int from, int to)
{
	int i;
	for (i = 0; i < numFacets; ++i)
	{
		facets[i].find_and_replace_facet(from, to);
	}
	for (i = 0; i < numVertices; ++i)
	{
		vertexInfos[i].find_and_replace_facet(from, to);
	}
}

void Polyhedron::list_squares_method(int nv, int *vertex_list, Plane *plane)
{
	int i, id;
	double *x, *y, *z, a, b, c, d;

	x = new double[nv];
	y = new double[nv];
	z = new double[nv];

	for (i = 0; i < nv; ++i)
	{
		id = vertex_list[i];
		x[i] = vertices[id].x;
		y[i] = vertices[id].y;
		z[i] = vertices[id].z;
	}

	aprox(nv, x, y, z, a, b, c, d);

	plane->norm.x = a;
	plane->norm.y = b;
	plane->norm.z = c;
	plane->dist = d;

	//debug
	//printf("list_squares_method: a = %lf, b = %lf, c = %lf, d = %lf\n", a, b, c, d);

	if (x != NULL)
		delete[] x;
	if (y != NULL)
		delete[] y;
	if (z != NULL)
		delete[] z;
}

void Polyhedron::list_squares_method_weight(int nv, int *vertex_list,
		Plane *plane)
{
	int i, id, i_prev, i_next, id0, id1;
	double *x, *y, *z, a, b, c, d;
	double *l, *w;

	x = new double[nv];
	y = new double[nv];
	z = new double[nv];
	l = new double[nv];
	w = new double[nv];

	for (i = 0; i < nv; ++i)
	{
		i_next = (i + 1) % nv;
		id0 = vertex_list[i];
		id1 = vertex_list[i_next];
		l[i] = sqrt(qmod(vertices[id0] - vertices[id1]));
	}

	for (i = 0; i < nv; ++i)
	{
		i_prev = (i - 1 + nv) % nv;
		w[i] = 0.5 * (l[i_prev] + l[i]);
	}

	for (i = 0; i < nv; ++i)
	{
		id = vertex_list[i];
		x[i] = vertices[id].x * w[i];
		y[i] = vertices[id].y * w[i];
		z[i] = vertices[id].z * w[i];
	}

	aprox(nv, x, y, z, a, b, c, d);

	plane->norm.x = a;
	plane->norm.y = b;
	plane->norm.z = c;
	plane->dist = d;

	//debug
	//printf("list_squares_method: a = %lf, b = %lf, c = %lf, d = %lf\n", a, b, c, d);

	if (x != NULL)
		delete[] x;
	if (y != NULL)
		delete[] y;
	if (z != NULL)
		delete[] z;
	if (z != NULL)
		delete[] l;
	if (z != NULL)
		delete[] w;
}

int Polyhedron::test_structure()
{
	int i, res;
	res = 0;
	for (i = 0; i < numFacets; ++i)
	{
		res += facets[i].test_structure();
	}
	return res;
}
