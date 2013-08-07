/* 
 * File:   Edge.h
 * Author: ilya
 *
 * Created on 19 Ноябрь 2012 г., 8:17
 */
#ifndef EDGE_H
#define	EDGE_H

class Edge
{
public:
	mutable int id;
	mutable int v0;
	mutable int v1;
	mutable int f0;
	mutable int f1;

	mutable list<EdgeContourAssociation> assocList;

	// Edge.cpp :
	Edge();

	Edge(int v0_orig, int v1_orig);

	Edge(int id_orig, int v0_orig, int v1_orig, int f0_orig, int f1_orig);

	Edge(int v0_orig, int v1_orig, int f0_orig, int f1_orig);

	Edge(int id_orig, int v0_orig, int v1_orig, int f0_orig, int f1_orig,
			list<EdgeContourAssociation> accocList_orig);

	Edge(int v0_orig, int v1_orig, int f0_orig, int f1_orig,
			list<EdgeContourAssociation> accocList_orig);

	Edge(const Edge& orig);
	~Edge();

	Edge& operator =(const Edge& orig);

	bool operator ==(const Edge& e) const;
	bool operator !=(const Edge& e) const;

	//Edge_io.cpp :
	void my_fprint(FILE* file) const;

private:

};

class EdgeComparison
{
public:
	inline bool operator() (Edge edge0, Edge edge1)
	{
		return edge0.v0 < edge1.v0 ||
				(edge0.v0 == edge1.v0 && edge0.v1 < edge1.v1);
	}
};

#endif	/* EDGE_H */

