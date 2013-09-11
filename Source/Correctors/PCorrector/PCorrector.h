/*
 * PCorrector.h
 *
 *  Created on: 12.05.2013
 *      Author: iliya
 */

#ifndef PCORRECTOR_H_
#define PCORRECTOR_H_

/*
 *
 */
class PCorrector
{
public:
	shared_ptr<Polyhedron> polyhedron;

	PCorrector();
	PCorrector(shared_ptr<Polyhedron> p);
	PCorrector(Polyhedron* p);
	virtual ~PCorrector();
};

#endif /* PCORRECTOR_H_ */
