#include "PolyhedraCorrectionLibrary.h"

#define EPS_PARALL 1e-16
#define MAX_MIN_DIST 1e+1

void Polyhedron::coalesce_facets(int fid0, int fid1)
{

	int nv;
	Plane plane;
	Facet join_facet;

	if (fid0 == fid1)
	{
		fprintf(stderr, "join_facets: Error. Cannot join facet with itself.\n");
		return;
	}

	// I ). Составление списка вершин
	printf("I ). Составление списка вершин\n");
	coalesce_facets_build_index(fid0, fid1, plane, join_facet, nv);
	if (nv == -1)
		return;

	// II ). Вычисление средней плоскости
	printf("II ). Вычисление средней плоскости\n");
	coalesce_facets_calculate_plane(fid0, fid1, join_facet, plane);
	join_facet.plane = plane;

	// III ). Дополнительная предобработка многогранника
	printf("III ). Дополнительная предобработка многогранника\n");
	facets[fid0] = join_facet;
	facets[fid1] = Facet();
	preprocessAdjacency();

	// IV ). Алгортм поднятия вершин, лежащих ниже плоскости
	printf("IV ). Алгортм поднятия вершин, лежащих ниже плоскости\n");
	coalesce_facets_rise(fid0);

	// V ). Предобработка многогранника после поднятия
	printf("V ). Предобработка многогранника после поднятия\n");
	preprocessAdjacency();

	//     VI). Рассечение многогранника плоскостью
	printf("VI). Рассечение многогранника плоскостью\n");
	intersectJoinMode(-plane, fid0);

	test_consections(true);

	printf("=====================================================\n");
	printf("=========    Грани %d и %d объединены !!!   =========\n", fid0,
			fid1);
	printf("=====================================================\n");
}

void Polyhedron::multi_coalesce_facets(int n, int *fid)
{

	int i;
	int nv;
	Plane plane;
	Facet join_facet;
	int fid0 = fid[0];

	// I ). Составление списка вершин
	printf("I ). Составление списка вершин\n");
	multi_coalesce_facets_build_index(n, fid, join_facet, nv);
	if (nv == -1)
		return;

	// II ). Вычисление средней плоскости
	printf("II ). Вычисление средней плоскости\n");
	multi_coalesce_facets_calculate_plane(n, fid, join_facet, plane);
	facets[fid[0]].plane = plane;

	// III ). Дополнительная предобработка многогранника
	printf("III ). Дополнительная предобработка многогранника\n");
	preprocessAdjacency();
	facets[fid[0]].my_fprint_all(stdout);

	// IV ). Алгортм поднятия вершин, лежащих ниже плоскости
	printf("IV ). Алгортм поднятия вершин, лежащих ниже плоскости\n");
	coalesce_facets_rise(fid0);

	// V ). Предобработка многогранника после поднятия
	printf("V ). Предобработка многогранника после поднятия\n");
	preprocessAdjacency();

	//     VI). Рассечение многогранника плоскостью
//    printf("VI). Рассечение многогранника плоскостью\n");
//    intersect_j(-plane, fid0);

	test_consections(true);

	printf("=====================================================\n");
	printf("=========    Грани ");
	printf("%d", fid[0]);
	for (i = 1; i < n - 1; ++i)
	{
		printf(", %d", fid[i]);
	}
	printf(" и %d ", fid[n - 1]);
	printf("объединены !!!   =========\n");
	printf("=====================================================\n");
}

void Polyhedron::coalesce_facets_calculate_plane(int fid0, int fid1,
		Facet& join_facet, Plane& plane)
{

	int *index, nv;

	join_facet.my_fprint_all(stdout);

	index = join_facet.indVertices;
	nv = join_facet.numVertices;

	list_squares_method(nv, index, &plane);

	//Проверка, что нормаль построенной плокости направлена извне многогранника
	if (plane.norm * facets[fid0].plane.norm < 0
			&& plane.norm * facets[fid1].plane.norm < 0)
	{
		plane.norm *= -1.;
		plane.dist *= -1.;
	}
	printf("Caluclated plane: (%lf)x  +  (%lf)y  + (%lf)z  +  (%lf)  =  0\n",
			plane.norm.x, plane.norm.y, plane.norm.z, plane.dist);

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
//void Polyhedron::join_facets_calculate_plane(int fid0, int fid1, Plane& plane, int& nv) {
//
//    int i, j;
//    int nv0, *index0;
//    int nv1, *index1;
//    int *index;
//    bool *is;
//
//
//    nv0 = facet[fid0].nv;
//    nv1 = facet[fid1].nv;
//    index0 = facet[fid0].index;
//    index1 = facet[fid1].index;
//
//    is = new bool[numv];
//    for (i = 0; i < numv; ++i)
//        is[i] = false;
//    for (i = 0; i < nv0; ++i)
//        is[index0[i]] = true;
//    for (i = 0; i < nv1; ++i)
//        is[index1[i]] = true;
//    nv = 0;
//    for (i = 0; i < numv; ++i)
//        if (is[i])
//            ++nv;
//    index = new int[nv];
//    for (i = 0, j = 0; i < numv; ++i)
//        if (is[i])
//            index[j++] = i;
//    list_squares_method(nv, index, &plane);
//
//    //Проверка, что нормаль построенной плокости направлена извне многогранника
//    if (plane.norm * facet[fid0].plane.norm < 0 &&
//            plane.norm * facet[fid1].plane.norm < 0) {
//        plane.norm *= -1.;
//        plane.dist *= -1.;
//    }
//
//    if (index != NULL)
//        delete[] index;
//    if (is != NULL)
//        delete[] is;
//}

void Polyhedron::coalesce_facets_build_index(int fid0, int fid1, Plane& plane,
		Facet& join_facet, int& nv)
{

	int i, j;
	int *index, *index0, *index1, nv0, nv1;
	bool *is, *del;

	nv0 = facets[fid0].numVertices;
	nv1 = facets[fid1].numVertices;
	nv = nv0 + nv1;
	index = new int[3 * nv + 1];
	index0 = facets[fid0].indVertices;
	index1 = facets[fid1].indVertices;

	// 1. Найдем общую вершину граней
	for (i = 0; i < nv1; ++i)
		if (facets[fid0].find_vertex(index1[i]) != -1)
			break;
	if (i == nv1)
	{
		printf(
				"join_facets : Error. Facets %d and %d have no common vertexes\n",
				fid0, fid1);
		nv = -1;
		return;
	}

	// 2. Найдем последнюю против часовой стрелки (для 1-й грани) общую точку граней
	i = facets[fid0].find_vertex(index1[i]);
	while (facets[fid1].find_vertex(index0[i]) != -1)
	{
		i = (nv0 + i + 1) % nv0;
	}

	// 3. Если эта точка висячая, то не добавляем ее
	i = (nv0 + i - 1) % nv0;
	j = 0;
	if (vertexInfos[index0[i]].get_nf() > 3)
		index[j++] = index0[i];

	// 4. Собираем все точки 1-й грани против часовой стрелки,
	//    пока не наткнемся на 2-ю грань
	i = (nv0 + i + 1) % nv0;
	while (facets[fid1].find_vertex(index0[i]) == -1)
	{
		index[j++] = index0[i];
		i = (nv0 + i + 1) % nv0;
	}

	// 5. Если крайняя точка не висячая, то добавляем ее
	//    i = (nv0 + i - 1) % nv0;
	if (vertexInfos[index0[i]].get_nf() > 3)
		index[j++] = index0[i];

	// 6. Собираем все точки 2-й грани против часовой стрелки, 
	// пока не наткнемся на 1-ю грань
	i = facets[fid1].find_vertex(index0[i]);
	i = (nv1 + i + 1) % nv1;
	while (facets[fid0].find_vertex(index1[i]) == -1)
	{
		index[j++] = index1[i];
		i = (nv1 + i + 1) % nv1;
	}
	nv = j;

	// 7. Общие и висячие вершины объединяемых граней удаляются из
	//    многогранника
	is = new bool[numVertices];
	del = new bool[numVertices];
	for (i = 0; i < numVertices; ++i)
	{
		is[i] = false;
		del[i] = false;
	}
	printf("Resulting index: ");
	for (i = 0; i < nv; ++i)
	{
		is[index[i]] = true;
		printf("%d ", index[i]);
	}
	printf("\n");

	for (i = 0; i < nv0; ++i)
		if (!is[index0[i]])
			del[index0[i]] = true;
	for (i = 0; i < nv1; ++i)
		if (!is[index1[i]])
			del[index1[i]] = true;
	for (i = 0; i < numVertices; ++i)
		if (del[i])
			delete_vertex_polyhedron(i);

	// 8. По построенному списку создается грань
	join_facet = Facet(fid0, nv, plane, index, this, true);

	if (is != NULL)
		delete[] is;
	if (del != NULL)
		delete[] del;
	if (index != NULL)
		delete[] index;
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

void Polyhedron::coalesce_facets_rise(int fid0)
{

	int i;
	int nv, *index, *sign_vertex;
	int ndown;
	int step;
	int imin;
	int sign;

	Plane plane;

	char *file_name_out;

	file_name_out = new char[255];

	nv = facets[fid0].numVertices;
	index = facets[fid0].indVertices;
	plane = facets[fid0].plane;
	printf(
			"Поднимаем до плоскости: (%lf)x  +  (%lf)y  + (%lf)z  +  (%lf)  =  0\n",
			plane.norm.x, plane.norm.y, plane.norm.z, plane.dist);

	// 1 ). Посчитаем знаки вершин контура и количество вершин ниже грани
	sign_vertex = new int[nv];
	ndown = 0;
	for (i = 0; i < nv; ++i)
	{
		sign = signum(vertices[index[i]], plane);
		printf("\tsignum[%d] = %d, (%.16lf)\n", index[i], sign,
				plane % vertices[index[i]]);
		sign_vertex[i] = sign;
		if (sign_vertex[i] == -1)
			++ndown;
	}

	int ndown_total = 0, nup_total;
	for (i = 0; i < numVertices; ++i)
	{
		sign = signum(vertices[i], plane);
		ndown_total += (sign == -1);
		nup_total = numVertices - ndown_total;
	}
	printf("TOTALY ndown = %d, nup = %d\n", ndown_total, nup_total);

	// 2 ). Цикл по всем низлежащим вершинам
	step = 0;
	while (ndown > 0)
	{
		printf(
				"-------------------------------------------------------------\n");
		printf(
				"-----------------     Шаг  %d         -----------------------\n",
				step);
		printf(
				"-------------------------------------------------------------\n");
		printf("ndown = %d\n", ndown);
		facets[fid0].my_fprint_all(stdout);

		sprintf(file_name_out, "../poly-data-out/multijoin-facets-%d.ply",
				step);
		fprint_ply_scale(1000., file_name_out, "join-facets-rise");
		// 2. 1). Находим, по какой вершине нужно шагать, и минимальное расстояние
		printf(
				"\t2. 1). Находим, по какой вершине нужно шагать, и минимальное расстояние\n");
		coalesce_facets_rise_find(fid0, imin);

		if (imin == -1)
		{
			printf(
					"join_facets_rise : Ошибка. Не удалось найти вершину (imin = -1)\n");
			return;
		}

		// 2. 2). Шагаем по выбранной вершине
		printf("\t2. 2). Шагаем по выбранной вершине\n");
		facets[fid0].my_fprint_all(stdout);
		coalesce_facets_rise_point(fid0, imin);

		if (test_structure() > 0)
		{
			printf("Не пройден тест на структуру!\n");
			return;
		}

		++step;
		nv = facets[fid0].numVertices;
		index = facets[fid0].indVertices;
		ndown = 0;
		for (i = 0; i < nv; ++i)
		{
			sign = signum(vertices[index[i]], plane);
			printf("\tsignum[%d] = %d, (%.16lf)\n", index[i], sign,
					plane % vertices[index[i]]);
			if (sign == -1)
				++ndown;
		}
		printf("Used plane: (%lf)x  +  (%lf)y  + (%lf)z  +  (%lf)  =  0\n",
				plane.norm.x, plane.norm.y, plane.norm.z, plane.dist);

		preprocessAdjacency();
	}

	printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	printf("!!!!!!!!     ВЕРШИНЫ ПОДНЯТЫ          !!!!!!!!!!\n");
	printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	facets[fid0].my_fprint_all(stdout);

	if (sign_vertex != NULL)
		delete[] sign_vertex;
	if (file_name_out != NULL)
		delete[] file_name_out;

}

void Polyhedron::coalesce_facets_rise_find(int fid0, int& imin)
{
	int i;
	int nv, *index;
	int fr1, fr2;

	double d, dmin;
	int pos, tmp;

	Plane plane;

	nv = facets[fid0].numVertices;
	index = facets[fid0].indVertices;
	plane = facets[fid0].plane;

	printf("Предварительный анализ вершин: ");
	for (i = 0; i < nv; ++i)
	{
		printf("\t\t2. 1. %d ). Предварительный анализ %d-й вершины : ", i,
				index[i]);
		if (signum(vertices[index[i]], plane) != -1)
		{
			printf("выше плоскости или на плоскости\n");
			continue;
		}
//        printf("\t%d ( %d )", i, index[i]);
		//Написано 2012-03-31 для решения проблемы с треугольными соседними гранями
		fr1 = index[nv + 1 + (nv + i) % nv];
		fr2 = index[nv + 1 + (nv + i + 1) % nv];
		//        if (facet[fl1].nv < 4) {
		//            delete_vertex_polyhedron(index[i]);
		//            facet[fl1] = Facet();
		//            --i;
		//            continue;
		//        } else 
		if (facets[fr1].numVertices == 3)
		{
			facets[fr1].my_fprint(stdout);
			pos = index[2 * nv + 1 + i];
			pos = (pos + 1) % 3;
			tmp = index[i];
			printf(" - Вместо вершины %d  в главной грани пишем вершину %d\n",
					index[i], facets[fr1].indVertices[pos]);
			index[i] = facets[fr1].indVertices[pos];
			delete_vertex_polyhedron(tmp);
			facets[fr1] = Facet();
			if (vertexInfos[index[i + 1]].numFacets == 3)
			{
				delete_vertex_polyhedron(index[i + 1]);
			}
			preprocessAdjacency();
			--i;
			facets[fid0].my_fprint_all(stdout);
			facets[fr2].my_fprint_all(stdout);
//            continue;
		}
		//конец написанного 2012-03-31
		printf("\n");
	}

	imin = -1;
	for (i = 0; i < nv; ++i)
	{
		printf("\t\t2. 1. %d ). Обработка %d-й вершины : ", i, index[i]);
		if (signum(vertices[index[i]], plane) != -1)
		{
			printf("выше плоскости или на плоскости\n");
			continue;
		}
		printf("ниже плоскости");
		//        printf("\n");

		coalesce_facets_rise_find_step(fid0, i, d);

		if (d < 0)
		{
			//Если перемещение отдаляет точку от плоскости
			//?????????????????????????????
			continue;
		}
		if ((dmin > d || imin == -1) && d > 1e-15)
		{
			dmin = d;
			imin = i;
		}
	}
}

void Polyhedron::coalesce_facets_rise_find_step(int fid0, int i, double& d)
{
	int nv, *index;
	int fl2, fl1, fr1, fr2;

	double d1, d2;

	Plane plane;
	Plane pl2, pl1, pr1, pr2;
	Vector3d v1, v2;

	nv = facets[fid0].numVertices;
	index = facets[fid0].indVertices;
	plane = facets[fid0].plane;

	// Номера соседних с контуром граней вблизи рассматриваемой точки:
	fl2 = index[nv + 1 + (nv + i - 2) % nv];
	fl1 = index[nv + 1 + (nv + i - 1) % nv];
	fr1 = index[nv + 1 + (nv + i) % nv];
	fr2 = index[nv + 1 + (nv + i + 1) % nv];

	pl2 = facets[fl2].plane;
	pl1 = facets[fl1].plane;
	pr1 = facets[fr1].plane;
	pr2 = facets[fr2].plane;

	// Найдем точку пересечения первой тройки граней

	if (qmod(pl2.norm % pr1.norm) < EPS_PARALL)
	{
		// Если грани параллельны: 
		printf("(parallel)");
		intersection(plane, pl1, pr1, v1);
	}
	else
	{
		// Если грани не параллельны:
		intersection(pl2, pl1, pr1, v1);
		if (signum(v1, plane) == 1)
		{
			// Если точка пересечения лежит выше плоскости:
			intersection(plane, pl1, pr1, v1);
		}
	}
	// Найдем проекцию перемещения точки на нормаль плоскости:
	d1 = (v1 - vertices[index[i]]) * plane.norm;
	if (facets[fl1].numVertices < 4)
		d1 = -1;
	printf("\td1 = %lf", d1);

	// Найдем точку пересечения второй тройки граней

	if (qmod(pl1.norm % pr2.norm) < EPS_PARALL)
	{
		// Если грани параллельны: 
		printf("(parallel)");
		intersection(plane, pl1, pr1, v2);
	}
	else
	{
		// Если грани не параллельны:
		intersection(pl1, pr1, pr2, v2);
		if (signum(v2, plane) == 1)
		{
			// Если точка пересечения лежит выше плоскости:
			intersection(plane, pl1, pr1, v2);
		}
	}
	// Найдем проекцию перемещения точки на нормаль плоскости:
	d2 = (v2 - vertices[index[i]]) * plane.norm;
	if (facets[fr1].numVertices < 4)
		d2 = -1;
	printf("\td2 = %lf", d2);
	printf("\n");

	d = (d1 < d2) ? d1 : d2;
	if (d1 < 0 && d2 > 0)
		d = d2;
	if (d1 > 0 && d2 < 0)
		d = d1;

	if (d < 0)
	{
		printf("For vertex %d:\n", i);
		facets[fl1].my_fprint_all(stdout);
		facets[fr1].my_fprint_all(stdout);
	}
}

void Polyhedron::coalesce_facets_rise_point(int fid0, int imin)
{

	int nv, *index;
	int fl2, fl1, fr1, fr2;
	int irep;
	int ind_new;
	int pos, what;

	double d1, d2;

	Plane plane;
	Plane pl2, pl1, pr1, pr2;
	Vector3d v1, v2;

	bool ifjoin;
	bool deg_imin_big, deg_irep_big;
	int deg_imin, deg_irep;

	int nnv;

	nv = facets[fid0].numVertices;
	index = facets[fid0].indVertices;
	plane = facets[fid0].plane;

	// Номера соседних с контуром граней вблизи рассматриваемой точки:
	fl2 = index[nv + 1 + (nv + imin - 2) % nv];
	fl1 = index[nv + 1 + (nv + imin - 1) % nv];
	fr1 = index[nv + 1 + (nv + imin) % nv];
	fr2 = index[nv + 1 + (nv + imin + 1) % nv];

	printf("\t\tПоднимаем %d-ю вершину\n", index[imin]);
	printf("\t\t\tКартина такая: ---%d---%d---%d---\n",
			index[(nv + imin - 1) % nv], index[imin],
			index[(nv + imin + 1) % nv]);
	printf("\t\t\t                   |     |     |     \n");
	printf("\t\t\t               %d  | %d  | %d  | %d  \n", fl2, fl1, fr1, fr2);

	pl2 = facets[fl2].plane;
	pl1 = facets[fl1].plane;
	pr1 = facets[fr1].plane;
	pr2 = facets[fr2].plane;

	ifjoin = false; // Эта константа отвечает за то, сливаются ли 2 точки или нет

	// Найдем точку пересечения первой тройки граней

	if (qmod(pl2.norm % pr1.norm) < EPS_PARALL)
	{
		// Если грани параллельны: 
		intersection(plane, pl1, pr1, v1);
	}
	else
	{
		// Если грани не параллельны:
		intersection(pl2, pl1, pr1, v1);
//        if (signum(v1, plane) == 1) {
//            // Если точка пересечения лежит выше плоскости:
//            printf("левая точка пересечения лежит выше плоскости\n");            
//            intersection(plane, pl1, pr1, v1);
//        } else 
		ifjoin = true;

	}
	// Найдем проекцию перемещения точки на нормаль плоскости:
	d1 = (v1 - vertices[index[imin]]) * plane.norm;
	printf("\td1 = %.16lf", d1);

	// Найдем точку пересечения второй тройки граней

	if (qmod(pl1.norm % pr2.norm) < EPS_PARALL)
	{
		// Если грани параллельны: 
		intersection(plane, pl1, pr1, v2);
	}
	else
	{
		// Если грани не параллельны:
		intersection(pl1, pr1, pr2, v2);
//        if (signum(v2, plane) == 1) {
//            // Если точка пересечения лежит выше плоскости:
//            printf("правая точка пересечения лежит выше плоскости\n");
//            intersection(plane, pl1, pr1, v2);
//        } else 
		ifjoin = true;

	}
	// Найдем проекцию перемещения точки на нормаль плоскости:
	d2 = (v2 - vertices[index[imin]]) * plane.norm;
	printf("\td2 = %.16lf\n", d2);

	if ((d1 <= d2 && d1 > 0) || (d1 > 0 && d2 <= 0))
	{
		printf("Движение определяет левый сосед вершины\n");
		// Движение определяет левый сосед вершины
		if (ifjoin)
		{
			// Если нужно объединяtellteть вершины

			irep = (nv + imin - 1) % nv;
			deg_imin = vertexInfos[index[imin]].get_nf();
			deg_imin_big = deg_imin > 3;
			deg_irep = vertexInfos[index[irep]].get_nf();
			deg_irep_big = deg_irep > 3;
			printf("\tdeg_imin = deg(%d) = %d\n", index[imin], deg_imin);
			printf("\tdeg_irep = deg(%d) = %d\n", index[irep], deg_irep);

			if (deg_imin_big && deg_irep_big)
			{

				// Создать новую вершину с номером ind_new
				ind_new = add_vertex(v1);

				// Добавить v1 в грань fl2 с информацией

				what = ind_new;
				pos = index[2 * nv + 1 + (nv + irep - 1) % nv];
				facets[fl2].add(what, pos);

				nnv = facets[fl2].numVertices;
				what = fid0;
				pos += nnv + 1;
				facets[fl2].add(what, pos);

				what = -1;
				pos += nnv;
				facets[fl2].add(what, pos);

				pos = index[2 * nv + 1 + (nv + irep - 1) % nv];
				pos = (nnv + pos - 1) % nnv;
				facets[fl2].indVertices[nnv + 1 + pos] = fl1;

				facets[fl2].indVertices[facets[fl2].numVertices] =
						facets[fl2].indVertices[0];

				//Добавить v1 в грань fl1 с информацией

				what = ind_new;
				pos = index[2 * nv + 1 + irep];
				facets[fl1].add(what, pos);

				nnv = facets[fl1].numVertices;
				what = fl2;
				pos += nnv + 1;
				facets[fl1].add(what, pos);

				what = -1;
				pos += nnv;
				facets[fl1].add(what, pos);

				pos = index[2 * nv + 1 + irep];
				pos = (nnv + pos - 1) % nnv;
				facets[fl1].indVertices[nnv + 1 + pos] = fr1;
				//                printf("===Trying to add %d at position %d in facet %d===\n", fr1, nnv + 1 + pos, )

				facets[fl1].indVertices[facets[fl1].numVertices] =
						facets[fl1].indVertices[0];

				//Добавить v1 в грань fr1 с информацией

				what = ind_new;
				pos = index[2 * nv + 1 + imin];
				facets[fr1].add(what, pos);

				nnv = facets[fr1].numVertices;
				what = fl1;
				pos += nnv + 1;
				facets[fr1].add(what, pos);

				what = -1;
				pos += nnv;
				facets[fr1].add(what, pos);

				facets[fr1].indVertices[facets[fr1].numVertices] =
						facets[fr1].indVertices[0];

				//Заменить irep в грани fid0 на ind_new
				index[irep] = ind_new;
				index[irep + nv + 1] = fr1;
				// Удалить вершину imin из главной грани (fid0) и исправим грань                
				facets[fid0].remove(imin); // с информацией!!!

				facets[fid0].update_info();
				facets[fl2].update_info();
				facets[fl1].update_info();
				facets[fr1].update_info();

			}
			else if (deg_imin_big && !deg_irep_big)
			{
				vertices[index[irep]] = v1;

				//Добавить v1 в грань fr1 с информацией

				what = index[irep];
				pos = index[2 * nv + 1 + imin];
				facets[fr1].add(what, pos);

				nnv = facets[fr1].numVertices;
				what = fl1;
				pos += nnv + 1;
				facets[fr1].add(what, pos);

				what = -1;
				pos += nnv;
				facets[fr1].add(what, pos);

				facets[fr1].indVertices[facets[fr1].numVertices] =
						facets[fr1].indVertices[0];

				//Исправить грань fl1
				nnv = facets[fl1].numVertices;
				pos = index[2 * nv + 1 + irep];
				pos = (nnv + pos - 1) % nnv;
				facets[fl1].indVertices[nnv + 1 + pos] = fr1;

				//Удалить imin из главной грани fid0 и исправим грань
				index[irep + nv + 1] = fr1;
				facets[fid0].remove(imin);

				facets[fid0].update_info();
				facets[fr1].update_info();

			}
			else if (!deg_imin_big && deg_irep_big)
			{

				vertices[index[imin]] = v1;

				// Добавить v1 в грань fl2 с информацией

				what = index[imin];
				pos = index[2 * nv + 1 + (nv + irep - 1) % nv];
				facets[fl2].add(what, pos);

				nnv = facets[fl2].numVertices;
				what = fid0;
				pos += nnv + 1;
				facets[fl2].add(what, pos);

				what = -1;
				pos += nnv;
				facets[fl2].add(what, pos);

				pos = index[2 * nv + 1 + (nv + irep - 1) % nv];
				pos = (nnv + pos - 1) % nnv;
				facets[fl2].indVertices[nnv + 1 + pos] = fl1;

				facets[fl2].indVertices[facets[fl2].numVertices] =
						facets[fl2].indVertices[0];

				//Исправить грань fl1
				nnv = facets[fl1].numVertices;
				pos = index[2 * nv + 1 + irep];
				pos = (nnv + pos - 1) % nnv;
				facets[fl1].indVertices[nnv + 1 + pos] = fl2;

				//Удалить irep из главной грани fid0 и исправим грань
				index[irep] = index[imin];
				index[irep + nv + 1] = fr1;
				facets[fid0].remove(imin);

				facets[fid0].update_info();
				facets[fl2].update_info();

			}
			else if (!deg_imin_big && !deg_irep_big)
			{

				vertices[index[irep]] = v1;

				//Удалить вершину imin из грани fl1
				nnv = facets[fl1].numVertices;
				pos = index[2 * nv + 1 + irep];
				pos = (nnv + pos - 1) % nnv;
				facets[fl1].remove(pos);

				//Заменить вершину imin из грани fr1
				pos = index[2 * nv + 1 + imin];
				facets[fr1].indVertices[pos] = index[irep];

				//Удалить imin из главной грани fid0
				//                index[irep] = index[imin];
				index[irep + nv + 1] = fr1;
				facets[fid0].remove(imin);

				facets[fid0].update_info();
				facets[fl1].update_info();
			}
		}
		else
		{
			// Если не нужно объединять вершины
			printf("\t\tВершины объединять не нужно\n");
			vertices[index[imin]] = v1;
		}
	}
	else if ((d2 < d1 && d2 > 0) || (d2 > 0 && d1 <= 0))
	{
		printf("Движение определяет правый сосед вершины\n");
		// Движение определяет правый сосед вершины
		if (ifjoin)
		{
			// Если нужно объединять вершины

			irep = (nv + imin + 1) % nv;
			deg_imin = vertexInfos[index[imin]].get_nf();
			deg_imin_big = deg_imin > 3;
			deg_irep = vertexInfos[index[irep]].get_nf();
			deg_irep_big = deg_irep > 3;
			printf("\tdeg_imin = deg(%d) = %d\n", index[imin], deg_imin);
			printf("\tdeg_irep = deg(%d) = %d\n", index[irep], deg_irep);

			if (deg_imin_big && deg_irep_big)
			{

				// Создать новую вершину с номером ind_new
				ind_new = add_vertex(v2);

				// Добавить вершину v2 в грань fl1 после imin и исправить грань
				what = ind_new;
				pos = index[2 * nv + 1 + (nv + imin - 1) % nv];
				facets[fl1].add(what, pos);

				nnv = facets[fl1].numVertices;
				what = fid0;
				pos += nnv + 1;
				facets[fl1].add(what, pos);

				what = -1;
				pos += nnv;
				facets[fl1].add(what, pos);

				pos = index[2 * nv + 1 + (nv + imin - 1) % nv];
				pos = (nnv + pos - 1) % nv;
				facets[fl1].indVertices[nnv + 1 + pos] = fr1;

				// Добавить вершину v2 в грань fr1 до imin и исправить грань
				what = ind_new;
				pos = index[2 * nv + 1 + imin];
				facets[fr1].add(what, pos);

				nnv = facets[fr1].numVertices;
				what = fl1;
				pos += nnv + 1;
				facets[fr1].add(what, pos);

				what = -1;
				pos += nnv;
				facets[fr1].add(what, pos);

				pos = index[2 * nv + 1 + imin];
				pos = (nnv + pos - 1) % nnv;
				facets[fr1].indVertices[nnv + 1 + pos] = fr2;

				//Добавить вершину v2 в грань fr2 до irep и исправить грань
				what = ind_new;
				pos = index[2 * nv + 1 + irep];
				facets[fr2].add(what, pos);

				nnv = facets[fr2].numVertices;
				what = fr1;
				pos += nnv + 1;
				facets[fr2].add(what, pos);

				what = -1;
				pos += nnv;
				facets[fr2].add(what, pos);

				//Удалить imin из главной грани fid0 и заменить irep на v2; и исправить грань
				index[irep] = ind_new;
				facets[fid0].remove(imin);

				facets[fid0].update_info();
				facets[fl1].update_info();
				facets[fr1].update_info();
				facets[fr2].update_info();

			}
			else if (deg_imin_big && !deg_irep_big)
			{

				vertices[index[irep]] = v2;

				//Добавить вершину irep в грань fl1 после imin и исправить грань
				what = index[irep];
				pos = index[2 * nv + 1 + (nv + imin - 1) % nv];
				facets[fl1].add(what, pos);

				nnv = facets[fl1].numVertices;
				what = fid0;
				pos += nnv + 1;
				facets[fl1].add(what, pos);

				what = -1;
				pos += nnv;
				facets[fl1].add(what, pos);

				pos = index[2 * nv + 1 + (nv + imin - 1) % nv];
				pos = (nnv + pos - 1) % nnv;
				facets[fl1].indVertices[nnv + 1 + pos] = fr1;

				// Исправить грань fr1
				nnv = facets[fr1].numVertices;
				pos = index[2 * nv + 1 + imin];
				pos = (nnv + pos - 1) % nnv;
				facets[fr1].indVertices[nnv + 1 + pos] = fl1;

				// Удалить вршину imin из главной грани fid0 и исправить грань
				facets[fid0].remove(imin);

				facets[fid0].update_info();
				facets[fl1].update_info();

			}
			else if (!deg_imin_big && deg_irep_big)
			{

				vertices[index[imin]] = v2;

				//Добавить вершину v2 в грань fr2 до irep и исправить грань
				what = index[imin];
				pos = index[2 * nv + 1 + irep];
				facets[fr2].add(what, pos);

				nnv = facets[fr2].numVertices;
				what = fr1;
				pos += nnv + 1;
				facets[fr2].add(what, pos);

				what = -1;
				pos += nnv;
				facets[fr2].add(what, pos);

				// Исправить грань fr1
				nnv = facets[fr1].numVertices;
				pos = index[2 * nv + 1 + imin];
				pos = (nnv + pos - 1) % nnv;
				facets[fr1].indVertices[nnv + 1 + pos] = fr2;

				// Удалить вершину irep из главной грани fid0 и исправить грань
				index[irep] = index[imin];
				facets[fid0].remove(imin);

				facets[fid0].update_info();
				facets[fr2].update_info();

			}
			else if (!deg_imin_big && !deg_irep_big)
			{

				vertices[index[irep]] = v2;

				//Заменить вершину imin из грани fl1 на вершину irep и исправить грань
				nnv = facets[fl1].numVertices;
				pos = index[2 * nv + 1 + (nv + imin - 1) % nv];
				pos = (nnv + pos - 1) % nnv;
				facets[fl1].indVertices[pos] = index[irep];

				//Удалить вершину imin из грани fr1 и исправить грань
				nnv = facets[fr1].numVertices;
				pos = index[2 * nv + 1 + imin];
				pos = (nnv + pos - 1) % nnv;
				facets[fr1].indVertices[nnv + 1 + pos] = fl1;
				pos = (nnv + pos + 1) % nnv;
				facets[fr1].remove(pos);

				//Удалить вершину imin из главной грани fid0 и исправить грань
				facets[fid0].remove(imin);

				facets[fid0].update_info();
				facets[fr1].update_info();
			}
		}
		else
		{
			// Если не нужно объединять вершины
			printf("\t\tВершины объединять не нужно\n");
			vertices[index[imin]] = v2;
		}
	}

}

//void Polyhedron::join_create_first_facet(int fid0, int fid1) {
//
//    int nv0, nv1, nv, nv_common, *index0, *index1, *index;
//    int *sign_vertex, i, j, k, ndown, ndown_new;
//    int fid;
//
//    
//    //IV). Алгоритм поднятия вершин, лежащих ниже плоскости
//    printf("IV). Алгоритм поднятия вершин, лежащих ниже плоскости\n");
//
//    int i_prev_prev, i_prev, i_next, i_next_next;
//    int f_prev_prev, f_prev, f_next, f_next_next;
//    Vector3d intrsct;
//    Vector3d dir;
//    Vector3d point;
//    Plane plane0;
//    Plane plane1;
//    Plane plane2;
//
//    double min_dist;
//    double dist;
//    double dist_to_plane;
//    double min_coeff;
//    double coeff;
//    double coeff_to_plane;
//    int min_i, i_replace;
//    
//    int tmp0, tmp1, tmp2;
//    int new_number;
//    int v_replace;
//    
//    double plane_dist;
//    bool ifParallel;
//
//    int step = 0;
//    while (ndown > 0) {
//        printf("-------- Шаг %d ---------\n", step++);
//        printf("\tНиже плоскости находятся %d вершин контура\n", ndown);
//        printf("\t1.Находим, по какой вершине нужно шагать, и минимальное расстояние\n");
//        min_i = -1;
//        for (i = 0; i < nv; ++i) {
//            printf("\t\t1.%d. Обработка %d-й вершины : ", i, index[i]);
//            if (sign_vertex[i] == 1) {
//                printf("выше плоскости\n");
//                continue;
//            }
//            printf("ниже плоскости");
//            if (i == 0) {
//                i_prev_prev = nv - 2;
//                i_prev = nv - 1;
//                i_next = 0;
//                i_next_next = 1;
//            } else if (i == 1) {
//                i_prev_prev = nv - 1;
//                i_prev = 0;
//                i_next = 1;
//                i_next_next = 2;
//            } else if (i == nv - 1) {
//                i_prev_prev = nv - 3;
//                i_prev = nv - 2;
//                i_next = nv - 1;
//                i_next_next = 0;
//            } else {
//                i_prev_prev = i - 2;
//                i_prev = i - 1;
//                i_next = i;
//                i_next_next = i + 1;
//            }
//            f_prev_prev = index[nv + 1 + i_prev_prev];
//            f_prev = index[nv + 1 + i_prev];
//            f_next = index[nv + 1 + i_next];
//            f_next_next = index[nv + 1 + i_next_next];
//            printf("\t\t\tЗатрагиваются грани: %d, %d, %d, %d\n", 
//                    f_prev_prev, f_prev, f_next, f_next_next);
//            
//            plane0 = facet[f_prev_prev].plane;
//            plane1 = facet[f_prev].plane;
//            plane2 = facet[f_next].plane;
//
////            printf("plane_%d : (%lf)*x + (%lf)*y + (%lf)*z + (%lf) = 0\n",
////                    f_prev_prev, plane0.norm.x, plane0.norm.y, plane0.norm.z,
////                    plane0.dist);
////            printf("plane_%d : (%lf)*x + (%lf)*y + (%lf)*z + (%lf) = 0\n",
////                    f_next, plane2.norm.x, plane2.norm.y, plane2.norm.z,
////                    plane2.dist);
////            printf("plane : (%lf)*x + (%lf)*y + (%lf)*z + (%lf) = 0\n",
////                    plane.norm.x, plane.norm.y, plane.norm.z,
////                    plane.dist);
//
//            plane0.norm.norm(1.);
//            plane2.norm.norm(1.);
//            if (plane0.norm * plane2.norm < 0.) {
//                plane2.norm *= -1.;
//                plane2.dist *= -1.;
//            }
//            
//            plane_dist = qmod(plane0.norm - plane2.norm);
////            printf("dist_plane(%d, %d) = %lf\n", f_prev_prev, f_next, plane_dist);
//            intersection(facet[f_prev].plane, facet[f_next].plane, plane, intrsct);
//            dist_to_plane = (vertex[index[i]] - intrsct)*plane.norm;
//            dist_to_plane /= sqrt(qmod(plane.norm));
//            dist_to_plane = fabs(dist_to_plane);
//            coeff_to_plane = sqrt(qmod(intrsct - vertex[index[i]]));
//            
////            dist_to_plane = fabs(plane.norm * vertex[index[i]] + plane.dist);
////            dist_to_plane /= sqrt(qmod(plane.norm));
//            printf("\t\t\tdist_to_plane(%d, plane) = %lf\n", index[i], dist_to_plane);
//            
//            if (plane_dist < EPS_PARALL) {
//                ifParallel = true;
//                dist = dist_to_plane;
//                coeff = coeff_to_plane;
//                printf("\t\t\tГрани %d и %d параллельны\n", f_prev_prev, f_next);
//            } else {   
//                ifParallel = false;
//                intersection(plane0, plane1, plane2, intrsct);
//                dist = (vertex[index[i]] - intrsct)*plane.norm;
//                dist /= sqrt(qmod(plane.norm));
//                dist = fabs(dist);
//                coeff = sqrt(qmod(intrsct - vertex[index[i]]));
////                dist = qmod(vertex[index[i]] - intrsct);
////                dist = sqrt(dist);
//                printf("\t\t\tdist = %lf\n", dist);
//            }
//            if (dist_to_plane < dist) {
//                dist = dist_to_plane;
//                coeff = coeff_to_plane;
//            }
//            if ((min_i == -1 || dist < min_dist) && dist > 0.) {
//                min_dist = dist;
//                min_coeff = coeff;
//                min_i = i;
//                i_replace = ifParallel ? min_i : i_prev;
//            }
//
//            plane0 = facet[f_prev].plane;
//            plane1 = facet[f_next].plane;
//            plane2 = facet[f_next_next].plane;
//
////            printf("plane_%d : (%lf)*x + (%lf)*y + (%lf)*z + (%lf) = 0\n",
////                    f_prev, plane0.norm.x, plane0.norm.y, plane0.norm.z,
////                    plane0.dist);
////            printf("plane_%d : (%lf)*x + (%lf)*y + (%lf)*z + (%lf) = 0\n",
////                    f_next_next, plane2.norm.x, plane2.norm.y, plane2.norm.z,
////                    plane2.dist);
////            printf("plane : (%lf)*x + (%lf)*y + (%lf)*z + (%lf) = 0\n",
////                    plane.norm.x, plane.norm.y, plane.norm.z,
////                    plane.dist);
//            
//            plane0.norm.norm(1.);
//            plane2.norm.norm(1.);
//            if (plane0.norm * plane2.norm < 0.) {
//                plane2.norm *= -1.;
//                plane2.dist *= -1.;
//            }
//            
//            plane_dist = qmod(plane0.norm - plane2.norm);
////            printf("dist_plane(%d, %d) = %lf\n", f_prev, f_next_next, plane_dist);
////            dist_to_plane = fabs(plane.norm * vertex[index[i]] + plane.dist);
////            dist_to_plane /= sqrt(qmod(plane.norm));
////            printf("\t\t\tdist_to_plane(%d, plane) = %lf\n", index[i], dist);
//            if (plane_dist < EPS_PARALL) {
//                ifParallel = true;
//                printf("\t\t\tГрани %d и %d параллельны\n", f_prev, f_next_next);
//                dist = dist_to_plane;
//                coeff = coeff_to_plane;
//            } else {   
//                ifParallel = false;
//                intersection(plane0, plane1, plane2, intrsct);
//                dist = (vertex[index[i]] - intrsct)*plane.norm;
//                dist /= sqrt(qmod(plane.norm));
//                dist = fabs(dist);
//                coeff = sqrt(qmod(intrsct - vertex[index[i]]));
////                dist = qmod(vertex[index[i]] - intrsct);
////                dist = sqrt(dist);
//                printf("\t\t\tdist = %lf\n", dist);
//            }
//            if (dist_to_plane < dist) {
//                dist = dist_to_plane;
//                coeff = coeff_to_plane;
//            }
//            if ((min_i == -1 || dist < min_dist) && dist > 0.) {
//                min_dist = dist;
//                min_coeff = coeff;
//                min_i = i;
//                i_replace = ifParallel ? min_i : i_next_next;
//            }
//        }
//        printf("\tИтак:");
//        printf("\t\tШагать нужно по %d вершине\n", min_i);
//        printf("\t\tНа расстояние %lf\n", min_dist);
//        printf("\t\tПричем эта вершина сольётся с вершиной %d\n", i_replace);
////        printf("min_i = %d, min_dist = %lf, i_replace = %d\n",
////                min_i, min_dist, i_replace);
//        printf("\t2. Подъем контура на найденное расстояние\n");
////        printf("\tMoving ");
////        print_vertex(index[min_i]);
//
//        if (min_dist > MAX_MIN_DIST) {
//            printf("\t\tОшибка! Слишком большое расстояние\n");
//            break;
////            i_replace = min_i;
////            dist = fabs(plane.norm * vertex[index[min_i]] + plane.dist);
////            dist /= sqrt(qmod(plane.norm));
////            min_dist = dist;
//        }
//            
//
//        for (i = 0; i < nv; ++i) {
//            printf("\t\t2.%d. Поднимается вершина %d", i, index[i]);
//            if (sign_vertex[i] == 1) {
//                printf("\tОна выше плоскости. Пропускаем.\n");
//                continue;
//            }
//            if (i == min_i) {
//                printf("\tВершину %d обработаем отдельно. Она ключевая.\n", index[i]);
//                continue;
//            }
//            if (i != i_replace) {
//                printf("\tБУДЕМ ОБРАБАТЫВАТЬ ТОЛЬКО ЗАМЕНЯЮЩУЮ ВЕРШИНУ %d\n", index[i_replace]);
//                continue;
//                
//            }
//            if (i == 0) {
//                i_prev = nv - 1;
//                i_next = 0;
//            } else {
//                i_prev = i - 1;
//                i_next = i;
//            }
//            f_prev = index[nv + 1 + i_prev];
//            f_next = index[nv + 1 + i_next];
//            intersection(facet[f_prev].plane, facet[f_next].plane, dir, point);
//            dir.norm(min_coeff);
//
//            // 2 случая:
//            //                tmp0 = index[2 * nv + 1 + i] > 0 ? index[2 * nv + 1 + i] - 1 : facet[f_prev].nv - 1;
//            //                facet[f_prev].get_next_facet(tmp0, tmp1, fid, tmp2);
//            facet[f_prev].find_next_facet2(index[i], fid);
//
//            // Точка сдвигается
//            if (fid == f_next) {
//                printf("\tВершина %d просто сдвигается\n", index[i]);
//                //                    print_vertex(index[i]);
//                vertex[index[i]] += dir;
//                //                    sign_vertex[i] = signum(vertex[index[i]], plane);                    
//                //                    print_vertex(index[i]);
//            }// Добавляется новая точка
//            else {
//                printf("\tСоздаем новую точку для вершины %d\n", index[i]);
//                //                    print_vertex(index[i]);
//                point = vertex[index[i]] + dir;
//                new_number = add_vertex(point);
//                printf("\t\t\tДобавляем вершину %d после %d в грани %d\n", new_number, index[i], f_next);
//                facet[f_prev].add_after_position(index[2 * nv + 1 + i_prev], new_number, f_next);
//                printf("\t\t\tДобавляем вершину %d до %d в грани %d\n", new_number, index[i], f_prev);
//                facet[f_next].add_before_position(index[2 * nv + 1 + i_next], new_number, f_prev);
//                index[i] = new_number;
//                //                    sign_vertex[i] = signum(vertex[index[i]], plane);                    
//                print_vertex(index[i]);
//            }
//            //                if (i == i_replace)
//            //                    v_replace = index[i];
//        }
//        // 3. Отдельно рассмотрим случай выбранной вершины. Эта вершина будет
//        // удалена из списка.
//        // 2011.10.01 : Исключение будет в том случае, если были параллельные 
//        // вершины и номера заменяемой (min_i) и заменяющей (i_replace) точек 
//        // совпадают...
//        printf("\t3. Подъем ключевой вершины %d\n", index[min_i]);
//        v_replace = index[i_replace];
//        i = min_i;
//        if (i == 0) {
//            i_prev = nv - 1;
//            i_next = 0;
//        } else {
//            i_prev = i - 1;
//            i_next = i;
//        }
//        f_prev = index[nv + 1 + i_prev];
//        f_next = index[nv + 1 + i_next];
//
//        // 2 случая:
//        //            facet[f_prev].my_fprint_all(stdout);
//        //            tmp0 = index[2 * nv + 1 + i] > 0 ? index[2 * nv + 1 + i] - 1 : facet[f_prev].nv - 1;
//        //            facet[f_prev].get_next_facet(tmp0, tmp1, fid, tmp2);
//        //!2
//        facet[f_prev].find_next_facet2(index[i], fid);
//                
//        // Та самая исключительная ситуация :
//        if (min_i == i_replace) {
//            intersection(facet[f_prev].plane, facet[f_next].plane, dir, point);
//            dir.norm(min_coeff);
//            if (dir * plane.norm < 0)
//                dir *= -1.;
//            printf("\t\tВершина %d просто сдвигается\n", index[i]);
//            vertex[index[i]] += dir;            
//        }
//        // Точка сдвигается
//        //            printf("f_prev = %d, f_next = %d, fid = %d\n", f_prev, f_next, fid);
//        else if (fid == f_next) {
//            printf("\t\t Вершина %d заменяется вершиной %d в грани %d\n", 
//                    index[i], v_replace, f_prev);
//            facet[f_prev].find_and_replace2(index[i], v_replace);
//            printf("\t\t Вершина %d заменяется вершиной %d в грани %d\n", 
//                    index[i], v_replace, f_next);
//            facet[f_next].find_and_replace2(index[i], v_replace);
//        }// Добавляется новая точка
//        else {
//            printf("\t\tДобавляется новая точка для вершины %d\n", index[i]);
//            //                printf("\tVertex %d is followed by %d in facet %d and %d\n", index[i], v_replace, f_prev, f_next);
//            //                printf("v_replace = %d\n", v_replace);
//            printf("\t\tДобавляем вершину %d после %d в грани %d\n", 
//                    v_replace, index[i], f_next);
//            facet[f_prev].add_after_position(index[2 * nv + 1 + i_prev], v_replace, f_next);
//            printf("\t\tДобавляем вершину %d до %d в грани %d\n", 
//                    v_replace, index[i], f_prev);
//            facet[f_next].add_before_position(index[2 * nv + 1 + i_next], v_replace, f_prev);
//        }
//
//        if (min_i != i_replace) {
//            printf("\t\tУдаляем ключевую вершину %d\n", min_i);
//            for (j = i; j < nv - 1; ++j)
//                index[2 * nv + 1 + j] = index[2 * nv + 2 + j];
//            for (j = i; j < 2 * nv - 2; ++j)
//                index[nv + 1 + j] = index[nv + 2 + j];
//            for (j = i; j < 3 * nv - 2; ++j)
//                index[j] = index[j + 1];
//            //            for (j = i; j < nv - 1; ++j)
//            //                sign_vertex[j] = sign_vertex[j + 1];
//            --nv;
//            if (i == 0)
//                index[nv] = index[0];
//        }
//        ndown_new = 0;
//        for (i = 0; i < nv; ++i) {
//            sign_vertex[i] = signum(vertex[index[i]], plane);
//            if (sign_vertex[i] == -1)
//                ++ndown_new;
//        }
//        printf("\tЗа шаг %d было поднято %d вершин\n", step, ndown - ndown_new);
//        ndown = ndown_new;
//    }
//    printf("----- Алгоритм завершил работу. Ниже плоскости осталось %d вершин-----\n", 
//            ndown);
//
//    //V). Предобработка многогранника после поднятия
//    printf("V). Предобработка многогранника после поднятия");
//    first_facet = Facet(fid0, nv, plane, index, this, true);
////    printf("Cycle 3.\n");
//    facet[fid0] = first_facet;
//    facet[fid0].my_fprint_all(stdout);
//    facet[fid1] = Facet();
//    facet[fid1].my_fprint_all(stdout);
//    preprocess_polyhedron();
//    //    my_fprint(stdout);
//
//    //    clear_unused();
//    printf("VI). Рассечение многогранника плоскостью - ВКЛЮЧЕНО\n");
//    intersect(-plane);
//    printf("VII). Грани %d и %d объединены!\n", fid0, fid1);
//    //    my_fprint(stdout);
//    //    if (index != NULL)    это не нужно, т. к. index = first_facet.index
//    //        delete[] index;   и поэтому удалится деструктором...
//    if (sign_vertex != NULL)
//        delete[] sign_vertex;
//}

void Polyhedron::delete_vertex_polyhedron(int v)
{
	printf("delete_vertex_polyhedron(%d)\n", v);
	for (int i = 0; i < numFacets; ++i)
		facets[i].delete_vertex(v);
}

int Polyhedron::add_vertex(Vector3d& vec)
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

void Polyhedron::set_vertex(int position, Vector3d vec)
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

void Polyhedron::print_vertex(int i)
{
	printf("vertex %d : (%lf , %lf , %lf)\n", i, vertices[i].x, vertices[i].y,
			vertices[i].z);
}

void Polyhedron::clear_unused()
{
	int i, numf_used, numv_used, nf;
	int *index_facet, *index_vertex;

	printf("\tvertex analyse\n");

	index_vertex = new int[numVertices];
	numv_used = 0;
	for (i = 0; i < numVertices; ++i)
	{
		nf = vertexInfos[i].get_nf();
		printf("\tanalyze vertex %d, nf = %d\n", i, nf);
		if (nf > 0)
		{
			index_vertex[numv_used++] = i;
		}
	}

	printf("index_vertex : ");
	for (i = 0; i < numv_used; ++i)
		printf("%d ", index_vertex[i]);
	printf("\n");

	for (i = 0; i < numv_used; ++i)
	{
		if (index_vertex[i] != i)
		{
			printf("\treplacing vertex %d by vertex %d\n", i, index_vertex[i]);
			find_and_replace_vertex(index_vertex[i], i);
			vertices[i] = vertices[index_vertex[i]];
			vertexInfos[i] = vertexInfos[index_vertex[i]];
		}
	}
	numVertices = numv_used;

	index_facet = new int[numFacets];
	numf_used = 0;
	for (i = 0; i < numFacets; ++i)
	{
		if (facets[i].numVertices > 0)
		{
			index_facet[numf_used++] = i;
		}
	}

	printf("index_facet : ");
	for (i = 0; i < numf_used; ++i)
		printf("%d ", index_facet[i]);
	printf("\n");

	for (i = 0; i < numf_used; ++i)
	{
		if (index_facet[i] != i)
		{
			printf("\treplacing facet %d by facet %d\n", i, index_facet[i]);
			find_and_replace_facet(index_facet[i], i);
			facets[i] = facets[index_facet[i]];
		}
	}
	numFacets = numf_used;

	//    my_fprint(stdout);
	preprocessAdjacency();

	if (index_vertex != NULL)
		delete[] index_vertex;
	if (index_facet != NULL)
		delete[] index_facet;
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
