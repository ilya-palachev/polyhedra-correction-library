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
	Polyhedron* polyhedron;
	PData(const Polyhedron* polyhedron);
	virtual ~PData();
};

#endif /* PDATA_H_ */
