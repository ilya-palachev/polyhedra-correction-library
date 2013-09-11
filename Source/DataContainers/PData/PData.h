/*
 * PData.h
 *
 *  Created on: 17.05.2013
 *      Author: iliya
 */

#ifndef PDATA_H_
#define PDATA_H_

/*
 *
 */
class PData
{
public:
	shared_ptr<Polyhedron> polyhedron;
	PData(shared_ptr<Polyhedron> p);
	virtual ~PData();
};

#endif /* PDATA_H_ */
