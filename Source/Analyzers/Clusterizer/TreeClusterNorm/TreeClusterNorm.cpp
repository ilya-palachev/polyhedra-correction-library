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

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "Analyzers/Clusterizer/TreeClusterNorm/TreeClusterNorm.h"
#include "Analyzers/Clusterizer/ClusterNorm/ClusterNorm.h"

TreeClusterNorm::TreeClusterNorm() :
				root(NULL)
{
	DEBUG_START;
	DEBUG_END;
}

TreeClusterNorm::TreeClusterNorm(const TreeClusterNorm& orig) :
				root(orig.root)
{
	DEBUG_START;
	DEBUG_END;
}

TreeClusterNorm::TreeClusterNorm(TreeClusterNormNode* root_orig) :
				root(root_orig)
{
	DEBUG_START;
	DEBUG_END;
}

TreeClusterNorm::~TreeClusterNorm()
{
	DEBUG_START;
	DEBUG_END;
}

void TreeClusterNorm::fprint(FILE* file)
{
	DEBUG_START;
	root->fprint(file, 0);
	DEBUG_END;
}

void TreeClusterNorm::fprint_dendrogamma_lev(FILE* file)
{
	DEBUG_START;
	for (int i = 0; i < 4; i++)
	{
		root->fprint_dendrogramma_lev(file, 0, i);
		REGULAR_PRINT(file, "\n");
	}
	root->cluster->fprint(stdout);
	DEBUG_END;
}
