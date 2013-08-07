/*
 * EdgeData.h
 *
 *  Created on: 17.05.2013
 *      Author: iliya
 */

#ifndef EDGEDATA_H_
#define EDGEDATA_H_

typedef set<Edge, EdgeComparison> EdgeSet;
typedef set<Edge, EdgeComparison>::iterator EdgeSetIterator;

class EdgeData
{
public:
	EdgeSet edges;
	int numEdges;

	EdgeData();
	EdgeData(int numEdgesMax);
	~EdgeData();

	EdgeSetIterator findEdge(int v0, int v1);
	void addEdge(int v0, int v1, int f0, int f1);
	void addEdge(int v0, int v1, int f0);

	bool operator ==(EdgeData& e);
	bool operator !=(EdgeData& e);

};

#endif /* EDGEDATA_H_ */
