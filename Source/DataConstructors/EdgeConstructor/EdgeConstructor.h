/*
 * EdgeConstructor.h
 *
 *  Created on: 05.06.2013
 *      Author: iliya
 */

#ifndef EDGECONSTRUCTOR_H_
#define EDGECONSTRUCTOR_H_

#include "PolyhedraCorrectionLibrary.h"

/*
 *
 */
class EdgeConstructor : public PDataConstructor
{
private:
	EdgeData* edgeData;

	int findEdge(int v0, int v1);
	void addEdge(int numEdgesMax, int v0, int v1, int f0, int f1);

public:
	EdgeConstructor(Polyhedron* p, EdgeData* e);
	~EdgeConstructor();
	void run();
};

#endif /* EDGECONSTRUCTOR_H_ */
