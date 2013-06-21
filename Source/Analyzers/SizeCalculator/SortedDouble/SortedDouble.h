/*
 * SortedDouble.h
 *
 *  Created on: 21.06.2013
 *      Author: ilya
 */

#ifndef SORTEDDOUBLE_H_
#define SORTEDDOUBLE_H_

class SortedDouble
{
public:
	int len;
	int num;
	double* array;

	SortedDouble(int Len);
	~SortedDouble();

	void add(double x);
	double calclulate();

};

#endif /* SORTEDDOUBLE_H_ */
