#include "PolyhedraCorrectionLibrary.h"

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
