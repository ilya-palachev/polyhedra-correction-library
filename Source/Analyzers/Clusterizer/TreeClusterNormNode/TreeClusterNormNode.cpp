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

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "Analyzers/Clusterizer/TreeClusterNormNode/TreeClusterNormNode.h"

TreeClusterNormNode::TreeClusterNormNode() : cluster(NULL), parent(NULL), child0(NULL), child1(NULL)
{
	DEBUG_START;
	DEBUG_END;
}

TreeClusterNormNode::TreeClusterNormNode(const TreeClusterNormNode &orig) :
	cluster(orig.cluster), parent(orig.parent), child0(orig.child0), child1(orig.child1)
{
	DEBUG_START;
	DEBUG_END;
}

TreeClusterNormNode::~TreeClusterNormNode()
{
	DEBUG_START;
	DEBUG_END;
}

void TreeClusterNormNode::fprint(FILE *file, int level)
{
	DEBUG_START;

	for (int i = 0; i < level; ++i)
	{
		REGULAR_PRINT(file, " ");
	}
	cluster->fprint(file);
	REGULAR_PRINT(file, "\n");
	if (child0 != NULL)
	{
		child0->fprint(file, level + 1);
	}
	if (child1 != NULL)
	{
		child1->fprint(file, level + 1);
	}
	DEBUG_END;
}

void TreeClusterNormNode::fprint_dendrogramma_lev(FILE *file, int level, int fix)
{
	DEBUG_START;

	if (level > fix)
	{
		return;
	}

	int numLeft = child0->cluster->num;
	int numRight = child1->cluster->num;

	if (level == fix)
	{
		REGULAR_PRINT(file, "|___");
		for (int i = 1; i < numLeft; ++i)
		{
			REGULAR_PRINT(file, "____");
		}
		for (int i = 0; i < numRight; ++i)
		{
			REGULAR_PRINT(file, "    ");
		}
	}
	else
	{
		child0->fprint_dendrogramma_lev(file, level + 1, fix);
		child1->fprint_dendrogramma_lev(file, level + 1, fix);
	}
	DEBUG_END;
}
