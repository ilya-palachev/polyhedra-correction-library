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
class VertexGrouper;

class PointShifter;
class PointShifterWeighted;
class PointShifterLinear;

class Verifier;

#include "Polyhedron/Polyhedron.h"
#include "SpecificPolyhedrons/Cube.h"
#include "SpecificPolyhedrons/Pyramid.h"
#include "SpecificPolyhedrons/Prism.h"
#include "SpecificPolyhedrons/CubeCutted.h"

#include "VertexInfo/VertexInfo.h"
#include "Correctors/Intersector/EdgeList/EdgeList.h"
#include "Facet/Facet.h"
#include "Correctors/Intersector/FutureFacet/FutureFacet.h"
#include "Correctors/Intersector/EdgeSet/EdgeSet.h"
#include "DataContainers/EdgeData/EdgeContourAssociation/EdgeContourAssociation.h"
#include "DataContainers/EdgeData/Edge/Edge.h"
#include "DataContainers/ShadeContourData/SContour/SContour.h"
#include "DataContainers/ShadeContourData/SideOfContour/SideOfContour.h"

#include "SpherePoint/SpherePoint.h"
#include "ClusterNorm/ClusterNorm.h"
#include "MatrixDistNorm/MatrixDistNorm.h"
#include "TreeClusterNormNode/TreeClusterNormNode.h"
#include "TreeClusterNorm/TreeClusterNorm.h"

#include "DataContainers/PData/PData.h"
#include "DataContainers/EdgeData/EdgeData.h"
#include "DataContainers/ShadeContourData/ShadeContourData.h"

#include "DataConstructors/PDataConstructor/PDataConstructor.h"
#include "DataConstructors/EdgeConstructor/EdgeConstructor.h"
#include "DataConstructors/ShadeContourConstructor/ShadeContourConstructor.h"

#include "Correctors/PCorrector/PCorrector.h"
#include "Correctors/Intersector/Intersector.h"
#include "Correctors/Intersector/FacetIntersector.h"
#include "Correctors/GlobalShadeCorrector/GlobalShadeCorrector.h"
#include "Correctors/GlobalShadeCorrector/GSAssociator/GSAssociator.h"
#include "Correctors/Coalescer/Coalescer.h"
#include "Correctors/VertexGrouper/VertexGrouper.h"
#include "Correctors/PointShifter/PointShifter.h"
#include "Correctors/PointShifterWeighted/PointShifterWeighted.h"
#include "Correctors/PointShifterLinear/PointShifterLinear.h"

#include "Verifier/Verifier.h"

#include "DebugPrint.h"
#include "DebugAssert.h"

