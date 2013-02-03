#ifndef VERTEXINFO_H
#define VERTEXINFO_H

class VertexInfo
{
//private:
public:
	int* index;
	int nf;

public:
	VertexInfo();
	VertexInfo(const int* index1, const int nf1);
	VertexInfo& operator=(const VertexInfo& vertexinfo1);
	~VertexInfo();
};

#endif // VERTEXINFO_H
