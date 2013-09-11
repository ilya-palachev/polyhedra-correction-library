/*
 * VertexGrouper.h
 *
 *  Created on: 21.06.2013
 *      Author: ilya
 */

#ifndef VERTEXGROUPER_H_
#define VERTEXGROUPER_H_

#include "PCorrector.h"

class VertexGrouper: public PCorrector
{
private:
	int idSavedVertex;

	int groupInner();
	int groupInnerFacet(int fid, double* A, double* b,
			Vector3d* vertex_old);
	int grouptInnerPair(int fid, int id0, int id1, int id2,
			int id3, double* A, double* b);
public:
	VertexGrouper();
	VertexGrouper(shared_ptr<Polyhedron> p);
	VertexGrouper(Polyhedron* p);
	~VertexGrouper();
	int run(int _id);
};

#endif /* VERTEXGROUPER_H_ */
