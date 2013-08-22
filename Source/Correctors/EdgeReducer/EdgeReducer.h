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

public:
	EdgeReducer();
	EdgeReducer(Polyhedron* p);
	~EdgeReducer();

	bool run(EdgeSetIterator _edge, EdgeDataPtr _edgeData);
};

#endif /* EDGEREDUCER_H_ */
