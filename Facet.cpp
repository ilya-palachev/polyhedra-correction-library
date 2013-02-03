#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Facet.h"

#define DEFAULT_NV 1000

Facet::Facet():
		index(new int[3*DEFAULT_NV + 1]),
		nv(DEFAULT_NV),
		plane()
{
	rgb[0] = 100;
	rgb[1] = 100;
	rgb[2] = 100;
}

Facet::Facet(const int* index1, const int nv1, const Plane plane1):
		index(),
		nv(nv1),
		plane(plane1)
{
	rgb[0] = 100;
	rgb[1] = 100;
	rgb[2] = 100;

	if(!index1)
	{
		fprintf(stderr, "Facet::Facet. Error. index1 = NULL\n");
	}
	if(nv1 < 3)
	{
		fprintf(stderr, "Facet::Facet. Error. nv1 < 3\n");
	}
	index = new int[3*nv + 1];
	for(int i = 0; i < 3*nv + 1; ++i)
		index[i] = index1[i];
}

Facet& Facet::operator =(const Facet& facet1)
{
	int i;

	rgb[0] = facet1.rgb[0];
	rgb[1] = facet1.rgb[1];
	rgb[2] = facet1.rgb[2];

	nv = facet1.nv;
	if(index) delete[] index;
	index = new int[3*nv + 1];
	for(i = 0; i < 3*nv + 1; ++i)
		index[i] = facet1.index[i];
	plane = facet1.plane;

	return *this;
}

Facet::~Facet()
{
	if(index) delete[] index;
}

void Facet::print_v(int facet_id)
{
	int i;
	printf("facet %d: ", facet_id);
	for(i = 0; i < nv; ++i)
		printf("%d ", index[i]);
	printf("\n");
}
