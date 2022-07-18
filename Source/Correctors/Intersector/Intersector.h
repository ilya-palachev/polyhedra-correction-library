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

#ifndef INTERSECTOR_H_
#define INTERSECTOR_H_

#include "Vector3d.h"
#include "Correctors/PCorrector/PCorrector.h"
#include "Correctors/Intersector/FutureFacet/FutureFacet.h"

class Facet;
class EdgeList;

class Intersector : public PCorrector
{
protected:
	std::vector<EdgeList> edgeLists;

private:
	void set_isUsed(int v0, int v1, bool val);
	bool intersectFacet(Facet *facet, Plane iplane, FutureFacet &ff, int &n_components);
	int prepareEdgeList(Facet *facet, Plane iplane);

public:
	Intersector();
	Intersector(PolyhedronPtr p);
	Intersector(Polyhedron *p);
	~Intersector();
	void run(Plane iplane);
	void runCoalesceMode(Plane iplane, int jfid);
};

#endif /* INTERSECTOR_H_ */
