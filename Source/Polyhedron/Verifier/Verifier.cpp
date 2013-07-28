/*
 * Verifier.cpp
 *
 *  Created on: 21.06.2013
 *      Author: ilya
 */

#include "PolyhedraCorrectionLibrary.h"

Verifier::Verifier() :
		polyhedron(),
		ifPrint(false)
{
}

Verifier::Verifier(Polyhedron* p) :
		polyhedron(p),
		ifPrint(true)
{
}

Verifier::Verifier(Polyhedron* p, bool _ifPrint) :
		polyhedron(p),
		ifPrint(_ifPrint)
{
}

Verifier::~Verifier()
{
}

#define EPS_PARALLEL 1e-15

int Verifier::countConsections()
{
	int count;
	if (ifPrint)
	{
		//    DEBUG_PRINT("Begin test_consections...\n");
	}

	count = 0;
	count += countInnerConsections();
	count += countOuterConsections();
	if (ifPrint)
	{
		if (count > 0)
		{
			DEBUG_PRINT("Total: %d consections found\n", count);
		}
	}
	return count;
}

int Verifier::countInnerConsections()
{
	int i, count;

	double *A, *b;
	Vector3d* vertex_old;

	A = new double[4];
	b = new double[2];
	vertex_old = new Vector3d[polyhedron->numFacets];

	count = 0;
	for (i = 0; i < polyhedron->numFacets; ++i)
	{
		count += countInnerConsectionsFacet(i, A, b, vertex_old);
	}
	if (ifPrint)
	{
		if (count > 0)
			DEBUG_PRINT("\tTotal: %d inner consections found\n", count);
	}
	if (A != NULL)
		delete[] A;
	if (b != NULL)
		delete[] b;
	if (vertex_old != NULL)
		delete[] vertex_old;

	return count;
}

int Verifier::countInnerConsectionsFacet(int fid, double* A,
		double* b, Vector3d* vertex_old)
{

	int i, j, nv, *index, count;
	Vector3d v;
	Plane plane;
	//Написано для случая, когда немного нарушена плоскостность грани
	//Перед проверкой на самопересечение все вершины грани проецируются на
	//плоскость наименьших квадратов (2012-03-12)
	//    DEBUG_PRINT("\tBegin test_inner_consections_facet(%d)\n", fid);

	nv = polyhedron->facets[fid].numVertices;
	index = polyhedron->facets[fid].indVertices;
	plane = polyhedron->facets[fid].plane;
	//    vertex_old = new Vector3d[nv];

	if (nv < 4)
	{
		return 0;
	}

	for (i = 0; i < nv; ++i)
	{
		v = polyhedron->vertices[index[i]];
		vertex_old[i] = v;
		polyhedron->vertices[index[i]] = plane.project(v);
	}

	count = 0;
	for (i = 0; i < nv; ++i)
	{
		for (j = i + 1; j < nv; ++j)
		{
			count += countInnerConsectionsPair(fid, index[i % nv],
					index[(i + 1) % nv], index[j % nv], index[(j + 1) % nv], A,
					b);
		}
	}
	if (ifPrint)
	{
		if (count > 0)
		{
			DEBUG_PRINT("\t\tIn facet %d: %d inner consections found\n", fid, count);
			polyhedron->facets[fid].my_fprint(stdout);
		}
	}
	//    if (count > 0)
	//        facet[fid].my_fprint(stdout);
	for (i = 0; i < nv; ++i)
	{
		polyhedron->vertices[index[i]] = vertex_old[i];
	}
	return count;
}

int Verifier::countInnerConsectionsPair(int fid, int id0,
		int id1, int id2, int id3, double* A, double* b)
{

	// fid - identificator of the facet
	// (id0, id1) - first edge
	// (id2, id3) - second edge
	double nx, ny, nz;
	double x0, x1, x2, x3, y0, y1, y2, y3, z0, z1, z2, z3;

	if (id0 < 0 || id1 < 0 || id2 < 0 || id2 < 0
			|| id0 >= polyhedron->numVertices
			|| id1 >= polyhedron->numVertices
			|| id2 >= polyhedron->numVertices
			|| id3 >= polyhedron->numVertices)
	{
		if (ifPrint)
		{
			DEBUG_PRINT(
					"\t\t\ttest_inner_consections_pair: Error. incorrect input\n");
		}
		return 0;
	}

	if ((id0 == id3 && id1 == id2) || (id0 == id2 && id1 == id3))
	{
		if (ifPrint)
		{
			DEBUG_PRINT("\t\t\t(%d, %d) and (%d, %d) are equal edges\n", id0, id1,
					id2, id3);
		}
		return 2;
	}

	if (id1 == id2)
	{
		if (qmod(
				(polyhedron->vertices[id1] - polyhedron->vertices[id0])
						% (polyhedron->vertices[id3] - polyhedron->vertices[id2])) < EPS_PARALLEL
				&& (polyhedron->vertices[id1] - polyhedron->vertices[id0])
						* (polyhedron->vertices[id3] - polyhedron->vertices[id2]) < 0)
		{
			if (ifPrint)
			{
				DEBUG_PRINT("\t\t\t(%d, %d) and (%d, %d) consect untrivially\n", id0,
						id1, id2, id3);
			}
			return 2;
		}
		else
		{
			return 0;
		}
	}

	if (id0 == id3)
	{
		if (qmod(
				(polyhedron->vertices[id1] - polyhedron->vertices[id0])
						% (polyhedron->vertices[id3] - polyhedron->vertices[id2])) < EPS_PARALLEL
				&& (polyhedron->vertices[id1] - polyhedron->vertices[id0])
						* (polyhedron->vertices[id3] - polyhedron->vertices[id2]) > 0)
		{
			if (ifPrint)
			{
				DEBUG_PRINT("\t\t\t(%d, %d) and (%d, %d) consect untrivially\n", id0,
						id1, id2, id3);
			}
			return 2;
		}
		else
		{
			return 0;
		}
	}

	if (qmod(
			(polyhedron->vertices[id1] - polyhedron->vertices[id0])
					% (polyhedron->vertices[id3] - polyhedron->vertices[id2])) < EPS_PARALLEL)
	{
		return 0;
	}

	nx = polyhedron->facets[fid].plane.norm.x;
	ny = polyhedron->facets[fid].plane.norm.y;
	nz = polyhedron->facets[fid].plane.norm.z;
	nx = fabs(nx);
	ny = fabs(ny);
	nz = fabs(nz);

	x0 = polyhedron->vertices[id0].x;
	y0 = polyhedron->vertices[id0].y;
	z0 = polyhedron->vertices[id0].z;

	x1 = polyhedron->vertices[id1].x;
	y1 = polyhedron->vertices[id1].y;
	z1 = polyhedron->vertices[id1].z;

	x2 = polyhedron->vertices[id2].x;
	y2 = polyhedron->vertices[id2].y;
	z2 = polyhedron->vertices[id2].z;

	x3 = polyhedron->vertices[id3].x;
	y3 = polyhedron->vertices[id3].y;
	z3 = polyhedron->vertices[id3].z;

	if (nz >= nx && nz >= ny)
	{
		A[0] = x1 - x0;
		A[1] = x2 - x3;
		A[2] = y1 - y0;
		A[3] = y2 - y3;
		b[0] = x2 - x0;
		b[1] = y2 - y0;
	}

	if (ny >= nx && ny >= nz)
	{
		A[0] = x1 - x0;
		A[1] = x2 - x3;
		A[2] = z1 - z0;
		A[3] = z2 - z3;
		b[0] = x2 - x0;
		b[1] = z2 - z0;
	}

	if (nx >= ny && nx >= nz)
	{
		A[0] = y1 - y0;
		A[1] = y2 - y3;
		A[2] = z1 - z0;
		A[3] = z2 - z3;
		b[0] = y2 - y0;
		b[1] = z2 - z0;
	}

	if (Gauss_string(2, A, b) == 0)
	{
		return 0;
	}

	if (b[0] > 0. && b[0] < 1. && b[1] > 0. && b[1] < 1.)
	{
		if (ifPrint)
		{
			DEBUG_PRINT("\t\t\tEdges (%d, %d) and (%d, %d) consect\n", id0, id1, id2,
					id3);
		}
		return 1;
	}
	else
		return 0;
}

int Verifier::countOuterConsections()
{
	int i, count;

	count = 0;
	for (i = 0; i < polyhedron->numFacets; ++i)
	{
		count += countOuterConsectionsFacet(i);
	}
	if (ifPrint)
	{
		if (count > 0)
			DEBUG_PRINT("\tTotal: %d outer consections found\n", count);
	}
	return count;
}

int Verifier::countOuterConsectionsFacet(int fid)
{
	int i, nv, *index, count;

	nv = polyhedron->facets[fid].numVertices;
	index = polyhedron->facets[fid].indVertices;

	count = 0;
	for (i = 0; i < nv; ++i)
	{
		count += countOuterConsectionsEdge(index[i % nv],
				index[(i + 1) % nv]);
	}
	if (ifPrint)
	{
		if (count > 0)
			DEBUG_PRINT("\t\tIn facet %d: %d outer consections found\n", fid, count);
	}
	return count;
}

int Verifier::countOuterConsectionsEdge(int id0, int id1)
{

	int j, count;

	count = 0;
	for (j = 0; j < polyhedron->numFacets; ++j)
	{
		//        DEBUG_PRINT("\tj = %d\n", j);
		count += countOuterConsectionsPair(id0, id1, j);
	}
	if (ifPrint)
	{
		if (count > 0)
			DEBUG_PRINT("\t\t\tFor edge (%d, %d): %d outer consections found\n", id0,
					id1, count);
	}
	return count;
}

int Verifier::countOuterConsectionsPair(int id0, int id1,
		int fid)
{

	int i, nv, *index;
	double AA, b, u, delta, alpha, sum;
	Vector3d A, A0, A1, normal;

	if (polyhedron->facets[fid].find_vertex(id0) >= 0 || polyhedron->facets[fid].find_vertex(id1) >= 0)
	{
		//        DEBUG_PRINT("contains.\n");
		return 0;
	}

	AA = polyhedron->facets[fid].plane.norm * (polyhedron->vertices[id0] - polyhedron->vertices[id1]);

	if (fabs(AA) < EPS_PARALLEL)
	{
		//        DEBUG_PRINT("parallel %le.\n", fabs(AA));
		return 0;
	}

	b = -polyhedron->facets[fid].plane.norm * polyhedron->vertices[id1] - polyhedron->facets[fid].plane.dist;
	u = b / AA;

	if (u > 1. || u < 0.)
	{
		//        DEBUG_PRINT("consection out of interval u = %lf.\n", u);
		return 0;
	}
	//    DEBUG_PRINT("\ttesting whether edge (%d, %d) consects facet %d.\n",
	//            id0, id1, fid);
	//    DEBUG_PRINT("\t\tu = %lf\n", u);

	A = u * polyhedron->vertices[id0] + (1. - u) * polyhedron->vertices[id1];
	//    DEBUG_PRINT("\t\tA = (%.2lf, %.2lf, %.2lf)\n", A.x, A.y, A.z);

	nv = polyhedron->facets[fid].numVertices;
	index = polyhedron->facets[fid].indVertices;
	normal = polyhedron->facets[fid].plane.norm;
	normal.norm(1.);
	//    DEBUG_PRINT("\t\t|n| = %lf,  ", sqrt(qmod(normal)));

	sum = 0.;
	for (i = 0; i < nv; ++i)
	{
		//        if (i > 0)
		//            DEBUG_PRINT("+");
		A0 = polyhedron->vertices[index[i % nv]] - A;
		A1 = polyhedron->vertices[index[(i + 1) % nv]] - A;
		delta = (A0 % A1) * normal;
		delta /= sqrt(qmod(A0) * qmod(A1));
		alpha = asin(delta);
		//        DEBUG_PRINT(" %lf ", alpha / M_PI * 180);
		sum += alpha;
	}

	//    DEBUG_PRINT(" = sum = %lf*\n", sum / M_PI * 180);

	if (fabs(sum) < 2 * M_PI)
	{
		return 0;
	}
	else
	{
		if (ifPrint)
		{
			DEBUG_PRINT("\t\t\t\tEdge (%d, %d) consects facet %d\n", id0, id1, fid);
		}
		return 1;
	}
}

int Verifier::checkEdges(EdgeData* edgeData)
{
	int numEdgesDesctructed = 0;
	list<Edge>::iterator edge = edgeData->edges.begin();
	for (int iEdge = 0; iEdge < edgeData->numEdges; ++iEdge)
	{
		if (!checkOneEdge(&*edge, edgeData))
		{
			++numEdgesDesctructed;
		}
		++edge;
	}
	return numEdgesDesctructed;
}

bool Verifier::checkOneEdge(Edge* edge, EdgeData* edgeData)
{
	DEBUG_START;
	DEBUG_PRINT("Checking edge [%d, %d], f0 = %d, f1 = %d", edge->v0, edge->v1,
			edge->f0, edge->f1);
	double dist = sqrt(qmod(polyhedron->vertices[edge->v0] -
			polyhedron->vertices[edge->v1]));
	DEBUG_PRINT("Distance between vertices = %lf", dist);

	Plane pi0 = polyhedron->facets[edge->f0].plane;
	Plane pi1 = polyhedron->facets[edge->f1].plane;

	int numIncidentFacets = polyhedron->vertexInfos[edge->v0].numFacets;
	int* incidentFacets = polyhedron->vertexInfos[edge->v0].indFacets;
	int f2;
	for (int iFacet = 0; iFacet < numIncidentFacets;)
	{
		int iFacetNext = (numIncidentFacets + iFacet + 1) % numIncidentFacets;

		if ( (incidentFacets[iFacet] == edge->f0 &&
				incidentFacets[iFacetNext] == edge->f1) ||
				(incidentFacets[iFacet] == edge->f1 &&
				incidentFacets[iFacetNext] == edge->f0) )
		{
			f2 = incidentFacets[(numIncidentFacets + iFacetNext + 1)
			                    % numIncidentFacets];
			break;
		}
		iFacet = iFacetNext;
	}
	Plane pi2 = polyhedron->facets[f2].plane;

	numIncidentFacets = polyhedron->vertexInfos[edge->v1].numFacets;
	incidentFacets = polyhedron->vertexInfos[edge->v1].indFacets;
	int f3;
	for (int iFacet = 0; iFacet < numIncidentFacets;)
	{
		int iFacetNext = (numIncidentFacets + iFacet + 1) % numIncidentFacets;

		if ( (incidentFacets[iFacet] == edge->f0 &&
				incidentFacets[iFacetNext] == edge->f1) ||
				(incidentFacets[iFacet] == edge->f1 &&
				incidentFacets[iFacetNext] == edge->f0) )
		{
			f3 = incidentFacets[(numIncidentFacets + iFacetNext + 1)
			                    % numIncidentFacets];
			break;
		}
		iFacet = iFacetNext;
	}
	Plane pi3 = polyhedron->facets[f3].plane;

	Vector3d A2; /* intersection of pi0, pi1, pi2 */
	intersection(pi0, pi1, pi2, A2);
	bool if_A2_under_pi3 = pi3 % A2 < 0;

	Vector3d A3; /* intersection of pi0, pi1, pi3 */
	intersection(pi0, pi1, pi3, A3);
	bool if_A3_under_pi2 = pi2 % A3 < 0;

	if (!if_A2_under_pi3)
	{
		ERROR_PRINT("Vertex %d (as intersection of facets %d, %d, %d)"
				" is higher than facet %d ",
				edge->v0, edge->f0, edge->f1, f2, f3);
	}
	else
	{
		DEBUG_PRINT("Vertex %d (as intersection of facets %d, %d, %d)"
				" is lower than facet %d ",
				edge->v0, edge->f0, edge->f1, f2, f3);
	}

	if (!if_A3_under_pi2)
	{
		ERROR_PRINT("Vertex %d (as intersection of facets %d, %d, %d)"
				" is higher than facet %d ",
				edge->v1, edge->f0, edge->f1, f3, f2);
	}
	else
	{
		DEBUG_PRINT("Vertex %d (as intersection of facets %d, %d, %d)"
				" is lower than facet %d ",
				edge->v1, edge->f0, edge->f1, f3, f2);
	}

#ifndef NDEBUG
	double distance = sqrt(qmod(polyhedron->vertices[edge->v0] -
			polyhedron->vertices[edge->v1]));
	DEBUG_PRINT("\t dist (v_%d, v_%d) = %le", edge->v0, edge->v1,
			distance);

	double movement0 = sqrt(qmod(polyhedron->vertices[edge->v0] - A2));
	DEBUG_PRINT("\t dist (v_%d, v_%d^{'}) = %le", edge->v0, edge->v0,
			movement0);

	double movement1 = sqrt(qmod(polyhedron->vertices[edge->v1] - A3));
	DEBUG_PRINT("\t dist (v_%d, v_%d^{'}) = %le", edge->v1, edge->v1,
			movement1);
#endif

	if (!if_A2_under_pi3 || !if_A3_under_pi2)
	{
		if (!reduceEdge(edge, edgeData))
		{
			DEBUG_END;
			return false;
		}
	}
	else
	{
		DEBUG_PRINT("Recalculating the position of vertex # %d", edge->v0);
		polyhedron->vertices[edge->v0] = A2;
		DEBUG_PRINT("Recalculating the position of vertex # %d", edge->v1);
		polyhedron->vertices[edge->v1] = A3;
	}

	DEBUG_END;
	return if_A2_under_pi3 && if_A3_under_pi2;
}

bool Verifier::reduceEdge(Edge* edge, EdgeData* edgeData)
{
	DEBUG_START;
	int iVertexReduced = edge->v1;
	int iVertexStayed = edge->v0;
	DEBUG_PRINT("Reduced edge: %d", iVertexReduced);
	DEBUG_PRINT(" Stayed edge: %d", iVertexStayed);

	VertexInfo* vertexInfoReduced = &polyhedron->vertexInfos[iVertexReduced];

	vertexInfoReduced->my_fprint_all(stderr);

	/* Stage 1. Update data structures "Facet" for those facets that contain
	 * "removed" vertex. */
	DEBUG_PRINT("Stage 1. Updating structures \"Facet\".");
	for (int iFacet = 0; iFacet < vertexInfoReduced->numFacets; ++iFacet)
	{
		int iFacetCurrent = vertexInfoReduced->indFacets[iFacet];
		DEBUG_PRINT("\tUpdating facet #%d.", iFacetCurrent);

		int iPositionReduced =
				vertexInfoReduced->indFacets[vertexInfoReduced->numFacets +
		                                    iFacet + 1];
		Facet* facetCurr = &polyhedron->facets[iFacetCurrent];

		int iPositionPrev = (facetCurr->numVertices + iPositionReduced - 1) %
				facetCurr->numVertices;
		int iPositionNext = (facetCurr->numVertices + iPositionReduced + 1) %
				facetCurr->numVertices;

		/* 2 cases: */

		/* 1). In case when "stayed" vertex LAYS in the current facet, delete
		 * "removed" vertex from facet. */

		/* 1a). The "stayed" vertex is EARLIER in the list of vertices than
		 * the "reduced" vertex. */
		if (facetCurr->indVertices[iPositionPrev] == iVertexStayed)
		{
			DEBUG_PRINT("Case 1a: \"stayed\" vertex lays in facet, "
					"it's earlier than reduced one");
			/* Let v2 be "stayed" vertex, v3 - "reduced" vertex
			 * Let numVertices == 6.
			 *
			 * Before:
			 * 0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18
			 * --------------------------------------------------------
			 * v0 v1 v2 v3 v4 v5 v0 f0 f1 f2 f3 f4 f5 p0 p1 p2 p3 p4 p5
			 *
			 *                        |
			 *                        |
			 *                        v
			 *
			 * After:
			 * 0  1  2     3  4  5  6  7     8  9  10 11 12    13 14 15
			 * --------------------------------------------------------
			 * v0 v1 v2    v4 v5 v0 f0 f1    f3 f4 f5 p0 p1    p3 p4 p5
			 *
			 *                   ^
			 *                   |
			 *                   |
			 *             need to be rewrited
			 *             if v0 is "reduced"
			 * */

			for (int i = iPositionReduced;
					i < facetCurr->numVertices + iPositionReduced - 1; ++i)
			{
				facetCurr->indVertices[i] = facetCurr->indVertices[i + 1];
			}

			for (int i = facetCurr->numVertices + iPositionReduced - 1;
					i < 2 * facetCurr->numVertices + iPositionReduced - 2; ++i)
			{
				facetCurr->indVertices[i] = facetCurr->indVertices[i + 2];
			}

			for (int i = 2 * facetCurr->numVertices + iPositionReduced - 2;
					i < 3 * facetCurr->numVertices - 2; ++i)
			{
				facetCurr->indVertices[i] = facetCurr->indVertices[i + 3];
			}

			--facetCurr->numVertices;
			facetCurr->indVertices[facetCurr->numVertices] =
					facetCurr->indVertices[0];
		}

		/* 1b). The "stayed" vertex is LATER in the list of vertices than
		 * the "reduced" vertex. */
		else if (facetCurr->indVertices[iPositionNext] == iVertexStayed)
		{
			DEBUG_PRINT("Case 1b: \"stayed\" vertex lays in facet, "
					"it's later than reduced one");
			/* Let v3 be "stayed" vertex, v2 - "reduced" vertex
			 * Let numVertices == 6.
			 *
			 * Before:
			 * 0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18
			 * --------------------------------------------------------
			 * v0 v1 v2 v3 v4 v5 v0 f0 f1 f2 f3 f4 f5 p0 p1 p2 p3 p4 p5
			 *
			 *                        |
			 *                        |
			 *                        v
			 *
			 * After:
			 * 0  1     2  3  4  5  6  7     8  9  10 11 12    13 14 15
			 * --------------------------------------------------------
			 * v0 v1    v3 v4 v5 v0 f0 f1    f3 f4 f5 p0 p1    p3 p4 p5
			 *
			 *                   ^
			 *                   |
			 *                   |
			 *             need to be rewrited
			 *             if v0 is "reduced"
			 * */

			for (int i = iPositionReduced;
					i < facetCurr->numVertices + iPositionReduced; ++i)
			{
				facetCurr->indVertices[i] = facetCurr->indVertices[i + 1];
			}

			for (int i = facetCurr->numVertices + iPositionReduced;
					i < 2 * facetCurr->numVertices + iPositionReduced - 1; ++i)
			{
				facetCurr->indVertices[i] = facetCurr->indVertices[i + 2];
			}

			for (int i = 2 * facetCurr->numVertices + iPositionReduced - 1;
					i < 3 * facetCurr->numVertices - 2; ++i)
			{
				facetCurr->indVertices[i] = facetCurr->indVertices[i + 3];
			}

			--facetCurr->numVertices;
			facetCurr->indVertices[facetCurr->numVertices] =
					facetCurr->indVertices[0];
		}

		/* 2). In case when "stayed" vertex DOES NOT LAY in the current facet,
		 * replace "reduced" vertex with "stayed" vertex. */
		else
		{
			DEBUG_PRINT("Case 1a: \"stayed\" vertex does not lay in facet");
			facetCurr->indVertices[iPositionReduced] = iVertexStayed;
		}
	}

	/* Stage 2. Update data structures "Edge" for those facets that contain
	 * "removed" vertex. */
	DEBUG_PRINT("Stage 2. Updating structures \"Edge\".");
	for (int iFacet = 0; iFacet < vertexInfoReduced->numFacets; ++iFacet)
	{
		int iVertexNeighbour =
				vertexInfoReduced->indFacets[vertexInfoReduced->numFacets +
				                             iFacet + 1];
		DEBUG_PRINT("\tUpdating edge [%d, %d]", iVertexReduced,
				iVertexNeighbour);
		list<Edge>::iterator edgeUpdated = edgeData->findEdge(iVertexReduced,
							iVertexNeighbour);
		if (iVertexNeighbour != iVertexStayed)
		{
			if (edgeUpdated->v0 == iVertexReduced &&
					edgeUpdated->v1 == iVertexNeighbour)
			{
				edgeUpdated->v0 = iVertexStayed;
			}
			else if (edgeUpdated->v0 == iVertexNeighbour &&
					edgeUpdated->v1 == iVertexReduced)
			{
				edgeUpdated->v1 = iVertexStayed;
			}
			else
			{
				ERROR_PRINT("Failed to find edge [%d, %d] in edge data.",
						iVertexReduced, iVertexNeighbour);
				DEBUG_PRINT("   Found edge: [%d, %d]",
						edgeUpdated->v0,
						edgeUpdated->v1);
				DEBUG_PRINT("Previous edge: [%d, %d]",
						edgeUpdated->v0,
						edgeUpdated->v1);
				DEBUG_PRINT("    Next edge: [%d, %d]",
						edgeUpdated->v0,
						edgeUpdated->v1);

				DEBUG_PRINT("Printing edge data:");
				list<Edge>::iterator edgePrinted = edgeData->edges.begin();
				for (int iEdge = 0; iEdge < edgeData->numEdges; ++iEdge)
				{
					DEBUG_PRINT("Edge #%d: [%d, %d]", iEdge,
							edgePrinted->v0, edgePrinted->v1);
					++edgePrinted;
				}
				DEBUG_END;
				return false;
			}
		}
		else
		{
			DEBUG_PRINT("\tThis edge must be deleted at all.");
			edge->assocList.clear();
		}
	}

	/* Stage 3. Rebuild data structure "VertexInfo" for all vertices
	 * which are neighbours of the "reduced" vertex. */
	DEBUG_PRINT("Stage 3. Updating structures \"VertexInfo\".");
	for (int iFacet = 0; iFacet < vertexInfoReduced->numFacets; ++iFacet)
	{
		int iVertexNeighbour =
				vertexInfoReduced->indFacets[vertexInfoReduced->numFacets +
				                             iFacet + 1];
		DEBUG_PRINT("\tUpdating vertexInfo #%d", iVertexNeighbour);
		polyhedron->vertexInfos[iVertexNeighbour].preprocess();
	}

	DEBUG_END;
	return true;
}
