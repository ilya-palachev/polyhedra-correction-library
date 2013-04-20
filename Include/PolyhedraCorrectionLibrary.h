#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <lapacke.h>
#include <list>
#include <assert.h>
#include <time.h>

#include "Vector3d.h"
#include "list_squares_method.h"
#include "Vector3d.h"
#include "array_operations.h"
#include "Gauss_string.h"

#ifndef POLY_H
#define POLY_H
class Polyhedron;
class Facet;
class VertexInfo;
class EdgeList;
class FutureFacet;
class EdgeSet;
class Edge;
class SContour;
class SideOfContour;

class SpherePoint;
class ClusterNorm;
class MatrixDistNorm;
class TreeClusterNormNode;
class TreeClusterNorm;
#endif /* POLY_H */

#include "Polyhedron.h"
#include "VertexInfo.h"
#include "Facet.h"
#include "EdgeList.h"
#include "FutureFacet.h"
#include "EdgeSet.h"
#include "Edge.h"
#include "SContour.h"
#include "SideOfContour.h"
#include "EdgeContourAssociation.h"

#include "SpherePoint.h"
#include "ClusterNorm.h"
#include "MatrixDistNorm.h"
#include "TreeClusterNormNode.h"
#include "TreeClusterNorm.h"

#include "DebugPrint.h"
#include "DebugAssert.h"

