/* 
 * Copyright (c) 2009-2013 Ilya Palachev <iliyapalachev@gmail.com>
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

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "Polyhedron/Polyhedron.h"
#include "Analyzers/Clusterizer/Clusterizer.h"

int Polyhedron::clusterize(double p)
{
	DEBUG_START;
	Clusterizer* clusterizer = new Clusterizer(get_ptr());
	int ret = clusterizer->clusterize(p);
	delete clusterizer;
	DEBUG_END;
	return ret;
}

void Polyhedron::clusterize2(double p)
{
	DEBUG_START;
	Clusterizer* clusterizer = new Clusterizer(get_ptr());
	clusterizer->clusterize2(p);
	DEBUG_END;
	delete clusterizer;
}

TreeClusterNorm& Polyhedron::build_TreeClusterNorm()
{
	DEBUG_START;
	Clusterizer* clusterizer = new Clusterizer(get_ptr());
	TreeClusterNorm& ret = clusterizer->build_TreeClusterNorm();
	delete clusterizer;
	DEBUG_END;
	return ret;
}

void Polyhedron::giveClusterNodeArray(TreeClusterNormNode* nodeArray,
		MatrixDistNorm& matrix)
{
	DEBUG_START;
	Clusterizer* clusterizer = new Clusterizer(get_ptr());
	clusterizer->giveClusterNodeArray(nodeArray, matrix);
	delete clusterizer;
	DEBUG_END;
}
void Polyhedron::reClusterNodeArray(TreeClusterNormNode* nodeArray_in,
		MatrixDistNorm& matrix_in, TreeClusterNormNode* nodeArray_out,
		MatrixDistNorm& matrix_out)
{
	DEBUG_START;
	Clusterizer* clusterizer = new Clusterizer(get_ptr());
	clusterizer->reClusterNodeArray(nodeArray_in, matrix_in, nodeArray_out,
			matrix_out);
	delete clusterizer;
	DEBUG_END;
}
