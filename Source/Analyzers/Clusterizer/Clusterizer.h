/*
 * Clusterizer.h
 *
 *  Created on: 21.06.2013
 *      Author: ilya
 */

#ifndef CLUSTERIZER_H_
#define CLUSTERIZER_H_

class Clusterizer: public PAnalyzer
{
private:
	int* cluster;
	int* A;
	bool* inc;
	double* dist;

public:
	Clusterizer();
	Clusterizer(Polyhedron* p);
	~Clusterizer();
	int clusterize(double p);
	void clusterize2(double p);
	TreeClusterNorm& build_TreeClusterNorm();
	void giveClusterNodeArray(TreeClusterNormNode* nodeArray,
			MatrixDistNorm& matrix);
	void reClusterNodeArray(TreeClusterNormNode* nodeArray_in,
			MatrixDistNorm& matrix_in, TreeClusterNormNode* nodeArray_out,
			MatrixDistNorm& matrix_out);
};

#endif /* CLUSTERIZER_H_ */
