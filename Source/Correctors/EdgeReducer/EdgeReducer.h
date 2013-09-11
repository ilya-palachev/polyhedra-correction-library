/*
 * EdgeReducer.h
 *
 *  Created on: Aug 22, 2013
 *      Author: ilya
 */

#ifndef EDGEREDUCER_H_
#define EDGEREDUCER_H_

typedef set<pair<int, int>>* IntPairPtr;

struct _EdgesWorkingSets
{
	IntPairPtr edgesErased;
	IntPairPtr edgesAdded;
	IntPairPtr edgesEdited;
};
typedef _EdgesWorkingSets EdgesWorkingSets;

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

	EdgesWorkingSets* edgesWS;

	void init();
	bool updateFacets();
	void cutDegeneratedFacet(int iFacet);
	bool rePreprocessFacets();
	bool updateEdges();
	bool updateVertexInfos();
	void cutDegeneratedVertex(int iVertex, queue<int>& facetsQueue);

	bool verifyEdgeData();

public:
	EdgeReducer();
	EdgeReducer(shared_ptr<Polyhedron> p);
	~EdgeReducer();

	bool run(EdgeSetIterator _edge, EdgeDataPtr _edgeData,
			EdgesWorkingSets* _edgesWS);
};

#endif /* EDGEREDUCER_H_ */
