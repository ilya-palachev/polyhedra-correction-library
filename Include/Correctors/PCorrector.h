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
	Polyhedron* polyhedron;

	PCorrector();
	PCorrector(Polyhedron* input);
	virtual ~PCorrector();
};

#endif /* PCORRECTOR_H_ */
