/*
 * EdgeConstructor.h
 *
 *  Created on: 05.06.2013
 *      Author: iliya
 */

#ifndef EDGECONSTRUCTOR_H_
#define EDGECONSTRUCTOR_H_

/*
 *
 */
class EdgeConstructor : public PDataConstructor
{
public:
	EdgeConstructor(Polyhedron* p, EdgeData* e);
	~EdgeConstructor();
	void run(EdgeData* &edgeData);
};

#endif /* EDGECONSTRUCTOR_H_ */
