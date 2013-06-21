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
	int id;

	int join_points_inner();
	int join_points_inner_facet(int fid, double* A, double* b,
			Vector3d* vertex_old);
	int join_points_inner_pair(int fid, int id0, int id1, int id2,
			int id3, double* A, double* b);
public:
	VertexGrouper();
	VertexGrouper(Polyhedron* p);
	~VertexGrouper();
	int join_points(int _id);
};

#endif /* VERTEXGROUPER_H_ */
