/*
 * Intersector.h
 *
 *  Created on: 18.06.2013
 *      Author: ilya
 */

#ifndef INTERSECTOR_H_
#define INTERSECTOR_H_

class Intersector: public PCorrector
{
protected:
	EdgeList* edgeLists;

private:
	void set_isUsed(int v0, int v1, bool val);
	bool intersectFacet(Facet* facet, Plane iplane, FutureFacet& ff,
			int& n_components);
	int prepareEdgeList(Facet* facet, Plane iplane);
public:
	Intersector();
	Intersector(Polyhedron* input);
	~Intersector();
	void run(Plane iplane);
	void runJoinMode(Plane iplane, int jfid);
};

#endif /* INTERSECTOR_H_ */
