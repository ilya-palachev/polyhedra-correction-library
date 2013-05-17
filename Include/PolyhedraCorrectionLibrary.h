#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <lapacke.h>
#include <list>
#include <assert.h>
#include <time.h>
#include <dlfcn.h>

#include "Vector3d.h"
#include "list_squares_method.h"
#include "Vector3d.h"
#include "array_operations.h"
#include "Gauss_string.h"

using namespace std;

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

class PCorrector;
class GlobalShadeCorrector;
class GSAssociator;

class PData;
class ShadeContourData;

class PDataConstructor;

#include "Polyhedron.h"
#include "VertexInfo.h"
#include "EdgeList.h"
#include "Facet.h"
#include "FutureFacet.h"
#include "EdgeSet.h"
#include "EdgeContourAssociation.h"
#include "Edge.h"
#include "SContour.h"
#include "SideOfContour.h"

#include "SpherePoint.h"
#include "ClusterNorm.h"
#include "MatrixDistNorm.h"
#include "TreeClusterNormNode.h"
#include "TreeClusterNorm.h"

#include "Correctors/PCorrector.h"
#include "Correctors/GlobalShadeCorrector.h"
#include "Correctors/GSAssociator.h"

#include "DataContainers/PData.h"
#include "DataContainers/ShadeContourData.h"

#include "DataConstructors/PDataConstructor.h"

#include "DebugPrint.h"
#include "DebugAssert.h"

