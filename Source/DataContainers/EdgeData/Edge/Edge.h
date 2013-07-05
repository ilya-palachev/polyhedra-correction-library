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
	int id;
	int v0;
	int v1;
	int f0;
	int f1;

	list<EdgeContourAssociation> assocList;

	// Edge.cpp :
	Edge();
	Edge(int id_orig, int v0_orig, int v1_orig, int f0_orig, int f1_orig);

	Edge(int v0_orig, int v1_orig, int f0_orig, int f1_orig);

	Edge(int id_orig, int v0_orig, int v1_orig, int f0_orig, int f1_orig,
			list<EdgeContourAssociation> accocList_orig);

	Edge(int v0_orig, int v1_orig, int f0_orig, int f1_orig,
			list<EdgeContourAssociation> accocList_orig);

	Edge(const Edge& orig);
	~Edge();

	Edge& operator =(const Edge& orig);

	bool operator ==(const Edge& e);
	bool operator !=(const Edge& e);

	//Edge_io.cpp :
	void my_fprint(FILE* file);

private:

};

#endif	/* EDGE_H */

