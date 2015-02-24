/*
 * Copyright (c) 2009-2014 Ilya Palachev <iliyapalachev@gmail.com>
 *
 * This file is part of Polyhedra Correction Library.
 *
 * Polyhedra Correction Library is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Polyhedra Correction Library is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Polyhedra Correction Library.
 * If not, see <http://www.gnu.org/licenses/>.
 */

/* Includes. */

/* Standard includes: */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <assert.h>
#include <dlfcn.h>
#include <time.h>

/* Standard Template Library includes: */
#include <list>
#include <set>
#include <queue>
#include <algorithm>
#include <memory>

/* Includes from auxiliary external libraries (built-in): */
#include "Vector3d.h"
#include "LeastSquaresMethod.h"
#include "Vector3d.h"
#include "array_operations.h"
#include "Gauss_string.h"

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
class EdgeSetIntersected;
class Edge;
class EdgeComparison;
class SContour;
class SideOfContour;

class SpherePoint;
class ClusterNorm;
class MatrixDistNorm;
class TreeClusterNormNode;
class TreeClusterNorm;

class PData;
class EdgeData;
class ShadowContourData;

class PDataConstructor;
class EdgeConstructor;
class ShadowContourConstructor;

class PCorrector;

class Intersector;
class FacetIntersector;

class GlobalShadowCorrector;
class GSAssociator;

class Coalescer;
class VertexGrouper;

class PointShifter;
class PointShifterWeighted;
class PointShifterLinear;

class EdgeReducer;

class Verifier;

class PAnalyzer;
class SizeCalculator;
class SortedDouble;
class Clusterizer;
class SortedSetOfPairs;

class GraphDumperGEXF;


#include "Polyhedron/Polyhedron.h"
#include "Polyhedron/Facet/Facet.h"
#include "Polyhedron/VertexInfo/VertexInfo.h"
#include "Polyhedron/SpecificPolyhedrons/Cube.h"
#include "Polyhedron/SpecificPolyhedrons/Pyramid.h"
#include "Polyhedron/SpecificPolyhedrons/Prism.h"
#include "Polyhedron/SpecificPolyhedrons/CubeCutted.h"

#include "DataContainers/PData/PData.h"
#include "DataContainers/EdgeData/EdgeContourAssociation/EdgeContourAssociation.h"
#include "DataContainers/EdgeData/Edge/Edge.h"
#include "DataContainers/EdgeData/EdgeData.h"
#include "DataContainers/ShadowContourData/ShadowContourData.h"
#include "DataContainers/ShadowContourData/SContour/SContour.h"
#include "DataContainers/ShadowContourData/SideOfContour/SideOfContour.h"
#include "DataContainers/SupportFunctionData/SupportFunctionData.h"
#include "DataContainers/SupportFunctionEstimationData/GardnerKiderlenSupportMatrix.h"

#include "DataConstructors/PDataConstructor/PDataConstructor.h"
#include "DataConstructors/EdgeConstructor/EdgeConstructor.h"
#include "DataConstructors/ShadowContourConstructor/ShadowContourConstructor.h"

#include "Correctors/PCorrector/PCorrector.h"
#include "Correctors/Intersector/Intersector.h"
#include "Correctors/Intersector/FacetIntersector.h"
#include "Correctors/Intersector/EdgeList/EdgeList.h"
#include "Correctors/Intersector/FutureFacet/FutureFacet.h"
#include "Correctors/Intersector/EdgeSetIntersected/EdgeSetIntersected.h"
#include "Correctors/GlobalShadowCorrector/GlobalShadowCorrector.h"
#include "Correctors/GlobalShadowCorrector/GSAssociator/GSAssociator.h"
#include "Correctors/Coalescer/Coalescer.h"
#include "Correctors/VertexGrouper/VertexGrouper.h"
#include "Correctors/PointShifter/PointShifter.h"
#include "Correctors/PointShifterWeighted/PointShifterWeighted.h"
#include "Correctors/PointShifterLinear/PointShifterLinear.h"

#include "Correctors/EdgeReducer/EdgeReducer.h"
#include "Polyhedron/Verifier/Verifier.h"

#include "Analyzers/PAnalyzer/PAnalyzer.h"
#include "Analyzers/SizeCalculator/SizeCalculator.h"
#include "Analyzers/SizeCalculator/SortedDouble/SortedDouble.h"
#include "Analyzers/Clusterizer/Clusterizer.h"
#include "Analyzers/Clusterizer/SortedSetOfPairs/SortedSetOfPairs.h"
#include "Analyzers/Clusterizer/SpherePoint/SpherePoint.h"
#include "Analyzers/Clusterizer/ClusterNorm/ClusterNorm.h"
#include "Analyzers/Clusterizer/MatrixDistNorm/MatrixDistNorm.h"
#include "Analyzers/Clusterizer/TreeClusterNormNode/TreeClusterNormNode.h"
#include "Analyzers/Clusterizer/TreeClusterNorm/TreeClusterNorm.h"
#include "Analyzers/ShadowContourClusterizer/ShadowContourClusterizer.h"

#include "Dumpers/GraphDumperGEXF/GraphDumperGEXF.h"

#include "Recoverer/Recoverer.h"

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "PCLDumper.h"
