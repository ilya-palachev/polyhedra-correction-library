/*
 * EdgeData.h
 *
 *  Created on: 17.05.2013
 *      Author: iliya
 */

#ifndef EDGEDATA_H_
#define EDGEDATA_H_

/*
 *
 */
class EdgeData
{
public:
	list<Edge> edges;
	int numEdges;

	EdgeData();
	EdgeData(int numEdgesMax);
	~EdgeData();

	list<Edge>::iterator findEdge(int v0, int v1);
	void addEdge(int numEdgesMax, int v0, int v1, int f0, int f1);
	void addEdge(int numEdgesMax, int v0, int v1, int f0);

	bool operator ==(EdgeData& e);
	bool operator !=(EdgeData& e);

};

#endif /* EDGEDATA_H_ */
