/*
 * Copyright (c) 2009-2014 Ilya Palachev <iliyapalachev@gmail.com>
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

#ifndef FACETINTERSECTOR_H_
#define FACETINTERSECTOR_H_

#include "Correctors/Intersector/Intersector.h"
#include "Correctors/Intersector/FutureFacet/FutureFacet.h"

class Facet;

class FacetIntersector: public Intersector
{
private:
	Facet* facet;

	int signum(int i, Plane plane);

public:
	FacetIntersector();
	FacetIntersector(PolyhedronPtr p, Facet* f);
	FacetIntersector(Intersector* i, Facet* f);
	~FacetIntersector();

	bool run(Plane iplane, FutureFacet& ff, int& n_components);
};

#endif /* FACETINTERSECTOR_H_ */
