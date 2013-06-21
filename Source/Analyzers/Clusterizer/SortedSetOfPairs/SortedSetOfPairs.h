/*
 * SortedSetOfPairs.h
 *
 *  Created on: 21.06.2013
 *      Author: ilya
 */

#ifndef SORTEDSETOFPAIRS_H_
#define SORTEDSETOFPAIRS_H_

class SortedSetOfPairs
{
public:
	int num;
	int len;

	int* ii;
	int* jj;

	SortedSetOfPairs(int Len);
	~SortedSetOfPairs();
	void add(int i, int j);
	void print();
};

#endif /* SORTEDSETOFPAIRS_H_ */
