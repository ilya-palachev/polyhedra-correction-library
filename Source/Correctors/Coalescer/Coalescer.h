/*
 * Copyright (c) 2009-2015 Ilya Palachev <iliyapalachev@gmail.com>
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

#ifndef COALESCER_H_
#define COALESCER_H_

#include "Vector3d.h"
#include "Polyhedron/Facet/Facet.h"
#include "Correctors/PCorrector/PCorrector.h"

class Coalescer: public PCorrector
{
private:
	Plane plane;
	Facet coalescedFacet;

	int buildIndex(int fid0, int fid1);
	int buildIndex(int n, int* fid);
	void calculatePlane(int fid0, int fid1);
	void calculatePlane(int n, int* fid);
	void rise(int fid0);
	int riseFind(int fid0);
	double riseFindStep(int fid0, int i);
	void risePoint(int fid0, int imin);
	void deleteVertexInPolyhedron(int v);
	int appendVertex(Vector3d& vec);
	void leastSquaresMethod(int nv, int* vertex_list, Plane* plane);
	void leastSquaresMethodWeighted(int nv, int* vertex_list, Plane* plane);
public:
	Coalescer();
	Coalescer(PolyhedronPtr p);
	Coalescer(Polyhedron* p);
	~Coalescer();
	void run(int fid0, int fid1);
	void run(int n, int* fid);
};

#endif /* COALESCER_H_ */
