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
private:
	Polyhedron* polyhedron;
public:
	PCorrector(Polyhedron* input);
	virtual ~PCorrector();

	virtual void runCorrection();
};

#endif /* PCORRECTOR_H_ */
