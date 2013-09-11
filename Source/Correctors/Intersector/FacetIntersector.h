/*
 * FacetIntersector.h
 *
 *  Created on: 18.06.2013
 *      Author: ilya
 */

#ifndef FACETINTERSECTOR_H_
#define FACETINTERSECTOR_H_

class FacetIntersector: public Intersector
{
private:
	Facet* facet;

	int signum(int i, Plane plane);

public:
	FacetIntersector();
	FacetIntersector(shared_ptr<Polyhedron> p, Facet* f);
	FacetIntersector(Intersector* i, Facet* f);
	~FacetIntersector();

	bool run(Plane iplane, FutureFacet& ff, int& n_components);
};

#endif /* FACETINTERSECTOR_H_ */
