/*
 * Copyright (c) 2009-2016 Ilya Palachev <iliyapalachev@gmail.com>
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

#ifndef CLUSTERIZER_H_
#define CLUSTERIZER_H_

#include <memory>

#include "Analyzers/PAnalyzer/PAnalyzer.h"
#include "Polyhedron/Polyhedron.h"
#include "Analyzers/Clusterizer/TreeClusterNormNode/TreeClusterNormNode.h"
#include "Analyzers/Clusterizer/TreeClusterNorm/TreeClusterNorm.h"
#include "Analyzers/Clusterizer/MatrixDistNorm/MatrixDistNorm.h"

class Clusterizer : public PAnalyzer
{
private:
	int *cluster;
	int *A;
	bool *inc;
	double *dist;

public:
	Clusterizer();
	Clusterizer(PolyhedronPtr p);
	~Clusterizer();
	int clusterize(double p);
	void clusterize2(double p);
	TreeClusterNorm &build_TreeClusterNorm();
	void giveClusterNodeArray(TreeClusterNormNode *nodeArray,
							  MatrixDistNorm &matrix);
	void reClusterNodeArray(TreeClusterNormNode *nodeArray_in,
							MatrixDistNorm &matrix_in,
							TreeClusterNormNode *nodeArray_out,
							MatrixDistNorm &matrix_out);
};

#endif /* CLUSTERIZER_H_ */
