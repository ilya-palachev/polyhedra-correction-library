#include "PolyhedraCorrectionLibrary.h"

int Polyhedron::clasterisation(double p)
{

	int i, j, nv, *index, fid;
	double *dist, sin_alpha, alpha;
	bool *inc;

	int *claster, nclaster;

	Vector3d n0, n1;

	dist = new double[numFacets * numFacets];
	inc = new bool[numFacets * numFacets];
	claster = new int[numFacets];

	for (i = 0; i < numFacets * numFacets; ++i)
		dist[i] = 100.;

	for (i = 0; i < numFacets; ++i)
	{
		nv = facets[i].numVertices;
		index = facets[i].indVertices;
		n0 = facets[i].plane.norm;
		for (j = 0; j < nv; ++j)
		{
			fid = index[nv + 1 + j];
			n1 = facets[fid].plane.norm;
			sin_alpha = sqrt(qmod(n0 % n1) / (qmod(n0) * qmod(n1)));
			alpha = asin(sin_alpha);
			alpha = fabs(alpha);
			if (i * numFacets + fid >= numFacets * numFacets)
				printf("Warning!!!\n");
			dist[i * numFacets + fid] = alpha;
		}
	}

	for (i = 0; i < numFacets * numFacets; ++i)
	{
		if (dist[i] < p)
			inc[i] = true;
		else
			inc[i] = false;
	}

	for (i = 0; i < numFacets; ++i)
		claster[i] = i;

	nclaster = numFacets;
	for (i = 0; i < numFacets; ++i)
	{
		for (j = 0; j < i; ++j)
		{
			if (inc[i * numFacets + j])
			{
				claster[i] = claster[j];
				--nclaster;
				break;
			}
		}
	}

	if (dist != NULL)
		delete[] dist;

	if (inc != NULL)
		delete[] inc;

	if (claster != NULL)
		delete[] claster;

	return nclaster;
}

class SortedSetOfPairs
{
public:
	int num;
	int len;

	int* ii;
	int* jj;

	SortedSetOfPairs(int Len);
	~SortedSetOfPairs();
	void add(int i, int j);
	void print();
};

SortedSetOfPairs::SortedSetOfPairs(int Len)
{
	len = Len;
	num = 0;
	ii = new int[Len];
	jj = new int[Len];
}

SortedSetOfPairs::~SortedSetOfPairs()
{
	if (ii != NULL)
		delete[] ii;
	if (jj != NULL)
		delete[] jj;
	ii = NULL;
	jj = NULL;
}

void SortedSetOfPairs::add(int i, int j)
{

	if (num == len)
	{
		printf("Error. OVERFLOW in SortedSetOfPairs\n");
		return;
	}

	int first = 0; // Первый элемент в массиве
	int last = num; // Последний элемент в массиве

	while (first < last)
	{
		int mid = (first + last) / 2;
		if (i < ii[mid] || (i == ii[mid] && j < jj[mid]))
		{
			last = mid;
		}
		else
		{
			first = mid + 1;
		}
	}

//    if (ii[last] == i && jj[last] == j)
//        return;
//    if (ii[last + 1] == i && jj[last + 1] == j)
//        return;
	if (last > 0 && ii[last - 1] == i && jj[last - 1] == j)
		return;
	insert_int(ii, num, last, i);
	insert_int(jj, num, last, j);
	++num;
}

void SortedSetOfPairs::print()
{
	int i;
	for (i = 0; i < num; ++i)
	{
		printf("ssop[%d] = \t(%d, \t%d)\n", i, ii[i], jj[i]);
	}
}

void fire(int l, int* A, int* claster, SortedSetOfPairs& ssop, int depth)
{

	int k, i;
	for (i = 0; i < depth; ++i)
		printf(" ");
	printf("fire %d\n", l);

	for (k = A[l]; k < A[l + 1]; ++k)
	{

		i = ssop.jj[k];

		if (claster[i] != claster[l])
		{
			claster[i] = claster[l];
			fire(i, A, claster, ssop, depth + 1);
		}
	}
}

void Polyhedron::clasterisation2(double p)
{

	int i, j, nv, *index, fid;
	double sin_alpha, alpha;

	int *claster;
	int *A;

	Vector3d n0, n1;

	claster = new int[numFacets];
	A = new int[numFacets];

	for (i = 0; i < numFacets; ++i)
	{
		A[i] = 0;
	}
	SortedSetOfPairs ssop(numFacets * numFacets);

	for (i = 0; i < numFacets; ++i)
	{
		nv = facets[i].numVertices;
		index = facets[i].indVertices;
		n0 = facets[i].plane.norm;
		for (j = 0; j < nv; ++j)
		{
			fid = index[nv + 1 + j];
			n1 = facets[fid].plane.norm;
			sin_alpha = sqrt(qmod(n0 % n1) / (qmod(n0) * qmod(n1)));
			alpha = asin(sin_alpha);
			alpha = fabs(alpha);
			if (alpha < p)
			{
				ssop.add(i, fid);
				ssop.add(fid, i);
				++A[i];
			}
		}

	}

	for (i = 0; i < numFacets - 1; ++i)
	{
		A[i + 1] += A[i];
	}

	for (i = numFacets - 1; i > 0; --i)
	{
		A[i] = A[i - 1];
	}
	A[0] = 0;

	ssop.print();
	for (i = 0; i < numFacets; ++i)
	{
		printf("A[%d] = %d\n", i, A[i]);
	}
//    return;

	for (i = 0; i < numFacets; ++i)
		claster[i] = i;

	int l = 0;
	while (l < numFacets - 1)
	{
		while (claster[l] != l)
			++l;
		printf("begin new step l = %d\n", l);
		fire(l, A, claster, ssop, 0);
		++l;
	}

	for (i = 0; i < numFacets; ++i)
	{
		if (claster[i] != i)
			printf("claster[%d] = %d\n", i, claster[i]);
	}

	if (claster != NULL)
		delete[] claster;

	if (A != NULL)
		delete[] A;

}
