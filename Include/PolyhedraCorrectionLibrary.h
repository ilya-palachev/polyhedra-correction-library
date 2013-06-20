#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
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

/* Specific polyhedrons. For testing purposes. */
class Cube;
class Pyramid;
class Prism;
class CubeCutted;

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

class PData;
class EdgeData;
class ShadeContourData;

class PDataConstructor;
class EdgeConstructor;
class ShadeContourConstructor;

class PCorrector;

class Intersector;
class FacetIntersector;

class GlobalShadeCorrector;
class GSAssociator;

class Coalescer;

class PointShifter;
class PointShifterWeighted;

#include "Polyhedron.h"
#include "SpecificPolyhedrons/Cube.h"
#include "SpecificPolyhedrons/Pyramid.h"
#include "SpecificPolyhedrons/Prism.h"
#include "SpecificPolyhedrons/CubeCutted.h"

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

#include "DataContainers/PData.h"
#include "DataContainers/EdgeData.h"
#include "DataContainers/ShadeContourData.h"

#include "DataConstructors/PDataConstructor.h"
#include "DataConstructors/EdgeConstructor.h"
#include "DataConstructors/ShadeContourConstructor.h"

#include "Correctors/PCorrector.h"
#include "Correctors/Intersector.h"
#include "Correctors/FacetIntersector.h"
#include "Correctors/GlobalShadeCorrector.h"
#include "Correctors/GSAssociator.h"
#include "Correctors/Coalescer.h"
#include "Correctors/PointShifter.h"
#include "Correctors/PointShifterWeighted.h"

#include "DebugPrint.h"
#include "DebugAssert.h"

