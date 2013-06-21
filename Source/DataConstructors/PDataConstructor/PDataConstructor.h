/*
 * PDataConstructor.h
 *
 *  Created on: 17.05.2013
 *      Author: iliya
 */

#ifndef PDATACONSTRUCTOR_H_
#define PDATACONSTRUCTOR_H_

/*
 *
 */
class PDataConstructor
{
public:
	Polyhedron* polyhedron;

	PDataConstructor(Polyhedron* p);
	virtual ~PDataConstructor();
};

#endif /* PDATACONSTRUCTOR_H_ */
