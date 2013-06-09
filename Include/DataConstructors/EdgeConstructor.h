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
private:
	EdgeData* edgeData;

public:
	EdgeConstructor(Polyhedron* p, EdgeData* e);
	~EdgeConstructor();
	void run();
};

#endif /* EDGECONSTRUCTOR_H_ */
