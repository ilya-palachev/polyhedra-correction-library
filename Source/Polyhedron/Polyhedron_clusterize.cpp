#include "PolyhedraCorrectionLibrary.h"

int Polyhedron::clusterize(double p)
{
	Clusterizer* clusterizer = new Clusterizer(this);
	int ret = clusterizer->clusterize(p);
	delete clusterizer;
	return ret;
}

void Polyhedron::clusterize2(double p)
{
	Clusterizer* clusterizer = new Clusterizer(this);
	clusterizer->clusterize2(p);
	delete clusterizer;
}

TreeClusterNorm& Polyhedron::build_TreeClusterNorm()
{
	Clusterizer* clusterizer = new Clusterizer(this);
	TreeClusterNorm& ret = clusterizer->build_TreeClusterNorm();
	delete clusterizer;
	return ret;
}

void Polyhedron::giveClusterNodeArray(TreeClusterNormNode* nodeArray,
		MatrixDistNorm& matrix)
{
	Clusterizer* clusterizer = new Clusterizer(this);
	clusterizer->giveClusterNodeArray(nodeArray, matrix);
	delete clusterizer;
}
void Polyhedron::reClusterNodeArray(TreeClusterNormNode* nodeArray_in,
		MatrixDistNorm& matrix_in, TreeClusterNormNode* nodeArray_out,
		MatrixDistNorm& matrix_out)
{
	Clusterizer* clusterizer = new Clusterizer(this);
	clusterizer->reClusterNodeArray(nodeArray_in, matrix_in, nodeArray_out,
			matrix_out);
	delete clusterizer;
}
