#include "PolyhedraCorrectionLibrary.h"

TreeClusterNorm& Polyhedron::build_TreeClusterNorm()
{

//    my_fprint(stdout);

	int* newIndex;
	SpherePoint spherePoint;
//    ClusterNorm newCluster;

	TreeClusterNormNode* nodeArray;
	TreeClusterNormNode* newChild0;
	TreeClusterNormNode* newChild1;

	ClusterNorm* newCluster;

	nodeArray = new TreeClusterNormNode[numFacets];
	newIndex = new int[1];

	for (int i = 0; i < numFacets; ++i)
	{
//        printf("norm[%d] = %lf\n", i, sqrt(qmod(facets[i].plane.norm)) );
		spherePoint = SpherePoint(facets[i].plane.norm);
		newIndex[0] = i;
		newCluster = new ClusterNorm;
		*newCluster = ClusterNorm(1, 1, spherePoint, newIndex, this);

		nodeArray[i].cluster = newCluster;

//        nodeArray[i].fprint(stdout, 0);
	}
	for (int i = 0; i < numFacets; ++i)
	{
//        nodeArray[i].fprint(stdout, 0);
	}

	MatrixDistNorm matrix(numFacets);

	matrix.build(numFacets, nodeArray);
//    matrix.fprint(stdout);
	matrix.fprint_clusters(stdout, nodeArray);

	int id0, id1;
	for (int i = 0; i < numFacets - 1; ++i)
	{
		printf("---------------------------------------------------------\n");
		printf("-----------------------    %d    ------------------------\n",
				i);
		printf("---------------------------------------------------------\n");

		matrix.fprint_ifStay(stdout);

		double distance_id0_id1 = matrix.findMin(id0, id1);

#ifndef NOPOROG
		if (distance_id0_id1 > POROG)
			break;
#endif
		//  printf("min_id0 = %d, min_id1 = %d, distance(id0, id1) = %lf\n", id0, id1, distance_id0_id1);
		printf("id0 = %d, id1 = %d, distance(id0, id1) = %lf\n", id0, id1,
				distance_id0_id1);
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

//        for (int j = id1; j < numFacets - i - 1; ++j) {
//            nodeArray[j] = nodeArray[j + 1];
//        }

		matrix.rebuild(id0, id1, nodeArray);
//        matrix.fprint(stdout);
	}

	matrix.fprint_clusters(stdout, nodeArray);
	matrix.setColors(nodeArray);

	int numClust = 0;
	for (int i = 0; i < matrix.n; ++i)
		if (matrix.ifStay[i])
			++numClust;
	printf("numClust = %d", numClust);

	TreeClusterNorm* tree;
	tree = new TreeClusterNorm;
	*tree = TreeClusterNorm(nodeArray);

//    tree->fprint(stdout);
	return *tree;
}

void Polyhedron::giveClusterNodeArray(TreeClusterNormNode* nodeArray,
		MatrixDistNorm& matrix)
{
//    my_fprint(stdout);

	int* newIndex;
	SpherePoint spherePoint;
//    ClusterNorm newCluster;

//    TreeClusterNormNode* nodeArray;
	TreeClusterNormNode* newChild0;
	TreeClusterNormNode* newChild1;

	ClusterNorm* newCluster;

	//nodeArray = new TreeClusterNormNode[numFacets];
	newIndex = new int[1];

	for (int i = 0; i < numFacets; ++i)
	{
//        printf("norm[%d] = %lf\n", i, sqrt(qmod(facet[i].plane.norm)) );
		spherePoint = SpherePoint(facets[i].plane.norm);
		newIndex[0] = i;
		newCluster = new ClusterNorm;
		*newCluster = ClusterNorm(1, 1, spherePoint, newIndex, this);

		nodeArray[i].cluster = newCluster;

//        nodeArray[i].fprint(stdout, 0);
	}
	for (int i = 0; i < numFacets; ++i)
	{
//        nodeArray[i].fprint(stdout, 0);
	}

//    MatrixDistNorm matrix(numFacets);

	matrix.build(numFacets, nodeArray);
//    matrix.fprint(stdout);
//    matrix.fprint_clusters(stdout, nodeArray);

	int id0, id1;
	for (int i = 0; i < numFacets - 1; ++i)
	{
		printf("---------------------------------------------------------\n");
		printf("-----------------------    %d    ------------------------\n",
				i);
		printf("---------------------------------------------------------\n");

		//matrix.fprint_ifStay(stdout);

		double distance_id0_id1 = matrix.findMin(id0, id1);

#ifndef NOPOROG
		if (distance_id0_id1 > POROG)
			break;
#endif
		//  printf("min_id0 = %d, min_id1 = %d, distance(id0, id1) = %lf\n", id0, id1, distance_id0_id1);
		printf("id0 = %d, id1 = %d, distance(id0, id1) = %lf\n", id0, id1,
				distance_id0_id1);
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

//        for (int j = id1; j < numFacets - i - 1; ++j) {
//            nodeArray[j] = nodeArray[j + 1];
//        }

		matrix.rebuild(id0, id1, nodeArray);
		//     matrix.fprint(stdout);
	}

	matrix.fprint_clusters(stdout, nodeArray);
//    matrix.setColors(nodeArray);

//    int numClust = 0;
//    for (int i = 0; i < matrix.n; ++i)
//        if (matrix.ifStay[i])
//            ++numClust;
//    printf("numClust = %d", numClust);

//    TreeClusterNorm* tree;
//    tree = new TreeClusterNorm;
//    *tree = TreeClusterNorm(nodeArray);

//    tree->fprint(stdout);
//    return *tree;
}

void Polyhedron::reClusterNodeArray(TreeClusterNormNode* nodeArray_in,
		MatrixDistNorm& matrix_in, TreeClusterNormNode* nodeArray_out,
		MatrixDistNorm& matrix_out)
{
	int numCluster = 0;
	for (int i = 0; i < matrix_in.n; ++i)
	{
		if (matrix_in.ifStay[i])
		{
			++numCluster;
		}
	}
	printf("==========================================================\n");
	printf("================  reClusterNodeArray =====================\n");
	printf("==========================================================\n");

//    int *indexNearestCluster = NULL;
//    indexNearestCluster = new int[2 * numFacets];
//
//    int *indexSuperNew = NULL;
//    indexSuperNew = new int[2 * numFacets];

	int position;
	ClusterNorm* newCluster;
	SpherePoint spherePoint;
	int *newIndex;

	newIndex = new int[1];

	for (int i = 0; i < matrix_in.n; ++i)
	{
		nodeArray_out[i].cluster = new ClusterNorm;
	}

	for (int i = 0; i < numFacets; i++)
	{
//        printf("Trying to find nearest cluster for facet %d\n", i);

		SpherePoint FacetNorm(facets[i].plane.norm);
		double minDistPoint = 1000000;
		position = -1;
		for (int j = 0; j < numCluster; j++)
		{
			double tmpdist = dist(FacetNorm, nodeArray_in[j].cluster->P);
			if (tmpdist < minDistPoint)
			{
				minDistPoint = tmpdist;
				position = j;
			}
		}
		printf("position[%d] = %d\n", i, position);

		spherePoint = SpherePoint(facets[i].plane.norm);
		newIndex[0] = i;
		newCluster = new ClusterNorm;
		*newCluster = ClusterNorm(1, 1, spherePoint, newIndex, this);

//        printf("Local cluster : ");
//        newCluster->fprint(stdout);
//        printf("\n");

//        printf("Previous state of cluster : ");
//        nodeArray_out[position].cluster->fprint(stdout);
//        printf("\n");

		if (nodeArray_out[position].cluster->num == 0)
		{
			nodeArray_out[position].cluster = newCluster;
		}
		else
		{
			*(nodeArray_out[position].cluster) += *newCluster;
		}
//        printf("End of step %d\n", i);

	}

	for (int i = 0; i < numCluster; i++)
	{
		matrix_out.ifStay[i] = true;
	}
	for (int i = numCluster + 1; i < matrix_in.n; i++)
	{
		matrix_out.ifStay[i] = false;
	}

//    for (int i = 0; i < numFacets; ++i) {
//        printf("indexNearestCluster[%d] = \t%d\n", i, indexNearestCluster[i]);
//    }
//    
//    int *numFacetsInCluster = NULL;
//    numFacetsInCluster = new int[2 * matrix_out.n];
//    for (int i = 0; i < matrix_in.n; ++i) {
//        numFacetsInCluster[i] = 0;
//    }
//    int tmp;
//    int numMaxFacets = 0;
//    for (int i = 0; i < numFacets; ++i) {
//        tmp = ++numFacetsInCluster[indexNearestCluster[i]];
//        if (tmp > numMaxFacets) {
//            numMaxFacets = tmp;
//        }
//    }
//    for (int i = 0; i < matrix_in.n; ++i) {
//        if (numFacetsInCluster[i] > 0)
//            printf("numFacetsInCluster[%d] = %d\n", i, numFacetsInCluster[i]); 
//    }
//    printf("numMaxFacets = %d\n", numMaxFacets);
//    
//    
//    
//    printf("after\n");
//
//    int indCluster = 0;
//    for (int i = 0; i < matrix_in.n; ++i) {
//        
//        if (!matrix_in.ifStay[i]) {
//            continue;
//        }
//        int k = 0;
//        for (int j = 0; j < numFacets; ++j) {
//            if (indexNearestCluster[j] == i) {
//                indexSuperNew[k++] = j;
//            }
//        }
//        
//        SpherePoint spherePoint(nodeArray_in[i].cluster->P);
//        
//        ClusterNorm* newCluster = new ClusterNorm;
//        *newCluster = ClusterNorm(numFacetsInCluster[i], 
//                numFacetsInCluster[i], spherePoint, indexSuperNew, this);
//        nodeArray_out[indCluster++].cluster = newCluster;
//        
//        printf("newCluster : ");
//        newCluster->fprint(stdout);
//        printf("\n");
//        
//    }
//    
////    MatrixDistNorm* matrixNew = new MatrixDistNorm;
////    *matrixNew = MatrixDistNorm(numCluster);
////    matrixNew->build(numCluster, nodeArray_out);
////    matrixNew->n = numCluster;
////    matrix_out = *matrixNew;
//    
//    MatrixDistNorm* matrixNew = new MatrixDistNorm;
//    *matrixNew = MatrixDistNorm(numCluster);
////    matrixNew->build(numCluster, nodeArray_out);
//    matrixNew->n = numCluster;
//    matrix_out = *matrixNew;
//    
//    for (int i = 0; i < numCluster; ++i) {
//        if (nodeArray_out[i].cluster->num == 0) {
//            printf("matrix_out.ifStay[%d] = false\n", i);
//            matrix_out.ifStay[i] = false;
//        }
//    }
//    
//    
//    
//    if (indexNearestCluster != NULL) {
//        delete[] indexNearestCluster;
//        indexNearestCluster = NULL;
//    }
//
//    if (indexSuperNew[numFacets] != NULL) {
//        delete[] indexSuperNew;
//        indexSuperNew[numFacets] = NULL;
//    }
//
}
