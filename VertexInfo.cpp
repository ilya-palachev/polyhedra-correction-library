#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "VertexInfo.h"

#define DEFAULT_NF 10

VertexInfo::VertexInfo():
		index(new int[2*DEFAULT_NF + 1]),
		nf(DEFAULT_NF)
{
}

VertexInfo::VertexInfo(const int *index1, const int nf1):
		index(),
		nf(nf1)
{
	if(!index1)
	{
		fprintf(stderr, "VertexInfo::VertexInfo. Error. index1 = NULL\n");
	}
	if(nf1 < 3)
	{
		fprintf(stderr, "VertexInfo::VertexInfo. Error. nf1 < 3\n");
	}
	index = new int[2*nf + 1];
	for(int i = 0; i < 2*nf + 1; ++i)
		index[i] = index1[i];
}

VertexInfo::~VertexInfo()
{
	if(index)
		delete[] index;
}

VertexInfo& VertexInfo::operator =(const VertexInfo& vertexinfo1)
{
	int i;

	nf = vertexinfo1.nf;
	if(index) delete[] index;
	index = new int[2*nf + 1];
	for(i = 0; i < 2*nf + 1; ++i)
		index[i] = vertexinfo1.index[i];

	return *this;
}
