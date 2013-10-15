/* 
 * Copyright (c) 2009-2013 Ilya Palachev <iliyapalachev@gmail.com>
 * 
 * This file is part of Polyhedra Correction Library.
 *
 * Polyhedra Correction Library is free software: you can redistribute 
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Polyhedra Correction Library is distributed in the hope that it will 
 * be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
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
