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
public:
	EdgeReducer();
	EdgeReducer(Polyhedron* p);
	~EdgeReducer();

	bool run(EdgeSetIterator edge, EdgeDataPtr edgeData);
};

#endif /* EDGEREDUCER_H_ */
