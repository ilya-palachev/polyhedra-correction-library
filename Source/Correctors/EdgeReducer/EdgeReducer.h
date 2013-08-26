/*
 * EdgeReducer.h
 *
 *  Created on: Aug 22, 2013
 *      Author: ilya
 */

#ifndef EDGEREDUCER_H_
#define EDGEREDUCER_H_

class EdgeReducer: public PCorrector
{
private:
	EdgeDataPtr edgeData;
	EdgeSetIterator edge;

	int iVertexReduced;
	int iVertexStayed;
	VertexInfo* vertexInfoReduced;
	VertexInfo* vertexInfoStayed;
	set<int> facetsUpdated;

	void init();
	bool updateFacets();
	void cutDegeneratedFacet(int iFacet);
	bool rePreprocessFacets();
	bool updateEdges();
	bool updateVertexInfos();
	void cutDegeneratedVertex(int iVertex);

public:
	EdgeReducer();
	EdgeReducer(Polyhedron* p);
	~EdgeReducer();

	bool run(EdgeSetIterator _edge, EdgeDataPtr _edgeData);
};

#endif /* EDGEREDUCER_H_ */
