/*
 * Coalescer.cpp
 *
 *  Created on: 19.06.2013
 *      Author: ilya
 */

#include "PolyhedraCorrectionLibrary.h"

#define EPS_PARALL 1e-16

Coalescer::Coalescer() :
		PCorrector(),
		plane(),
		coalescedFacet()
{
}

Coalescer::Coalescer(Polyhedron* p) :
		PCorrector(p),
		plane(),
		coalescedFacet()
{
}

Coalescer::~Coalescer()
{
}

void Coalescer::run(int fid0, int fid1)
{
	int nv;

	if (fid0 == fid1)
	{
		ERROR_PRINT("Error. Cannot coalesce facet with itself.");
		return;
	}

	// I ). Составление списка вершин
	printf("I ). Составление списка вершин\n");
	nv = buildIndex(fid0, fid1);
	if (nv == -1)
		return;

	// II ). Вычисление средней плоскости
	printf("II ). Вычисление средней плоскости\n");
	calculatePlane(fid0, fid1);
	coalescedFacet.plane = plane;

	// III ). Дополнительная предобработка многогранника
	printf("III ). Дополнительная предобработка многогранника\n");
	polyhedron->facets[fid0] = coalescedFacet;
	polyhedron->facets[fid1] = Facet();
	polyhedron->preprocessAdjacency();

	// IV ). Алгортм поднятия вершин, лежащих ниже плоскости
	printf("IV ). Алгортм поднятия вершин, лежащих ниже плоскости\n");
	rise(fid0);

	// V ). Предобработка многогранника после поднятия
	printf("V ). Предобработка многогранника после поднятия\n");
	polyhedron->preprocessAdjacency();

	//     VI). Рассечение многогранника плоскостью
	printf("VI). Рассечение многогранника плоскостью\n");
	polyhedron->intersectJoinMode(-plane, fid0);

	polyhedron->test_consections(true);

	printf("=====================================================\n");
	printf("=========    Грани %d и %d объединены !!!   =========\n", fid0,
			fid1);
	printf("=====================================================\n");
}

void Coalescer::run(int n, int* fid)
{

	int i;
	int nv;
	Plane plane;
	Facet join_facet;
	int fid0 = fid[0];

	// I ). Составление списка вершин
	printf("I ). Составление списка вершин\n");
	nv = buildIndex(n, fid);
	if (nv == -1)
		return;

	// II ). Вычисление средней плоскости
	printf("II ). Вычисление средней плоскости\n");
	calculatePlane(n, fid);
	polyhedron->facets[fid[0]].plane = plane;

	// III ). Дополнительная предобработка многогранника
	printf("III ). Дополнительная предобработка многогранника\n");
	polyhedron->preprocessAdjacency();
	polyhedron->facets[fid[0]].my_fprint_all(stdout);

	// IV ). Алгортм поднятия вершин, лежащих ниже плоскости
	printf("IV ). Алгортм поднятия вершин, лежащих ниже плоскости\n");
	rise(fid0);

	// V ). Предобработка многогранника после поднятия
	printf("V ). Предобработка многогранника после поднятия\n");
	polyhedron->preprocessAdjacency();

	//     VI). Рассечение многогранника плоскостью
//    printf("VI). Рассечение многогранника плоскостью\n");
//    intersect_j(-plane, fid0);

	polyhedron->test_consections(true);

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


int Coalescer::buildIndex(int fid0, int fid1)
{
	int nv;

	int i, j;
	int *index, *index0, *index1, nv0, nv1;
	bool *is, *del;

	nv0 = polyhedron->facets[fid0].numVertices;
	nv1 = polyhedron->facets[fid1].numVertices;
	nv = nv0 + nv1;
	index = new int[3 * nv + 1];
	index0 = polyhedron->facets[fid0].indVertices;
	index1 = polyhedron->facets[fid1].indVertices;

	// 1. Найдем общую вершину граней
	for (i = 0; i < nv1; ++i)
		if (polyhedron->facets[fid0].find_vertex(index1[i]) != -1)
			break;
	if (i == nv1)
	{
		printf(
				"join_facets : Error. Facets %d and %d have no common vertexes\n",
				fid0, fid1);
		nv = -1;
		return 0;
	}

	// 2. Найдем последнюю против часовой стрелки (для 1-й грани) общую точку граней
	i = polyhedron->facets[fid0].find_vertex(index1[i]);
	while (polyhedron->facets[fid1].find_vertex(index0[i]) != -1)
	{
		i = (nv0 + i + 1) % nv0;
	}

	// 3. Если эта точка висячая, то не добавляем ее
	i = (nv0 + i - 1) % nv0;
	j = 0;
	if (polyhedron->vertexInfos[index0[i]].get_nf() > 3)
		index[j++] = index0[i];

	// 4. Собираем все точки 1-й грани против часовой стрелки,
	//    пока не наткнемся на 2-ю грань
	i = (nv0 + i + 1) % nv0;
	while (polyhedron->facets[fid1].find_vertex(index0[i]) == -1)
	{
		index[j++] = index0[i];
		i = (nv0 + i + 1) % nv0;
	}

	// 5. Если крайняя точка не висячая, то добавляем ее
	//    i = (nv0 + i - 1) % nv0;
	if (polyhedron->vertexInfos[index0[i]].get_nf() > 3)
		index[j++] = index0[i];

	// 6. Собираем все точки 2-й грани против часовой стрелки,
	// пока не наткнемся на 1-ю грань
	i = polyhedron->facets[fid1].find_vertex(index0[i]);
	i = (nv1 + i + 1) % nv1;
	while (polyhedron->facets[fid0].find_vertex(index1[i]) == -1)
	{
		index[j++] = index1[i];
		i = (nv1 + i + 1) % nv1;
	}
	nv = j;

	// 7. Общие и висячие вершины объединяемых граней удаляются из
	//    многогранника
	is = new bool[polyhedron->numVertices];
	del = new bool[polyhedron->numVertices];
	for (i = 0; i < polyhedron->numVertices; ++i)
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
	for (i = 0; i < polyhedron->numVertices; ++i)
		if (del[i])
			polyhedron->delete_vertex_polyhedron(i);

	// 8. По построенному списку создается грань
	coalescedFacet = Facet(fid0, nv, plane, index, this, true);

	if (is != NULL)
		delete[] is;
	if (del != NULL)
		delete[] del;
	if (index != NULL)
		delete[] index;

	return nv;
}

int Coalescer::buildIndex(int n, int* fid)
{
	int nv;

	int i, j, *index, *nfind, nnv, nv_safe;
	int v_first, v;
	int i_next;
	bool* del;

	nv_safe = 0;
	for (i = 0; i < n; ++i)
	{
		nv_safe += polyhedron->facets[fid[i]].numVertices;
	}
	index = new int[3 * nv_safe + 1];
	nfind = new int[polyhedron->numVertices];
	for (i = 0; i < polyhedron->numVertices; ++i)
	{
		nfind[i] = 0;
	}
	for (i = 0; i < n; ++i)
	{
		for (j = 0; j < polyhedron->facets[fid[i]].numVertices; ++j)
		{
			++(nfind[polyhedron->facets[fid[i]].indVertices[j]]);
		}
	}

	for (i = 0; i < n; ++i)
	{
		printf("facet  fid[%d] = %d : \n\n", i, fid[i]);
		nnv = polyhedron->facets[fid[i]].numVertices;
		for (j = 0; j < nnv; ++j)
		{
			printf("\t%d", polyhedron->facets[fid[i]].indVertices[j]);
		}
		printf("\n");
		for (j = 0; j < nnv; ++j)
		{
			printf("\t%d", nfindpolyhedron->[facets[fid[i]].indVertices[j]]);
		}
		printf("\n\n");
		polyhedron->facets[fid[i]].my_fprint_all(stdout);
	}

	nv = 0;
	j = 0;
	nnv = polyhedron->facets[fid[0]].numVertices;
	while (nfind[polyhedron->facets[fid[0]].indVertices[j]] > 1)
	{
		j = (nnv + j + 1) % nnv;
	}
	v_first = polyhedron->facets[fid[0]].indVertices[j];

	i = 0;
	v = v_first;
	printf("v = %d\n", v);
	printf("New index : ");
	do
	{
		printf("\n next facet %d\n", fid[i]);
		nnv = polyhedron->facets[fid[i]].numVertices;
		while (nfind[polyhedron->facets[fid[i]].indVertices[j]] == 1)
		{
			if (polyhedron->facets[fid[i]].indVertices[j] == v_first && nv > 0)
			{
				break;
			}
			index[nv] = polyhedron->facets[fid[i]].indVertices[j];

			printf("%d ", index[nv]);
			++nv;
			j = (nnv + j + 1) % nnv;
		}
		v = polyhedron->facets[fid[i]].indVertices[j];
		index[nv] = polyhedron->facets[fid[i]].indVertices[j];
		printf("%d\n", index[nv]);
		++nv;
		if (v == v_first)
			break;
		for (i = 0; i < n; ++i)
		{
			j = polyhedron->facets[fid[i]].find_vertex(v);
			printf("\tVertex %d was found in facet %d at position %d\n", v,
					fid[i], j);
			if (j != -1)
			{
				nnv = polyhedron->facets[fid[i]].numVertices;
				j = (nnv + j + 1) % nnv;
				printf("\tnfind[%d] = %d\n", polyhedron->facets[fid[i]].indVertices[j],
						nfind[polyhedron->facets[fid[i]].indVertices[j]]);
				if (nfind[polyhedron->facets[fid[i]].indVertices[j]] == 1)
				{
					break;
				}
			}
		}

		v = polyhedron->facets[fid[i]].indVertices[j];
		printf("v = %d\n", v);
	} while (v != v_first);
	printf("\n\n");
	--nv;

	coalescedFacet = Facet(fid[0], nv, polyhedron->facets[fid[0]].plane, index, polyhedron, true);

	polyhedron->facets[fid[0]] = coalescedFacet;
	for (i = 1; i < n; ++i)
	{
		polyhedron->facets[fid[i]].numVertices = 0;
		printf("Внимание! Грань %d пуста\n", fid[i]);
	}
	polyhedron->facets[fid[0]].my_fprint_all(stdout);

	polyhedron->preprocessAdjacency();
	printf("------End of preprocess_polyhedron...------\n");
	index = polyhedron->facets[fid[0]].indVertices;
	del = new bool[polyhedron->numVertices];

	for (i = 0; i < polyhedron->numVertices; ++i)
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
	for (i = 0; i < polyhedron->numVertices; ++i)
	{
		if (del[i])
		{
			polyhedron->delete_vertex_polyhedron(i);
		}
	}

	if (del != NULL)
		delete[] del;

	return nv;
}

void Coalescer::calculatePlane(int fid0, int fid1)
{
	int *index, nv;

	coalescedFacet.my_fprint_all(stdout);

	index = coalescedFacet.indVertices;
	nv = coalescedFacet.numVertices;

	polyhedron->list_squares_method(nv, index, &plane);

	//Проверка, что нормаль построенной плокости направлена извне многогранника
	if (plane.norm * polyhedron->facets[fid0].plane.norm < 0
			&& plane.norm * polyhedron->facets[fid1].plane.norm < 0)
	{
		plane.norm *= -1.;
		plane.dist *= -1.;
	}
	printf("Calculated plane: (%lf)x  +  (%lf)y  + (%lf)z  +  (%lf)  =  0\n",
			plane.norm.x, plane.norm.y, plane.norm.z, plane.dist);
}

void Coalescer::calculatePlane(int n, int* fid)
{

	int *index, nv;
	int ninvert, i;

	coalescedFacet.my_fprint_all(stdout);

	index = coalescedFacet.indVertices;
	nv = coalescedFacet.numVertices;

	polyhedron->list_squares_method(nv, index, &plane);

	//Проверка, что нормаль построенной плокости направлена извне многогранника

	ninvert = 0;
	for (i = 0; i < n; ++i)
	{
		ninvert += (plane.norm * polyhedron->facets[fid[i]].plane.norm < 0);
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

void Coalescer::rise(int fid0)
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

	nv = polyhedron->facets[fid0].numVertices;
	index = polyhedron->facets[fid0].indVertices;
	plane = polyhedron->facets[fid0].plane;
	printf(
			"Поднимаем до плоскости: (%lf)x  +  (%lf)y  + (%lf)z  +  (%lf)  =  0\n",
			plane.norm.x, plane.norm.y, plane.norm.z, plane.dist);

	// 1 ). Посчитаем знаки вершин контура и количество вершин ниже грани
	sign_vertex = new int[nv];
	ndown = 0;
	for (i = 0; i < nv; ++i)
	{
		sign = polyhedron->signum(polyhedron->vertices[index[i]], plane);
		printf("\tsignum[%d] = %d, (%.16lf)\n", index[i], sign,
				plane % polyhedron->vertices[index[i]]);
		sign_vertex[i] = sign;
		if (sign_vertex[i] == -1)
			++ndown;
	}

	int ndown_total = 0, nup_total;
	for (i = 0; i < polyhedron->numVertices; ++i)
	{
		sign = polyhedron->signum(polyhedron->vertices[i], plane);
		ndown_total += (sign == -1);
		nup_total = polyhedron->numVertices - ndown_total;
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
		polyhedron->facets[fid0].my_fprint_all(stdout);

		sprintf(file_name_out, "../poly-data-out/multijoin-facets-%d.ply",
				step);
		polyhedron->fprint_ply_scale(1000., file_name_out, "join-facets-rise");
		// 2. 1). Находим, по какой вершине нужно шагать, и минимальное расстояние
		printf(
				"\t2. 1). Находим, по какой вершине нужно шагать, и минимальное расстояние\n");
		imin = riseFind(fid0);

		if (imin == -1)
		{
			printf(
					"join_facets_rise : Ошибка. Не удалось найти вершину (imin = -1)\n");
			return;
		}

		// 2. 2). Шагаем по выбранной вершине
		printf("\t2. 2). Шагаем по выбранной вершине\n");
		polyhedron->facets[fid0].my_fprint_all(stdout);
		risePoint(fid0, imin);

		if (polyhedron->test_structure() > 0)
		{
			printf("Не пройден тест на структуру!\n");
			return;
		}

		++step;
		nv = polyhedron->facets[fid0].numVertices;
		index = polyhedron->facets[fid0].indVertices;
		ndown = 0;
		for (i = 0; i < nv; ++i)
		{
			sign = polyhedron->signum(polyhedron->vertices[index[i]], plane);
			printf("\tsignum[%d] = %d, (%.16lf)\n", index[i], sign,
					plane % polyhedron->vertices[index[i]]);
			if (sign == -1)
				++ndown;
		}
		printf("Used plane: (%lf)x  +  (%lf)y  + (%lf)z  +  (%lf)  =  0\n",
				plane.norm.x, plane.norm.y, plane.norm.z, plane.dist);

		polyhedron->preprocessAdjacency();
	}

	printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	printf("!!!!!!!!     ВЕРШИНЫ ПОДНЯТЫ          !!!!!!!!!!\n");
	printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	polyhedron->facets[fid0].my_fprint_all(stdout);

	if (sign_vertex != NULL)
		delete[] sign_vertex;
	if (file_name_out != NULL)
		delete[] file_name_out;
}

int Coalescer::riseFind(int fid0)
{
	int imin;

	int i;
	int nv, *index;
	int fr1, fr2;

	double d, dmin;
	int pos, tmp;

	Plane plane;

	nv = polyhedron->facets[fid0].numVertices;
	index = polyhedron->facets[fid0].indVertices;
	plane = polyhedron->facets[fid0].plane;

	printf("Предварительный анализ вершин: ");
	for (i = 0; i < nv; ++i)
	{
		printf("\t\t2. 1. %d ). Предварительный анализ %d-й вершины : ", i,
				index[i]);
		if (polyhedron->signum(polyhedron->vertices[index[i]], plane) != -1)
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
		if (polyhedron->facets[fr1].numVertices == 3)
		{
			polyhedron->facets[fr1].my_fprint(stdout);
			pos = index[2 * nv + 1 + i];
			pos = (pos + 1) % 3;
			tmp = index[i];
			printf(" - Вместо вершины %d  в главной грани пишем вершину %d\n",
					index[i], polyhedron->facets[fr1].indVertices[pos]);
			index[i] = polyhedron->facets[fr1].indVertices[pos];
			polyhedron->delete_vertex_polyhedron(tmp);
			polyhedron->facets[fr1] = Facet();
			if (polyhedron->vertexInfos[index[i + 1]].numFacets == 3)
			{
				polyhedron->delete_vertex_polyhedron(index[i + 1]);
			}
			polyhedron->preprocessAdjacency();
			--i;
			polyhedron->facets[fid0].my_fprint_all(stdout);
			polyhedron->facets[fr2].my_fprint_all(stdout);
//            continue;
		}
		//конец написанного 2012-03-31
		printf("\n");
	}

	imin = -1;
	for (i = 0; i < nv; ++i)
	{
		printf("\t\t2. 1. %d ). Обработка %d-й вершины : ", i, index[i]);
		if (polyhedron->signum(polyhedron->vertices[index[i]], plane) != -1)
		{
			printf("выше плоскости или на плоскости\n");
			continue;
		}
		printf("ниже плоскости");
		//        printf("\n");

		d = riseFindStep(fid0, i);

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

	return imin;
}

double Coalescer::riseFindStep(int fid0, int i)
{
	double d;

	int nv, *index;
	int fl2, fl1, fr1, fr2;

	double d1, d2;

	Plane plane;
	Plane pl2, pl1, pr1, pr2;
	Vector3d v1, v2;

	nv = polyhedron->facets[fid0].numVertices;
	index = polyhedron->facets[fid0].indVertices;
	plane = polyhedron->facets[fid0].plane;

	// Номера соседних с контуром граней вблизи рассматриваемой точки:
	fl2 = index[nv + 1 + (nv + i - 2) % nv];
	fl1 = index[nv + 1 + (nv + i - 1) % nv];
	fr1 = index[nv + 1 + (nv + i) % nv];
	fr2 = index[nv + 1 + (nv + i + 1) % nv];

	pl2 = polyhedron->facets[fl2].plane;
	pl1 = polyhedron->facets[fl1].plane;
	pr1 = polyhedron->facets[fr1].plane;
	pr2 = polyhedron->facets[fr2].plane;

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
		if (polyhedron->signum(v1, plane) == 1)
		{
			// Если точка пересечения лежит выше плоскости:
			intersection(plane, pl1, pr1, v1);
		}
	}
	// Найдем проекцию перемещения точки на нормаль плоскости:
	d1 = (v1 - polyhedron->vertices[index[i]]) * plane.norm;
	if (polyhedron->facets[fl1].numVertices < 4)
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
		if (polyhedron->signum(v2, plane) == 1)
		{
			// Если точка пересечения лежит выше плоскости:
			intersection(plane, pl1, pr1, v2);
		}
	}
	// Найдем проекцию перемещения точки на нормаль плоскости:
	d2 = (v2 - polyhedron->vertices[index[i]]) * plane.norm;
	if (polyhedron->facets[fr1].numVertices < 4)
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
		polyhedron->facets[fl1].my_fprint_all(stdout);
		polyhedron->facets[fr1].my_fprint_all(stdout);
	}
	return d;
}

void Coalescer::risePoint(int fid0, int imin)
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

	nv = polyhedron->facets[fid0].numVertices;
	index = polyhedron->facets[fid0].indVertices;
	plane = polyhedron->facets[fid0].plane;

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

	pl2 = polyhedron->facets[fl2].plane;
	pl1 = polyhedron->facets[fl1].plane;
	pr1 = polyhedron->facets[fr1].plane;
	pr2 = polyhedron->facets[fr2].plane;

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
	d1 = (v1 - polyhedron->vertices[index[imin]]) * plane.norm;
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
	d2 = (v2 - polyhedron->vertices[index[imin]]) * plane.norm;
	printf("\td2 = %.16lf\n", d2);

	if ((d1 <= d2 && d1 > 0) || (d1 > 0 && d2 <= 0))
	{
		printf("Движение определяет левый сосед вершины\n");
		// Движение определяет левый сосед вершины
		if (ifjoin)
		{
			// Если нужно объединяtellteть вершины

			irep = (nv + imin - 1) % nv;
			deg_imin = polyhedron->vertexInfos[index[imin]].get_nf();
			deg_imin_big = deg_imin > 3;
			deg_irep = polyhedron->vertexInfos[index[irep]].get_nf();
			deg_irep_big = deg_irep > 3;
			printf("\tdeg_imin = deg(%d) = %d\n", index[imin], deg_imin);
			printf("\tdeg_irep = deg(%d) = %d\n", index[irep], deg_irep);

			if (deg_imin_big && deg_irep_big)
			{

				// Создать новую вершину с номером ind_new
				ind_new = polyhedron->appendVertex(v1);

				// Добавить v1 в грань fl2 с информацией

				what = ind_new;
				pos = index[2 * nv + 1 + (nv + irep - 1) % nv];
				polyhedron->facets[fl2].add(what, pos);

				nnv = polyhedron->facets[fl2].numVertices;
				what = fid0;
				pos += nnv + 1;
				polyhedron->facets[fl2].add(what, pos);

				what = -1;
				pos += nnv;
				polyhedron->facets[fl2].add(what, pos);

				pos = index[2 * nv + 1 + (nv + irep - 1) % nv];
				pos = (nnv + pos - 1) % nnv;
				polyhedron->facets[fl2].indVertices[nnv + 1 + pos] = fl1;

				polyhedron->facets[fl2].indVertices[polyhedron->facets[fl2].numVertices] =
						polyhedron->facets[fl2].indVertices[0];

				//Добавить v1 в грань fl1 с информацией

				what = ind_new;
				pos = index[2 * nv + 1 + irep];
				polyhedron->facets[fl1].add(what, pos);

				nnv = polyhedron->facets[fl1].numVertices;
				what = fl2;
				pos += nnv + 1;
				polyhedron->facets[fl1].add(what, pos);

				what = -1;
				pos += nnv;
				polyhedron->facets[fl1].add(what, pos);

				pos = index[2 * nv + 1 + irep];
				pos = (nnv + pos - 1) % nnv;
				polyhedron->facets[fl1].indVertices[nnv + 1 + pos] = fr1;
				//                printf("===Trying to add %d at position %d in facet %d===\n", fr1, nnv + 1 + pos, )

				polyhedron->facets[fl1].indVertices[polyhedron->facets[fl1].numVertices] =
						polyhedron->facets[fl1].indVertices[0];

				//Добавить v1 в грань fr1 с информацией

				what = ind_new;
				pos = index[2 * nv + 1 + imin];
				polyhedron->facets[fr1].add(what, pos);

				nnv = polyhedron->facets[fr1].numVertices;
				what = fl1;
				pos += nnv + 1;
				polyhedron->facets[fr1].add(what, pos);

				what = -1;
				pos += nnv;
				polyhedron->facets[fr1].add(what, pos);

				polyhedron->facets[fr1].indVertices[polyhedron->facets[fr1].numVertices] =
						polyhedron->facets[fr1].indVertices[0];

				//Заменить irep в грани fid0 на ind_new
				index[irep] = ind_new;
				index[irep + nv + 1] = fr1;
				// Удалить вершину imin из главной грани (fid0) и исправим грань
				polyhedron->facets[fid0].remove(imin); // с информацией!!!

				polyhedron->facets[fid0].update_info();
				polyhedron->facets[fl2].update_info();
				polyhedron->facets[fl1].update_info();
				polyhedron->facets[fr1].update_info();

			}
			else if (deg_imin_big && !deg_irep_big)
			{
				polyhedron->vertices[index[irep]] = v1;

				//Добавить v1 в грань fr1 с информацией

				what = index[irep];
				pos = index[2 * nv + 1 + imin];
				polyhedron->facets[fr1].add(what, pos);

				nnv = polyhedron->facets[fr1].numVertices;
				what = fl1;
				pos += nnv + 1;
				polyhedron->facets[fr1].add(what, pos);

				what = -1;
				pos += nnv;
				polyhedron->facets[fr1].add(what, pos);

				polyhedron->facets[fr1].indVertices[polyhedron->facets[fr1].numVertices] =
						polyhedron->facets[fr1].indVertices[0];

				//Исправить грань fl1
				nnv = polyhedron->facets[fl1].numVertices;
				pos = index[2 * nv + 1 + irep];
				pos = (nnv + pos - 1) % nnv;
				polyhedron->facets[fl1].indVertices[nnv + 1 + pos] = fr1;

				//Удалить imin из главной грани fid0 и исправим грань
				index[irep + nv + 1] = fr1;
				polyhedron->facets[fid0].remove(imin);

				polyhedron->facets[fid0].update_info();
				polyhedron->facets[fr1].update_info();

			}
			else if (!deg_imin_big && deg_irep_big)
			{

				polyhedron->vertices[index[imin]] = v1;

				// Добавить v1 в грань fl2 с информацией

				what = index[imin];
				pos = index[2 * nv + 1 + (nv + irep - 1) % nv];
				polyhedron->facets[fl2].add(what, pos);

				nnv = polyhedron->facets[fl2].numVertices;
				what = fid0;
				pos += nnv + 1;
				polyhedron->facets[fl2].add(what, pos);

				what = -1;
				pos += nnv;
				polyhedron->facets[fl2].add(what, pos);

				pos = index[2 * nv + 1 + (nv + irep - 1) % nv];
				pos = (nnv + pos - 1) % nnv;
				polyhedron->facets[fl2].indVertices[nnv + 1 + pos] = fl1;

				polyhedron->facets[fl2].indVertices[polyhedron->facets[fl2].numVertices] =
						polyhedron->facets[fl2].indVertices[0];

				//Исправить грань fl1
				nnv = polyhedron->facets[fl1].numVertices;
				pos = index[2 * nv + 1 + irep];
				pos = (nnv + pos - 1) % nnv;
				polyhedron->facets[fl1].indVertices[nnv + 1 + pos] = fl2;

				//Удалить irep из главной грани fid0 и исправим грань
				index[irep] = index[imin];
				index[irep + nv + 1] = fr1;
				polyhedron->facets[fid0].remove(imin);

				polyhedron->facets[fid0].update_info();
				polyhedron->facets[fl2].update_info();

			}
			else if (!deg_imin_big && !deg_irep_big)
			{

				polyhedron->vertices[index[irep]] = v1;

				//Удалить вершину imin из грани fl1
				nnv = polyhedron->facets[fl1].numVertices;
				pos = index[2 * nv + 1 + irep];
				pos = (nnv + pos - 1) % nnv;
				polyhedron->facets[fl1].remove(pos);

				//Заменить вершину imin из грани fr1
				pos = index[2 * nv + 1 + imin];
				polyhedron->facets[fr1].indVertices[pos] = index[irep];

				//Удалить imin из главной грани fid0
				//                index[irep] = index[imin];
				index[irep + nv + 1] = fr1;
				polyhedron->facets[fid0].remove(imin);

				polyhedron->facets[fid0].update_info();
				polyhedron->facets[fl1].update_info();
			}
		}
		else
		{
			// Если не нужно объединять вершины
			printf("\t\tВершины объединять не нужно\n");
			polyhedron->vertices[index[imin]] = v1;
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
			deg_imin = polyhedron->vertexInfos[index[imin]].get_nf();
			deg_imin_big = deg_imin > 3;
			deg_irep = polyhedron->vertexInfos[index[irep]].get_nf();
			deg_irep_big = deg_irep > 3;
			printf("\tdeg_imin = deg(%d) = %d\n", index[imin], deg_imin);
			printf("\tdeg_irep = deg(%d) = %d\n", index[irep], deg_irep);

			if (deg_imin_big && deg_irep_big)
			{

				// Создать новую вершину с номером ind_new
				ind_new = polyhedron->appendVertex(v2);

				// Добавить вершину v2 в грань fl1 после imin и исправить грань
				what = ind_new;
				pos = index[2 * nv + 1 + (nv + imin - 1) % nv];
				polyhedron->facets[fl1].add(what, pos);

				nnv = polyhedron->facets[fl1].numVertices;
				what = fid0;
				pos += nnv + 1;
				polyhedron->facets[fl1].add(what, pos);

				what = -1;
				pos += nnv;
				polyhedron->facets[fl1].add(what, pos);

				pos = index[2 * nv + 1 + (nv + imin - 1) % nv];
				pos = (nnv + pos - 1) % nv;
				polyhedron->facets[fl1].indVertices[nnv + 1 + pos] = fr1;

				// Добавить вершину v2 в грань fr1 до imin и исправить грань
				what = ind_new;
				pos = index[2 * nv + 1 + imin];
				polyhedron->facets[fr1].add(what, pos);

				nnv = polyhedron->facets[fr1].numVertices;
				what = fl1;
				pos += nnv + 1;
				polyhedron->facets[fr1].add(what, pos);

				what = -1;
				pos += nnv;
				polyhedron->facets[fr1].add(what, pos);

				pos = index[2 * nv + 1 + imin];
				pos = (nnv + pos - 1) % nnv;
				polyhedron->facets[fr1].indVertices[nnv + 1 + pos] = fr2;

				//Добавить вершину v2 в грань fr2 до irep и исправить грань
				what = ind_new;
				pos = index[2 * nv + 1 + irep];
				polyhedron->facets[fr2].add(what, pos);

				nnv = polyhedron->facets[fr2].numVertices;
				what = fr1;
				pos += nnv + 1;
				polyhedron->facets[fr2].add(what, pos);

				what = -1;
				pos += nnv;
				polyhedron->facets[fr2].add(what, pos);

				//Удалить imin из главной грани fid0 и заменить irep на v2; и исправить грань
				index[irep] = ind_new;
				polyhedron->facets[fid0].remove(imin);

				polyhedron->facets[fid0].update_info();
				polyhedron->facets[fl1].update_info();
				polyhedron->facets[fr1].update_info();
				polyhedron->facets[fr2].update_info();

			}
			else if (deg_imin_big && !deg_irep_big)
			{

				polyhedron->vertices[index[irep]] = v2;

				//Добавить вершину irep в грань fl1 после imin и исправить грань
				what = index[irep];
				pos = index[2 * nv + 1 + (nv + imin - 1) % nv];
				polyhedron->facets[fl1].add(what, pos);

				nnv = polyhedron->facets[fl1].numVertices;
				what = fid0;
				pos += nnv + 1;
				polyhedron->facets[fl1].add(what, pos);

				what = -1;
				pos += nnv;
				polyhedron->facets[fl1].add(what, pos);

				pos = index[2 * nv + 1 + (nv + imin - 1) % nv];
				pos = (nnv + pos - 1) % nnv;
				polyhedron->facets[fl1].indVertices[nnv + 1 + pos] = fr1;

				// Исправить грань fr1
				nnv = polyhedron->facets[fr1].numVertices;
				pos = index[2 * nv + 1 + imin];
				pos = (nnv + pos - 1) % nnv;
				polyhedron->facets[fr1].indVertices[nnv + 1 + pos] = fl1;

				// Удалить вршину imin из главной грани fid0 и исправить грань
				polyhedron->facets[fid0].remove(imin);

				polyhedron->facets[fid0].update_info();
				polyhedron->facets[fl1].update_info();

			}
			else if (!deg_imin_big && deg_irep_big)
			{

				polyhedron->vertices[index[imin]] = v2;

				//Добавить вершину v2 в грань fr2 до irep и исправить грань
				what = index[imin];
				pos = index[2 * nv + 1 + irep];
				polyhedron->facets[fr2].add(what, pos);

				nnv = polyhedron->facets[fr2].numVertices;
				what = fr1;
				pos += nnv + 1;
				polyhedron->facets[fr2].add(what, pos);

				what = -1;
				pos += nnv;
				polyhedron->facets[fr2].add(what, pos);

				// Исправить грань fr1
				nnv = polyhedron->facets[fr1].numVertices;
				pos = index[2 * nv + 1 + imin];
				pos = (nnv + pos - 1) % nnv;
				polyhedron->facets[fr1].indVertices[nnv + 1 + pos] = fr2;

				// Удалить вершину irep из главной грани fid0 и исправить грань
				index[irep] = index[imin];
				polyhedron->facets[fid0].remove(imin);

				polyhedron->facets[fid0].update_info();
				polyhedron->facets[fr2].update_info();

			}
			else if (!deg_imin_big && !deg_irep_big)
			{

				polyhedron->vertices[index[irep]] = v2;

				//Заменить вершину imin из грани fl1 на вершину irep и исправить грань
				nnv = polyhedron->facets[fl1].numVertices;
				pos = index[2 * nv + 1 + (nv + imin - 1) % nv];
				pos = (nnv + pos - 1) % nnv;
				polyhedron->facets[fl1].indVertices[pos] = index[irep];

				//Удалить вершину imin из грани fr1 и исправить грань
				nnv = polyhedron->facets[fr1].numVertices;
				pos = index[2 * nv + 1 + imin];
				pos = (nnv + pos - 1) % nnv;
				polyhedron->facets[fr1].indVertices[nnv + 1 + pos] = fl1;
				pos = (nnv + pos + 1) % nnv;
				polyhedron->facets[fr1].remove(pos);

				//Удалить вершину imin из главной грани fid0 и исправить грань
				polyhedron->facets[fid0].remove(imin);

				polyhedron->facets[fid0].update_info();
				polyhedron->facets[fr1].update_info();
			}
		}
		else
		{
			// Если не нужно объединять вершины
			printf("\t\tВершины объединять не нужно\n");
			polyhedron->vertices[index[imin]] = v2;
		}
	}

}
