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
private:
	Edge* edges;
	int numEdges;
public:
	EdgeData();
	~EdgeData();
	inline Edge& operator[] (const int i)
	{
		return edges[i];
	}
	inline int getNumEdges()
	{
		return numEdges;
	}
};

#endif /* EDGEDATA_H_ */
