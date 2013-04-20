/* 
 * File:   TreeClusterNormNode.cpp
 * Author: nk
 * 
 * Created on 1 Май 2012 г., 12:43
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Polyhedron.h"
//#include "list_squares_method.h"
//#include "Vector3d.h"
//#include "array_operations.h"
//#include "Gauss_string.h"

//#include "ClusterNorm.h"
//#include "MatrixDistNorm.h"
//#include "SpherePoint.h"
//#include "TreeClusterNorm.h"
//#include "TreeClusterNormNode.h"

TreeClusterNormNode::TreeClusterNormNode() :
				cluster(NULL),
				parent(NULL),
				child0(NULL),
				child1(NULL) {
}

TreeClusterNormNode::TreeClusterNormNode(
		const TreeClusterNormNode& orig) :
				cluster(orig.cluster),
				parent(orig.parent),
				child0(orig.child0),
				child1(orig.child1) {
}

TreeClusterNormNode::~TreeClusterNormNode() {
}

void TreeClusterNormNode::fprint(
		FILE* file,
		int level) {

//    if (level > 10)
//        return;

	for (int i = 0; i < level; ++i) {
		fprintf(file, " ");
	}
	cluster->fprint(file);
	fprintf(file, "\n");
	if (child0 != NULL) {
		child0->fprint(file, level + 1);
	}
	if (child1 != NULL) {
		child1->fprint(file, level + 1);
	}
}

//void TreeClusterNormNode::fprint_dendrogramma(FILE* file, int level) {
//    
//    int numLeft  = child0->cluster->num;
//    int numRight = child1->cluster->num;
//    
//    
//    fprintf(file, "|___");
//    for (int i = 1; i < numLeft; ++i) {
//        fprintf(file, "____");
//    }
//    for (int i = 0; i < numRight; ++i) {
//        fprintf(file, "    ");
//    }
//    
//
//    for (int i = 0; i < level; ++i) {
//        fprintf(file, " ");
//    }
//    cluster->fprint(file);
//    fprintf(file, "\n");
//    if (child0 != NULL) {
//        child0->fprint(file, level + 1);
//    }
//    if (child1 != NULL) {
//        child1->fprint(file, level + 1);
//    }
//}

void TreeClusterNormNode::fprint_dendrogramma_lev(
		FILE* file,
		int level,
		int fix) {
	if (level > fix) {
		return;
	}

	int numLeft = child0->cluster->num;
	int numRight = child1->cluster->num;

	if (level == fix) {
		fprintf(file, "|___");
		for (int i = 1; i < numLeft; ++i) {
			fprintf(file, "____");
		}
		for (int i = 0; i < numRight; ++i) {
			fprintf(file, "    ");
		}
	} else {
		child0->fprint_dendrogramma_lev(file, level + 1, fix);
		child1->fprint_dendrogramma_lev(file, level + 1, fix);
	}
}

