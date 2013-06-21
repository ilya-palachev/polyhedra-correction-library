/*
 * PAnalyzer.h
 *
 *  Created on: 21.06.2013
 *      Author: ilya
 */

#ifndef PANALYZER_H_
#define PANALYZER_H_

class PAnalyzer
{
protected:
	Polyhedron* polyhedron;
public:
	PAnalyzer();
	PAnalyzer(Polyhedron* p);
	virtual ~PAnalyzer();
};

#endif /* PANALYZER_H_ */
