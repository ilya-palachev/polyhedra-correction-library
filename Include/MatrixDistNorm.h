/*
 * MatrixDistNorm.h
 *
 * Created on:  01.05.2012
 *     Author:  Kaligin Nikolai <nkaligin@yandex.ru>
 *
 */

#ifndef MATRIXDISTNORM_H
#define MATRIXDISTNORM_H

class MatrixDistNorm
{
public:
	int nMax;
	int n;
	double* matrix;
	bool* ifStay;

	MatrixDistNorm();
	MatrixDistNorm(int nMax_orig);
	MatrixDistNorm(const MatrixDistNorm& orig);
	virtual ~MatrixDistNorm();

	void build(int m, TreeClusterNormNode* nodeArray);
	double findMin(int& imin, int& jmin);
	void rebuild(int imin, int jmin, TreeClusterNormNode* nodeArray);

	void fprint(FILE* file);
	void fprint_ifStay(FILE* file);

	void fprint_clusters(FILE* file, TreeClusterNormNode* nodeArray);
	void fprint_clusters2(FILE* file, TreeClusterNormNode* nodeArray);
	double sqNorm(TreeClusterNormNode* nodeArray1,
			TreeClusterNormNode* nodeArray2);
	void setColors(TreeClusterNormNode* nodeArray);
	void setColors2(TreeClusterNormNode* nodeArray);

};

#endif /* MATRIXDISTNORM_H */
