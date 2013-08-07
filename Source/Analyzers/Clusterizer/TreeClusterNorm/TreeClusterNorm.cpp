/* 
 * File:   TreeClusterNorm.cpp
 * Author: nk
 * 
 * Created on 1 Май 2012 г., 12:43
 */
#include "PolyhedraCorrectionLibrary.h"

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
