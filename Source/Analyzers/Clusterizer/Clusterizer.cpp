/*
 * Clusterizer.cpp
 *
 *  Created on: 21.06.2013
 *      Author: ilya
 */

#include "PolyhedraCorrectionLibrary.h"

Clusterizer::Clusterizer() :
		PAnalyzer(), cluster(), A(), inc(), dist()
{
	DEBUG_START;
	DEBUG_END;
}

Clusterizer::Clusterizer(Polyhedron* p) :
		PAnalyzer(p), cluster(new int[polyhedron->numFacets]), A(
				new int[polyhedron->numFacets]), inc(
				new bool[polyhedron->numFacets * polyhedron->numFacets]), dist(
				new double[polyhedron->numFacets * polyhedron->numFacets])
{
	DEBUG_START;
	DEBUG_END;
}

Clusterizer::~Clusterizer()
{
	DEBUG_START;
	if (dist != NULL)
	{
		delete[] dist;
		dist = NULL;
	}

	if (A != NULL)
	{
		delete[] A;
		A = NULL;
	}

	if (inc != NULL)
	{
		delete[] inc;
		inc = NULL;
	}

	if (cluster != NULL)
	{
		delete[] cluster;
		cluster = NULL;
	}
	DEBUG_END;
}

int Clusterizer::clusterize(double p)
{
	DEBUG_START;
	int i, j, nv, *index, fid;
	double sin_alpha, alpha;

	int ncluster;

	Vector3d n0, n1;

	for (i = 0; i < polyhedron->numFacets * polyhedron->numFacets; ++i)
		dist[i] = 100.;

	for (i = 0; i < polyhedron->numFacets; ++i)
	{
		nv = polyhedron->facets[i].numVertices;
		index = polyhedron->facets[i].indVertices;
		n0 = polyhedron->facets[i].plane.norm;
		for (j = 0; j < nv; ++j)
		{
			fid = index[nv + 1 + j];
			n1 = polyhedron->facets[fid].plane.norm;
			sin_alpha = sqrt(qmod(n0 % n1) / (qmod(n0) * qmod(n1)));
			alpha = asin(sin_alpha);
			alpha = fabs(alpha);
			if (i * polyhedron->numFacets + fid
					>= polyhedron->numFacets * polyhedron->numFacets)
				DEBUG_PRINT("Warning!!!\n");
			dist[i * polyhedron->numFacets + fid] = alpha;
		}
	}

	for (i = 0; i < polyhedron->numFacets * polyhedron->numFacets; ++i)
	{
		if (dist[i] < p)
			inc[i] = true;
		else
			inc[i] = false;
	}

	for (i = 0; i < polyhedron->numFacets; ++i)
		cluster[i] = i;

	ncluster = polyhedron->numFacets;
	for (i = 0; i < polyhedron->numFacets; ++i)
	{
		for (j = 0; j < i; ++j)
		{
			if (inc[i * polyhedron->numFacets + j])
			{
				cluster[i] = cluster[j];
				--ncluster;
				break;
			}
		}
	}

	DEBUG_END;
	return ncluster;
}

void fire(int l, int* A, int* cluster, SortedSetOfPairs& ssop, int depth)
{
	DEBUG_START;

	int k, i;
	for (i = 0; i < depth; ++i)
		DEBUG_PRINT(" ");
	DEBUG_PRINT("fire %d\n", l);

	for (k = A[l]; k < A[l + 1]; ++k)
	{

		i = ssop.jj[k];

		if (cluster[i] != cluster[l])
		{
			cluster[i] = cluster[l];
			fire(i, A, cluster, ssop, depth + 1);
		}
	}
	DEBUG_END;
}

void Clusterizer::clusterize2(double p)
{
	DEBUG_START;

	int i, j, nv, *index, fid;
	double sin_alpha, alpha;

	Vector3d n0, n1;

	for (i = 0; i < polyhedron->numFacets; ++i)
	{
		A[i] = 0;
	}
	SortedSetOfPairs ssop(polyhedron->numFacets * polyhedron->numFacets);

	for (i = 0; i < polyhedron->numFacets; ++i)
	{
		nv = polyhedron->facets[i].numVertices;
		index = polyhedron->facets[i].indVertices;
		n0 = polyhedron->facets[i].plane.norm;
		for (j = 0; j < nv; ++j)
		{
			fid = index[nv + 1 + j];
			n1 = polyhedron->facets[fid].plane.norm;
			sin_alpha = sqrt(qmod(n0 % n1) / (qmod(n0) * qmod(n1)));
			alpha = asin(sin_alpha);
			alpha = fabs(alpha);
			if (alpha < p)
			{
				ssop.add(i, fid);
				ssop.add(fid, i);
				++A[i];
			}
		}

	}

	for (i = 0; i < polyhedron->numFacets - 1; ++i)
	{
		A[i + 1] += A[i];
	}

	for (i = polyhedron->numFacets - 1; i > 0; --i)
	{
		A[i] = A[i - 1];
	}
	A[0] = 0;

	ssop.print();
	for (i = 0; i < polyhedron->numFacets; ++i)
	{
		DEBUG_PRINT("A[%d] = %d\n", i, A[i]);
	}
//    return;

	for (i = 0; i < polyhedron->numFacets; ++i)
		cluster[i] = i;

	int l = 0;
	while (l < polyhedron->numFacets - 1)
	{
		while (cluster[l] != l)
			++l;
		DEBUG_PRINT("begin new step l = %d\n", l);
		fire(l, A, cluster, ssop, 0);
		++l;
	}

	for (i = 0; i < polyhedron->numFacets; ++i)
	{
		if (cluster[i] != i)
			DEBUG_PRINT("cluster[%d] = %d\n", i, cluster[i]);
	}
	DEBUG_END;
}

const double LIMIT_FOR_CLUSTERIZATION = 0.5;

TreeClusterNorm& Clusterizer::build_TreeClusterNorm()
{
	DEBUG_START;

	int* newIndex;
	SpherePoint spherePoint;

	TreeClusterNormNode* nodeArray;
	TreeClusterNormNode* newChild0;
	TreeClusterNormNode* newChild1;

	ClusterNorm* newCluster;

	nodeArray = new TreeClusterNormNode[polyhedron->numFacets];
	newIndex = new int[1];

	for (int i = 0; i < polyhedron->numFacets; ++i)
	{
//        DEBUG_PRINT("norm[%d] = %lf\n", i, sqrt(qmod(facets[i].plane.norm)) );
		spherePoint = SpherePoint(polyhedron->facets[i].plane.norm);
		newIndex[0] = i;
		newCluster = new ClusterNorm;
		*newCluster = ClusterNorm(1, 1, spherePoint, newIndex, polyhedron);

		nodeArray[i].cluster = newCluster;

//        nodeArray[i].fprint(stdout, 0);
	}
	for (int i = 0; i < polyhedron->numFacets; ++i)
	{
//        nodeArray[i].fprint(stdout, 0);
	}

	MatrixDistNorm matrix(polyhedron->numFacets);

	matrix.build(polyhedron->numFacets, nodeArray);
//    matrix.fprint(stdout);
	matrix.fprint_clusters(stdout, nodeArray);

	int id0, id1;
	for (int i = 0; i < polyhedron->numFacets - 1; ++i)
	{
		DEBUG_PRINT(
				"---------------------------------------------------------\n");
		DEBUG_PRINT(
				"-----------------------    %d    ------------------------\n",
				i);
		DEBUG_PRINT(
				"---------------------------------------------------------\n");

		matrix.fprint_ifStay(stdout);

		double distance_id0_id1 = matrix.findMin(id0, id1);

#ifndef NOPOROG
		if (distance_id0_id1 > LIMIT_FOR_CLUSTERIZATION)
			break;
#endif
		//  DEBUG_PRINT("min_id0 = %d, min_id1 = %d, distance(id0, id1) = %lf\n", id0, id1, distance_id0_id1);
		DEBUG_PRINT("id0 = %d, id1 = %d, distance(id0, id1) = %lf\n",
				id0, id1, distance_id0_id1);
//        matrix.fprint(stdout);

		newChild0 = new TreeClusterNormNode;
		*newChild0 = nodeArray[id0];

		newChild1 = new TreeClusterNormNode;
		*newChild1 = nodeArray[id1];

		newCluster = new ClusterNorm;
		*newCluster = *(newChild0->cluster);
		*newCluster += *(newChild1->cluster);

		nodeArray[id0].cluster = newCluster;
		nodeArray[id0].child0 = newChild0;
		nodeArray[id0].child1 = newChild1;

		matrix.rebuild(id0, id1, nodeArray);
	}

	matrix.fprint_clusters(stdout, nodeArray);
	matrix.setColors(nodeArray);

	int numClust = 0;
	for (int i = 0; i < matrix.n; ++i)
		if (matrix.ifStay[i])
			++numClust;
	DEBUG_PRINT("numClust = %d", numClust);

	TreeClusterNorm* tree;
	tree = new TreeClusterNorm;
	*tree = TreeClusterNorm(nodeArray);

	tree->fprint(stdout);
	DEBUG_END;
	return *tree;
}

void Clusterizer::giveClusterNodeArray(TreeClusterNormNode* nodeArray,
		MatrixDistNorm& matrix)
{

	int* newIndex;
	SpherePoint spherePoint;

	TreeClusterNormNode* newChild0;
	TreeClusterNormNode* newChild1;

	ClusterNorm* newCluster;

	newIndex = new int[1];

	for (int i = 0; i < polyhedron->numFacets; ++i)
	{
		DEBUG_PRINT("norm[%d] = %lf\n",
				i, sqrt(qmod(polyhedron->facets[i].plane.norm)));
		spherePoint = SpherePoint(polyhedron->facets[i].plane.norm);
		newIndex[0] = i;
		newCluster = new ClusterNorm;
		*newCluster = ClusterNorm(1, 1, spherePoint, newIndex, polyhedron);

		nodeArray[i].cluster = newCluster;

		nodeArray[i].fprint(stdout, 0);
	}
	for (int i = 0; i < polyhedron->numFacets; ++i)
	{
		nodeArray[i].fprint(stdout, 0);
	}

	matrix.build(polyhedron->numFacets, nodeArray);
	matrix.fprint(stdout);
	matrix.fprint_clusters(stdout, nodeArray);

	int id0, id1;
	for (int i = 0; i < polyhedron->numFacets - 1; ++i)
	{
		DEBUG_PRINT(
				"---------------------------------------------------------\n");
		DEBUG_PRINT(
				"-----------------------    %d    ------------------------\n",
				i);
		DEBUG_PRINT(
				"---------------------------------------------------------\n");

		//matrix.fprint_ifStay(stdout);

		double distance_id0_id1 = matrix.findMin(id0, id1);

#ifndef NOPOROG
		if (distance_id0_id1 > LIMIT_FOR_CLUSTERIZATION)
			break;
#endif
		DEBUG_PRINT("min_id0 = %d, min_id1 = %d, distance(id0, id1) = %lf\n",
				id0, id1, distance_id0_id1);
		DEBUG_PRINT("id0 = %d, id1 = %d, distance(id0, id1) = %lf\n",
				id0, id1, distance_id0_id1);
		matrix.fprint(stdout);

		newChild0 = new TreeClusterNormNode;
		*newChild0 = nodeArray[id0];

		newChild1 = new TreeClusterNormNode;
		*newChild1 = nodeArray[id1];

		newCluster = new ClusterNorm;
		*newCluster = *(newChild0->cluster);
		*newCluster += *(newChild1->cluster);

		nodeArray[id0].cluster = newCluster;
		nodeArray[id0].child0 = newChild0;
		nodeArray[id0].child1 = newChild1;

		matrix.rebuild(id0, id1, nodeArray);
		matrix.fprint(stdout);
	}

	matrix.fprint_clusters(stdout, nodeArray);
	DEBUG_END;
}

void Clusterizer::reClusterNodeArray(TreeClusterNormNode* nodeArray_in,
		MatrixDistNorm& matrix_in, TreeClusterNormNode* nodeArray_out,
		MatrixDistNorm& matrix_out)
{
	DEBUG_START;
	int numCluster = 0;
	for (int i = 0; i < matrix_in.n; ++i)
	{
		if (matrix_in.ifStay[i])
		{
			++numCluster;
		}
	}
	DEBUG_PRINT("==========================================================\n");
	DEBUG_PRINT("================  reClusterNodeArray =====================\n");
	DEBUG_PRINT("==========================================================\n");

	int position;
	ClusterNorm* newCluster;
	SpherePoint spherePoint;
	int *newIndex;

	newIndex = new int[1];

	for (int i = 0; i < matrix_in.n; ++i)
	{
		nodeArray_out[i].cluster = new ClusterNorm;
	}

	for (int i = 0; i < polyhedron->numFacets; i++)
	{
		DEBUG_PRINT("Trying to find nearest cluster for facet %d\n", i);

		SpherePoint FacetNorm(polyhedron->facets[i].plane.norm);
		double minDistPoint = 1000000;
		position = -1;
		for (int j = 0; j < numCluster; j++)
		{
			double tmpdist = distSpherePoint(FacetNorm,
					nodeArray_in[j].cluster->P);
			if (tmpdist < minDistPoint)
			{
				minDistPoint = tmpdist;
				position = j;
			}
		}
		DEBUG_PRINT("position[%d] = %d\n", i, position);

		spherePoint = SpherePoint(polyhedron->facets[i].plane.norm);
		newIndex[0] = i;
		newCluster = new ClusterNorm;
		*newCluster = ClusterNorm(1, 1, spherePoint, newIndex, polyhedron);

		DEBUG_PRINT("Local cluster : ");
		newCluster->fprint(stdout);
		DEBUG_PRINT("\n");

		DEBUG_PRINT("Previous state of cluster : ");
		nodeArray_out[position].cluster->fprint(stdout);
		DEBUG_PRINT("\n");

		if (nodeArray_out[position].cluster->num == 0)
		{
			nodeArray_out[position].cluster = newCluster;
		}
		else
		{
			*(nodeArray_out[position].cluster) += *newCluster;
		}
		DEBUG_PRINT("End of step %d\n", i);

	}

	for (int i = 0; i < numCluster; i++)
	{
		matrix_out.ifStay[i] = true;
	}
	for (int i = numCluster + 1; i < matrix_in.n; i++)
	{
		matrix_out.ifStay[i] = false;
	}
	DEBUG_END;
}

