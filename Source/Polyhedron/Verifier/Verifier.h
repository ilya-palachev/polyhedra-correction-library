/*
 * Copyright (c) 2009-2016 Ilya Palachev <iliyapalachev@gmail.com>
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
 * along with Polyhedra Correction Library.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef VERIFIER_H_
#define VERIFIER_H_

#include <memory>

#include "Vector3d.h"
#include "Polyhedron/Polyhedron.h"
#include "DataContainers/EdgeData/EdgeData.h"
#include "Correctors/EdgeReducer/EdgeReducer.h"

class Verifier
{
private:
	PolyhedronPtr polyhedron;
	bool ifPrint;
	EdgesWorkingSets edgesWS;

	int countInnerConsections();
	int countInnerConsectionsFacet(int fid, double *A, double *b,
								   Vector3d *vertex_old);
	int countInnerConsectionsPair(int fid, int id0, int id1, int id2, int id3,
								  double *A, double *b);
	int countOuterConsections();
	int countOuterConsectionsFacet(int fid);
	int countOuterConsectionsEdge(int id0, int id1);
	int countOuterConsectionsPair(int id0, int id1, int fid);

	/* Check whether the edge is not destructed. Returns "true" if it is not
	 * destructed. */
	bool checkOneEdge(EdgeSetIterator edge, EdgeDataPtr edgeData);

	/* Reduce one edge (for case when the edge is destructed).
	 * We need to verify following structures: VertexInfo,
	 * Facet, EdgeData, Edge for correctness. */
	bool reduceEdge(EdgeSetIterator edge, EdgeDataPtr edgeData);

public:
	Verifier();
	Verifier(PolyhedronPtr p);
	Verifier(Polyhedron *p);
	Verifier(PolyhedronPtr p, bool _ifPrint);
	Verifier(Polyhedron *p, bool _ifPrint);

	~Verifier();

	/* Count the number of self-consections in the polyhedron. */
	int countConsections();

	/* Count the number of destructed edges of the polyhedron. */
	int checkEdges(EdgeDataPtr edgeData);
};

#endif /* VERIFIER_H_ */
