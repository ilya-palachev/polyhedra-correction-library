/* 
 * Copyright (c) 2009-2013 Ilya Palachev <iliyapalachev@gmail.com>
 * Copyright (c) 2009-2012 Nikolai Kaligin <nkaligin@yandex.ru>
 * 
 * This file is part of Polyhedra Correction Library.
 *
 * Polyhedra Correction Library is free software: you can redistribute 
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Polyhedra Correction Library is distributed in the hope that it will 
 * be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Polyhedra Correction Library.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MATRIXDISTNORM_H
#define MATRIXDISTNORM_H

#include <cstdio>

#include "Analyzers/Clusterizer/TreeClusterNormNode/TreeClusterNormNode.h"

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
