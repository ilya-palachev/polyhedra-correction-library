#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Polyhedron-class.h"
#include "VertexInfo.h"
#include "Facet.h"
#include "EdgeList.h"
#include "FutureFacet.h"
#include "EdgeSet.h"
#include "Edge.h"
#include "SContour.h"
#include "SideOfContour.h"


#include "Vector3d.h"
#include "list_squares_method.h"
#include "Vector3d.h"
#include "array_operations.h"
#include "Gauss_string.h"


#ifndef POLYHEDRON_H
#define POLYHEDRON_H

//#define DEBUG
//#define DEBUG1
#define OUTPUT
//#define TCPRINT //Печатать вывод из Polyhedron::test_consections()
#define EPS_SIGNUM 1e-15
#define EPS_COLLINEARITY 1e-14
#include "DebugPrint.h"


class Polyhedron;
class Facet;
class VertexInfo;
class EdgeList;
class FutureFacet;
class EdgeSet;
class Edge;
class SContour;
class SideOfContour;

#endif // POLYHEDRON_H
