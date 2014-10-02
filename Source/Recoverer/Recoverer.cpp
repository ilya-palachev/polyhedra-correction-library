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

#include <cmath>
#include <set>
#include <fstream>
#include <boost/concept_check.hpp>

#include <Eigen/Dense>

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "Constants.h"
#include "DataContainers/ShadeContourData/ShadeContourData.h"
#include "DataContainers/ShadeContourData/SContour/SContour.h"
#include "Polyhedron/Facet/Facet.h"
#include "Polyhedron/VertexInfo/VertexInfo.h"
#include "Analyzers/SizeCalculator/SizeCalculator.h"
#include "Recoverer/Recoverer.h"
#include "Recoverer/CGALSupportFunctionEstimator.h"
#include "Recoverer/IpoptSupportFunctionEstimator.h"
#include "Recoverer/TsnnlsSupportFunctionEstimator.h"

using namespace std;

static void printEstimationReport(SparseMatrix Q, VectorXd h0, VectorXd h,
	RecovererEstimator estimator);

char *makeNameWithSuffix(const char *outputName, const char *suffix)
{
	char *name = (char*) malloc(strlen(outputName) + strlen(suffix) + 1);
	name[0] = '\0';
	strcat(name, outputName);
	strcat(name, suffix);
	return name;
}

static void checkPolyhedronIDs(Polyhedron_3 polyhedron)
{
#ifndef NDEBUG
	DEBUG_START;
	long int iVertex = 0;
	for (auto vertex = polyhedron.vertices_begin();
			vertex != polyhedron.vertices_end(); ++vertex)
	{
		ASSERT(iVertex++ == vertex->id);
	}

	long int iFacet = 0;
	for (auto facet = polyhedron.facets_begin();
		facet != polyhedron.facets_end(); ++facet)
	{
		ASSERT(iFacet++ == facet->id);
	}

	long int iHalfedge = 0;
	for (auto halfedge = polyhedron.halfedges_begin();
		halfedge != polyhedron.halfedges_end(); ++halfedge)
	{
		ASSERT(iHalfedge++ == halfedge->id);
	}
	DEBUG_END;
#endif
}

Recoverer::Recoverer() :
	outputName(NULL),
	estimator(CGAL_ESTIMATOR),
	ifBalancing(false),
	ifConvexifyContours(false),
	ifRegularize(false),
	ifScaleMatrix(false),
	distDirectionMinimal(0.),
	iXmax(0),
	iYmax(0),
	iZmax(0),
	vectorRegularizing(0., 0., 0.),
	mapID(NULL),
	mapIDsize(0),
	hvaluesInit(NULL),
	mapIDinverse(NULL),
	shadowDataInit(NULL),
	shadowDataPrep(NULL),
	supportDirections()
{
	DEBUG_START;
	DEBUG_END;
}

Recoverer::~Recoverer()
{
	DEBUG_START;
	if (mapID != NULL)
		delete[] mapID;
	if (hvaluesInit != NULL)
		delete[] hvaluesInit;
	if (mapIDinverse != NULL)
		delete[] mapIDinverse;
	DEBUG_END;
}

void Recoverer::initData(ShadeContourDataPtr SCData)
{
	DEBUG_START;
	shadowDataInit = SCData;
	ShadeContourDataPtr SCDataPrep(new
		ShadeContourData(SCData->polyhedron));
	shadowDataPrep = SCDataPrep;
	ASSERT(SCDataPrep->numContours == shadowDataPrep->numContours);
	shadowDataPrep->numContours = SCData->numContours;
	shadowDataPrep->contours = new SContour[SCData->numContours];
	DEBUG_END;
}

void Recoverer::setOutputName(char *name)
{
	DEBUG_START;
	ASSERT(name);
	if (outputName)
		free(outputName);
	outputName = name;
	DEBUG_END;
}

void Recoverer::enableBalancing(void)
{
	DEBUG_START;
	ifBalancing = true;
	DEBUG_END;
}

void Recoverer::enableContoursConvexification(void)
{
	DEBUG_START;
	ifConvexifyContours = true;
	DEBUG_END;
}

void Recoverer::disableContoursConvexification(void)
{
	DEBUG_START;
	ifConvexifyContours = false;
	DEBUG_END;
}

void Recoverer::enableRegularization(void)
{
	DEBUG_START;
	ifRegularize = true;
	DEBUG_END;
}

void Recoverer::enableMatrixScaling(void)
{
	DEBUG_START;
	ifScaleMatrix = true;
	DEBUG_END;
}

void Recoverer::setDistDirectionMinimal(double dist)
{
	DEBUG_START;
	distDirectionMinimal = dist;
	DEBUG_END;
}

void Recoverer::buildNaiveMatrix(ShadeContourDataPtr SCData)
{
	DEBUG_START;

	initData(SCData);

	SupportFunctionEstimationData* data =
			buildSFEData(SCData);

	ofstream fileMatrix;
	char *name = makeNameWithSuffix(outputName, ".support-matrix.mat");
	DEBUG_PRINT("Printing to %s", name);
	fileMatrix.open(name);
	ASSERT(fileMatrix);
	free(name);
	fileMatrix << data->supportMatrix();
	fileMatrix.close();

	ofstream fileVector;
	name = makeNameWithSuffix(outputName, ".support-vector.mat");
	fileVector.open(name);
	ASSERT(fileVector);
	free(name);
	fileVector << data->supportVector();
	fileVector.close();

	DEBUG_END;
}

PolyhedronPtr Recoverer::buildPolyhedronFromPlanes(vector<Plane> supportPlanes)
{
	DEBUG_START;
	/* 1. Map planes to dual space to obtain the set of points in it. */
	vector<Vector3d> supportPoints = mapPlanesToDualSpace(supportPlanes);

	/* 2. Construct convex hull in the dual space. */
	PolyhedronPtr polyhedronDual = constructConvexHull(supportPoints);

	/* 3. Map dual polyhedron to the primal space. */
	PolyhedronPtr polyhedron = buildDualPolyhedron(polyhedronDual);
	DEBUG_END;
	return polyhedron;
}

PolyhedronPtr Recoverer::buildNaivePolyhedron(ShadeContourDataPtr SCData)
{
	DEBUG_START;

	/* 1. Balance contours if it is required. */
	if (ifBalancing)
	{
		balanceAllContours(SCData);
	}

	initData(SCData);

	/* 2. Extract support planes from shadow contour data. */
	vector<Plane> supportPlanes = extractSupportPlanes(SCData);
	DEBUG_PRINT("Number of extracted support planes: %ld",
		(long unsigned int) supportPlanes.size());

	/* 3. Build polyhedron from extracted support planes. */
	PolyhedronPtr polyhedron = buildPolyhedronFromPlanes(supportPlanes);

	DEBUG_END;
	return polyhedron;
}

PolyhedronPtr Recoverer::buildDualNonConvexPolyhedron(ShadeContourDataPtr
		SCData)
{
	DEBUG_START;

	/* 1. Balance contours if it is required. */
	if (ifBalancing)
	{
		balanceAllContours(SCData);
	}

	initData(SCData);

	/* 2. Extract support planes from shadow contour data. */
	vector<Plane> supportPlanes = extractSupportPlanes(SCData);
	DEBUG_PRINT("Number of extracted support planes: %ld",
		(long unsigned int) supportPlanes.size());

	/* 3. Map planes to dual space to obtain the set of points in it. */
	vector<Vector3d> supportPoints = mapPlanesToDualSpace(supportPlanes);

	/* 4. Construct convex hull in the dual space. */
	PolyhedronPtr polyhedronDual = constructConvexHull(supportPoints);

	DEBUG_END;
	return polyhedronDual;
}

PolyhedronPtr Recoverer::buildMaybeDualContours(bool ifDual,
		ShadeContourDataPtr SCData)
{
	DEBUG_START;
	/* New polyhedron will have 1 facet for each shadow contour. */
        int numFacets = SCData->numContours;

	/* Count the total number of vertices required for polyhedron. */
	int numVertices = 0;
	list<vector<Vector3d>> verticesAll;
	for (int iContour = 0; iContour < SCData->numContours; ++iContour)
	{
		SContour* contour = &SCData->contours[iContour];
		auto DEBUG_VARIABLE normal = contour->plane.norm;
	        ASSERT((fpclassify(normal.x) != FP_ZERO
	                || fpclassify(normal.y) != FP_ZERO
	                || fpclassify(normal.z) != FP_ZERO)
	                && "normal is null vector");

		vector<Vector3d> vertices;
		if (ifDual)
		{
			auto planes = extractSupportPlanes(contour);
			vertices = mapPlanesToDualSpace(planes);
		}
		else
		{
			vertices = contour->getPoints();
		}
		verticesAll.push_back(vertices);
		numVertices += vertices.size();
	}

	PolyhedronPtr p(new Polyhedron(numVertices, numFacets));

	auto vertices = verticesAll.begin();
	int iVertex = 0;
	for (int iContour = 0; iContour < SCData->numContours; ++iContour)
	{
		ASSERT(vertices != verticesAll.end());
		SContour* contour = &SCData->contours[iContour];
		Facet *facet = new Facet(iContour, vertices->size(),
			contour->plane, NULL);
		p->facets[iContour] = *facet;

		int iSide = 0;
		for (auto vertex : *vertices)
		{
			p->vertices[iVertex] = vertex;
			p->facets[iContour].indVertices[iSide++] = iVertex;
			++iVertex;
		}

		/* Cycling vertex. */
		p->facets[iContour].indVertices[p->facets[iContour].numVertices] =
				p->facets[iContour].indVertices[0];
		++vertices;
	}

	p->set_parent_polyhedron_in_facets();

	DEBUG_END;
	return p;
}

PolyhedronPtr Recoverer::buildDualContours(ShadeContourDataPtr SCData)
{
	DEBUG_START;

	/* Balance contours if it is required. */
	if (ifBalancing)
	{
		balanceAllContours(SCData);
	}

	PolyhedronPtr p = buildMaybeDualContours(true, SCData);

	DEBUG_END;
	return p;
}

PolyhedronPtr Recoverer::buildContours(ShadeContourDataPtr SCData)
{
	DEBUG_START;

	/* Balance contours if it is required. */
	if (ifBalancing)
	{
		balanceAllContours(SCData);
	}

	PolyhedronPtr p = buildMaybeDualContours(false, SCData);

	DEBUG_END;
	return p;
}


vector<Plane> Recoverer::extractSupportPlanes(SContour* contour)
{
	DEBUG_START;
	vector<Plane> supportPlanes;
	auto DEBUG_VARIABLE normal = contour->plane.norm;
        ASSERT((fpclassify(normal.x) != FP_ZERO
                || fpclassify(normal.y) != FP_ZERO
                || fpclassify(normal.z) != FP_ZERO)
                && "normal is null vector");
	ASSERT(fabs(normal.z) < EPS_MIN_DOUBLE);

	if (ifConvexifyContours)
	{
		SContour *contourConv = contour->convexify();
		ASSERT(contour->id >= 0);
		ASSERT(shadowDataPrep);
		ASSERT(contour->id < shadowDataPrep->numContours);
		shadowDataPrep->contours[contour->id] = *contourConv;
		contour = contourConv;
	}
	ASSERT(contour);
	ASSERT(contour->sides);

	/* Iterate through the array of sides of current contour. */
	for (int iSide = 0; iSide < contour->ns; ++iSide)
	{
		SideOfContour* sideCurr = &contour->sides[iSide];
		ASSERT(sideCurr);
		/*
		 * Here the plane that is incident to points A1 and A2 of the
		 * current side and collinear to the vector of projection.
		 *
		 * TODO: Here should be the calculation of the best plane
		 * fitting these conditions. Current implementation can
		 * produce big errors.
		 */
		ASSERT(qmod(sideCurr->A1 - sideCurr->A2) > 0);
		
		Vector3d supportPlaneNormal = (sideCurr->A1 - sideCurr->A2) %
			normal;
		/* Normalization must be done here, not at higher level! */
		supportPlaneNormal.norm(1.);

		/*
		 * We do this, because CGAL::convex_hull_3 is buggy for 1e-15
		 * values.
		 */
		if (fabs(supportPlaneNormal.x) < 10. * EPS_MIN_DOUBLE)
		{
			supportPlaneNormal.x = 0.;
		}
		if (fabs(supportPlaneNormal.y) < 10. * EPS_MIN_DOUBLE)
		{
			supportPlaneNormal.y = 0.;
		}
		if (fabs(supportPlaneNormal.z) < 10. * EPS_MIN_DOUBLE)
		{
			supportPlaneNormal.z = 0.;
		}

		/*
		 * The only way to find right orientation of normal is to check
		 * that both points of the contour side lie at the positive
		 * halfspace of the plane.
		 */
		if (supportPlaneNormal * sideCurr->A1 < 0)
		{
			supportPlaneNormal = -supportPlaneNormal;
		}
		ASSERT(supportPlaneNormal * sideCurr->A1 > 0);
		ASSERT(supportPlaneNormal * sideCurr->A2 > 0);

		Plane supportPlane(supportPlaneNormal,
			- supportPlaneNormal * sideCurr->A1);

		supportPlanes.push_back(supportPlane);

		DEBUG_VARIABLE double error1 = supportPlane.norm * sideCurr->A1
			+ supportPlane.dist;
		DEBUG_VARIABLE double error2 = supportPlane.norm * sideCurr->A2
			+ supportPlane.dist;

		DEBUG_PRINT("   side #%d\t%le\t%le", iSide, error1, error2);

		/* TODO: Here should be more strict conditions. */
		ASSERT(fabs(error1) < 1000 * EPS_MIN_DOUBLE);
		ASSERT(fabs(error2) < 1000 * EPS_MIN_DOUBLE);
	}
	DEBUG_END;
	return supportPlanes;
}

vector<Plane> Recoverer::extractSupportPlanes(ShadeContourDataPtr SCData)
{
	DEBUG_START;
	vector<Plane> supportPlanes;

	/* Iterate through the array of contours. */
	for (int iContour = 0; iContour < SCData->numContours; ++iContour)
	{
		DEBUG_PRINT("contour #%d", iContour);
		SContour* contourCurr = &SCData->contours[iContour];
		auto DEBUG_VARIABLE normal = contourCurr->plane.norm;
	        ASSERT((fpclassify(normal.x) != FP_ZERO
	                || fpclassify(normal.y) != FP_ZERO
	                || fpclassify(normal.z) != FP_ZERO)
	                && "normal is null vector");
		/* Extract support planes from contour and try to push them */
		vector<Plane> supportPlanesPortion =
			extractSupportPlanes(contourCurr);
		for (auto &plane : supportPlanesPortion)
		{
			/* Plane is skipped if it already exists in array. */
			int iExisting = 0;
			bool ifExist = false;
			auto planePrev = supportPlanes.begin();
			for (; planePrev != supportPlanes.end(); ++planePrev)
			{
				auto direction = planePrev->norm;
				if ((fabs(direction.x - plane.norm.x)
					< 10. * EPS_MIN_DOUBLE) &&
					(fabs(direction.y - plane.norm.y)
					< 10. * EPS_MIN_DOUBLE) &&
					(fabs(direction.z - plane.norm.z)
					< 10. * EPS_MIN_DOUBLE))
				{
					ifExist = true;
					break;
				}
				++iExisting;
			}
			if (ifExist)
			{
				DEBUG_PRINT("! Aborting, since direction "
					"already exists in the directions "
					"array, it's #%d", iExisting);
				DEBUG_PRINT("Old value = %lf, new value = %lf",
					planePrev->dist, plane.dist);
				ASSERT(fabs(planePrev->dist - plane.dist)
					< 10. * EPS_MIN_DOUBLE);
				continue;
			}
			supportPlanes.push_back(plane);
		}
	}
	DEBUG_END;
	return supportPlanes;
}

vector<Vector3d> Recoverer::mapPlanesToDualSpace(vector<Plane> planes)
{
	DEBUG_START;
	vector<Vector3d> vectors;
	
	for (auto &plane : planes)
	{
		/*
		 * The transformation of polar duality works as follows:
		 *
		 * (ax + by + cz + d = 0) |--> (-a/d, -b/d, -c/d)
		 *
		 * and reverse.
		 */
		Vector3d vector = plane.norm;

		/* 3 multiplications is more efficient than 3 divisions: */
		ASSERT(fpclassify(plane.dist) != FP_ZERO);
		ASSERT(fabs(plane.dist) > EPS_MIN_DOUBLE);
		vector *= -1. / plane.dist;
		vectors.push_back(vector);

		DEBUG_PRINT("map (%lf) x + (%lf) y + (%lf) z + (%lf) = 0  |--> "
			"(%lf, %lf, %lf)", plane.norm.x, plane.norm.y, plane.norm.z,
			plane.dist, vector.x, vector.y, vector.z);
	}
	
	return vectors;
	DEBUG_END;
}

/**
 * A functor computing the plane containing a triangular facet
 */
struct Plane_from_facet
{
	/*
	 * Calculate the plane of facet as the plane that contains 3 sequential
	 * points of the facet.
	 *
	 * TODO: It can produce big errors.
	 * Here should be more accurate calculation of the best fitting plane
	 * for the given set of points.
	 */
	Polyhedron_3::Plane_3 operator()(Polyhedron_3::Facet& f)
	{
		Polyhedron_3::Halfedge_handle h = f.halfedge();
		Point_3 point0 = h->vertex()->point();
		Point_3 point1 = h->next()->vertex()->point();
		Point_3 point2 = h->opposite()->vertex()->point();
		return Polyhedron_3::Plane_3(point0, point1, point2);
	}
};

static CGALpointsSet convertPCLpointsToCGALpoints(vector<Vector3d> pointsPCL)
{
	DEBUG_START;
	/* Convert Vector3d objects to Point_3 objects. */
	CGALpointsSet pointsCGAL(PCL_Point_3_comparer);

	long int i = 0;
	for (auto &point : pointsPCL)
	{
		PCL_Point_3 pointCGAL(point.x, point.y, point.z, i++);
		DEBUG_VARIABLE long int sizePrev = pointsCGAL.size();
		pointsCGAL.insert(pointCGAL);
		ASSERT((long int) pointsCGAL.size() == sizePrev + 1);

		ASSERT(fabs(point.x - pointCGAL.x()) < EPS_MIN_DOUBLE);
		ASSERT(fabs(point.y - pointCGAL.y()) < EPS_MIN_DOUBLE);
		ASSERT(fabs(point.z - pointCGAL.z()) < EPS_MIN_DOUBLE);
	}
	DEBUG_PRINT("There are %ld PCL points and %ld CGAL points",
		(long unsigned int) pointsPCL.size(),
		(long unsigned int) pointsCGAL.size());
	auto itPointCGAL = pointsCGAL.begin();
	do
	{
		auto DEBUG_VARIABLE pointCGAL = *itPointCGAL;
		++itPointCGAL;
		if (itPointCGAL != pointsCGAL.end())
		{
			auto DEBUG_VARIABLE pointCGALNext = *itPointCGAL;
			ASSERT(pointCGAL <= pointCGALNext);
		}
		else break;
	} while(1);
	ASSERT(pointsCGAL.size() == pointsPCL.size());
	DEBUG_END;
	return pointsCGAL;
}

static Polyhedron_3 convexHullCGAL(CGALpointsSet pointsCGAL)
{
	DEBUG_START;
	Polyhedron_3 poly;

#ifndef NDEBUG
	long i = 0;
	for (auto point = pointsCGAL.begin(); point != pointsCGAL.end();
		++point)
	{
		DEBUG_PRINT("processed point #%ld: (%le, %le, %le)", i,
			point->x(), point->y(), point->z());
		++i;
	}
#endif

	/* Use the algorithm of standard STATIC convex hull. */
	CGAL::convex_hull_3(pointsCGAL.begin(), pointsCGAL.end(), poly);

	/* Calculte equations of planes for each facet. */
	std::transform(poly.facets_begin(), poly.facets_end(), poly.planes_begin(),
			Plane_from_facet());

#ifndef NDEBUG
	i = 0;
	for (auto vertex = poly.vertices_begin();
		vertex != poly.vertices_end(); ++vertex)
	{
		auto point = vertex->point();
		DEBUG_PRINT("convex hull's point #%ld: (%le, %le, %le)", i,
			point.x(), point.y(), point.z());

		/* Check that all generated points are equal. */
		long iPrev = 0;
		for (auto vertexPrev = poly.vertices_begin();
			vertexPrev != vertex; ++vertexPrev)
		{
			auto pointPrev = vertexPrev->point();
			if ((fabs(point.x() - pointPrev.x())
				< 10. * EPS_MIN_DOUBLE)
				&& (fabs(point.y() - pointPrev.y())
				< 10. * EPS_MIN_DOUBLE)
				&& (fabs(point.z() - pointPrev.z())
				< 10. * EPS_MIN_DOUBLE))
			{
				DEBUG_PRINT("Points %ld and %ld are equal!",
					iPrev, i);
				DEBUG_PRINT("point #%ld = (%le, %le, %le)",
					iPrev, pointPrev.x(), pointPrev.y(),
					pointPrev.z());
				DEBUG_PRINT("point #%ld = (%le, %le, %le)",
					i, point.x(), point.y(), point.z());
				ASSERT(0 && "Equal points generated by convex "
					"hull!!!");
			}
		}
		++i;
	}
#endif

	DEBUG_PRINT("Convex hull of %ld points contains %ld extreme points.",
		(long unsigned int) pointsCGAL.size(),
		(long unsigned int) poly.size_of_vertices());
	
	DEBUG_PRINT("Output polyhedron contains %ld vertices, %ld halfedges and "
		"%ld facets",
		(long unsigned int) poly.size_of_vertices(),
		(long unsigned int) poly.size_of_halfedges(),
		(long unsigned int) poly.size_of_facets());
	DEBUG_END;
	return poly;
}

static void assignPolyhedronIDs(Polyhedron_3 *poly)
{
	DEBUG_START;
	/* Assign vertex IDs that will be used later. */
	int i = 0;
	for (auto vertex = poly->vertices_begin(); vertex != poly->vertices_end();
		 ++vertex)
	{
		vertex->id = i++;
	}

	/* Assign facet IDs that will be used later. */
	i = 0;
	for (auto facet = poly->facets_begin(); facet != poly->facets_end(); ++facet)
	{
		facet->id = i++;
	}
	
	/* Assert halfedge IDs that will be checked later. */
	i = 0;
	for (auto halfedge = poly->halfedges_begin();
		 halfedge != poly->halfedges_end(); ++halfedge)
	{
		halfedge->id = i++;
	}
	DEBUG_END;
}

static std::set<PCL_Point_3> getPoints(Polyhedron_3 poly)
{
	DEBUG_START;
	std::set<PCL_Point_3> pointsHull;
	int i = 0;
	for (auto vertex = poly.vertices_begin(); vertex != poly.vertices_end();
		 ++vertex)
	{
		auto point = vertex->point();
		pointsHull.insert(PCL_Point_3(point.x(), point.y(), point.z(),
			i++));
	}
	DEBUG_END;
	return pointsHull;
}

void Recoverer::constructIDmaps(std::set<PCL_Point_3> pointsHull,
	CGALpointsSet pointsCGAL)
{
	DEBUG_START;
	mapID = new long int[pointsCGAL.size()];
	mapIDsize = pointsCGAL.size();
	for (long int i = 0; i < (long int) pointsCGAL.size(); ++i)
	{
		mapID[i] = INT_NOT_INITIALIZED;
	}
	mapIDinverse = new std::list<long int>[pointsHull.size()];
	auto pointHull = pointsHull.begin();
	auto point = pointsCGAL.begin();
	int i = 0;
	double minDist = std::numeric_limits<double>::max();
	double numConsidered = 0;
	auto pointHullMin = pointsHull.end();
	while (point != pointsCGAL.end())
	{
		double dist = (*point - *pointHull).squared_length();
		if (dist < EPS_MIN_DOUBLE)
		{
			DEBUG_PRINT("----- Map case found!");
			mapID[point->id] = pointHull->id;
			mapIDinverse[pointHull->id].push_back(point->id);
			++point;
			++i;
			numConsidered = 0;
			minDist = std::numeric_limits<double>::max();
		}
		else
		{
			++pointHull;
			if (pointHull == pointsHull.end())
				pointHull = pointsHull.begin();
			++numConsidered;
			if (numConsidered >= pointsHull.size())
			{
				ASSERT(pointHullMin != pointsHull.end());
				DEBUG_PRINT("----- Forcing map to nearest "
					"hull point #%ld = (%lf, %lf, %lf), "
					"minDist is %lf", pointHullMin->id,
					pointHullMin->x(), pointHullMin->y(),
					pointHullMin->z(), minDist);
				mapID[point->id] = pointHullMin->id;
				mapIDinverse[pointHullMin->id].push_back(point->id);
				++point;
				++i;
				numConsidered = 0;
				minDist = std::numeric_limits<double>::max();
			}
			else if (dist < minDist)
			{
				minDist = dist;
				pointHullMin = pointHull;
			}
		}

	}

	if (estimator == ZERO_ESTIMATOR)
	{
		DEBUG_END;
		return;
	}

	for (long int i = 0; i < (long int) pointsCGAL.size(); ++i)
	{
		ASSERT(mapID[i] != INT_NOT_INITIALIZED);
	}

	for (long int i = 0; i < (long int) pointsHull.size(); ++i)
	{
		if (mapIDinverse[i].size() > 1)
		{
			DEBUG_PRINT("Hull's point #%ld has %ld corresponding "
				"initial points.",
				i, mapIDinverse[i].size());
		}
	}
	DEBUG_END;
}

Polyhedron_3 Recoverer::constructHullAndIDmaps(vector<Vector3d> pointsPCL)
{
	DEBUG_START;

	/* Construct the sorted set of CGAL points from PCL points. */
	auto pointsCGAL = convertPCLpointsToCGALpoints(pointsPCL);

	/* Construct convex hull using CGAL library. */
	Polyhedron_3 poly = convexHullCGAL(pointsCGAL);

	/* Assign IDs to the parts of obtained polyhedron. */
	assignPolyhedronIDs(&poly);

	/* Get vertex points of the obtained polyhedron. */
	auto pointsHull = getPoints(poly);

	/*
	 * Now construct 2 maps: direct and inverse, between initial points and
	 * extreme points of their recently obtained convex hull.
	 */
	constructIDmaps(pointsHull, pointsCGAL);

	return poly;
}

Polyhedron_3 Recoverer::constructConvexHullCGAL(vector<Vector3d> pointsPCL)
{
	DEBUG_START;

	/* Construct the sorted set of CGAL points from PCL points. */
	auto pointsCGAL = convertPCLpointsToCGALpoints(pointsPCL);

	/* Construct convex hull using CGAL library. */
	Polyhedron_3 poly = convexHullCGAL(pointsCGAL);

	/* Assign IDs to the parts of obtained polyhedron. */
	assignPolyhedronIDs(&poly);

	return poly;
}

PolyhedronPtr Recoverer::constructConvexHull (vector<Vector3d> points)
{
	DEBUG_START;
	Polyhedron_3 poly = constructConvexHullCGAL(points);

	/* Convert CGAL polyhedron to PCL polyhedron: */
	PolyhedronPtr polyhedronDualPCL(new Polyhedron(poly));

	polyhedronDualPCL->set_parent_polyhedron_in_facets();
	polyhedronDualPCL->my_fprint(stdout);
	
	char *name = makeNameWithSuffix(outputName, ".dual-polyhedron.ply");
	polyhedronDualPCL->fprint_ply_autoscale(DEFAULT_MAX_COORDINATE,
		name, "dual-polyhedron");
	free(name);
	DEBUG_END;
	return polyhedronDualPCL;
}

PolyhedronPtr Recoverer::buildDualPolyhedron(PolyhedronPtr p)
{
	DEBUG_START;

	/* Preprocess the initial polyhedron. */
	p->preprocessAdjacency();

	/* Allocate dual polyhedron. */
	PolyhedronPtr pDual(new Polyhedron());

	pDual->numVertices = p->numFacets;
	pDual->numFacets = p->numVertices;

	pDual->vertices = new Vector3d[pDual->numVertices];
	pDual->facets = new Facet[pDual->numFacets];

	/* Prepare and map the vector of planes to vector of points. */
	vector<Plane> planes;
	for (int iFacet = 0; iFacet < p->numFacets; ++iFacet)
	{
		planes.push_back(p->facets[iFacet].plane);
	}

	vector<Vector3d> vertices = mapPlanesToDualSpace(planes);

	int iVertex = 0;
	for (auto &vertex : vertices)
	{
		pDual->vertices[iVertex++] = vertex;
	}

	/*
	 * Create dual facets using the information computed during
	 * pre-processing.
	 */
	for (int iVertex = 0; iVertex < p->numVertices; ++iVertex)
	{
		Facet *facetCurr = &pDual->facets[iVertex];
		facetCurr->numVertices = p->vertexInfos[iVertex].numFacets;
		facetCurr->indVertices = new int[3 * facetCurr->numVertices + 1];
		for (int iFacet = 0; iFacet < facetCurr->numVertices; ++iFacet)
		{
			facetCurr->indVertices[iFacet] =
				p->vertexInfos[iVertex].indFacets[iFacet];
		}
	}

	/* Preprocess dual polyhedron. */
	pDual->set_parent_polyhedron_in_facets();
	pDual->preprocessAdjacency();
	pDual->my_fprint(stdout);

	char *name = makeNameWithSuffix(outputName, ".primal-debug.ply");
	pDual->fprint_ply_autoscale(DEFAULT_MAX_COORDINATE,
		name, "primal-polyhedron");
	free(name);
	DEBUG_END;
	return pDual;
}

void Recoverer::shiftAllContours(ShadeContourDataPtr SCData, Vector3d shift)
{
	DEBUG_START;
	ASSERT(isfinite(shift.x));
	ASSERT(isfinite(shift.y));
	ASSERT(isfinite(shift.z));
	for (int iContour = 0; iContour < SCData->numContours; ++iContour)
	{
		SContour* contourCurr = &SCData->contours[iContour];
		for (int iSide = 0; iSide < contourCurr->ns; ++iSide)
		{
			SideOfContour* sideCurr = &contourCurr->sides[iSide];
			sideCurr->A1 += shift;
			sideCurr->A2 += shift;
		}
		/*
		 * If we want new plane to include point (x + xt, y + yt, z + zt) for
		 * each point (x, y, z) that is included in plane
		 * a * x + b * y + c * z + d = 0
		 * then its free coefficient must become equal to
		 * d - a * xt - b * yt - c * zt
		 *
		 * TODO: For our case usually we shift points in vertical planes on a
		 * vertical vector, thus, the plane will not actually move.
		 */
		contourCurr->plane.dist -= contourCurr->plane.norm * shift;
	}
	DEBUG_END;
}

void Recoverer::balanceAllContours(ShadeContourDataPtr SCData)
{
	DEBUG_START;
	/* Construct polyhedron consisting of contours as facets. */
	PolyhedronPtr p = buildMaybeDualContours(false, SCData);

	/* Calculate the mass center of contours. */
	SizeCalculator* sizeCalculator = new SizeCalculator(p);
	Vector3d center = sizeCalculator->calculateSurfaceCenter();
	DEBUG_PRINT("Center of contours = (%lf, %lf, %lf)",
		center.x, center.y, center.z);

	/* Shift all contours on z component of the vector of mass center. */
	Vector3d ez(0., 0., 1.);
	shiftAllContours(SCData, - (ez * center) * ez);
	DEBUG_END;
}

/**
 * Stores the handles of vertexes that form a tetrahedron.
 */
typedef struct
{
	Polyhedron_3::Vertex_handle v0, v1, v2, v3;
} TetrahedronVertex;

#ifndef NDEBUG
/**
 * Counts the number of covering tetrahedrons for a given polyhedron.
 *
 * @param polyhedron	Convex hull of the set of directions
 */
static unsigned long int countCoveringTetrahedrons(Polyhedron_3& polyhedron)
{
	DEBUG_START;

	/*
	 * Test: Check whether the proper number of tetrahedrons has been found.
	 *
	 * First, calcualate the sum of vertex degrees
	 */
	unsigned long int degreeSum = 0;
	long int iVertex DEBUG_VARIABLE = 0;
	for (auto vertex = polyhedron.vertices_begin();
			vertex != polyhedron.vertices_end(); ++vertex)
	{
		ASSERT(iVertex++ == vertex->id);
		degreeSum += vertex->vertex_degree();
	}

	/* Second, the number of edges is a half of degrees sum. */
	unsigned long int numTetrahedrons = degreeSum / 2;

	/*
	 * Third, each of 3 edges incident to any vertex of degree 3, share the same
	 * tetrahedron, so we need to decrease the number of tetrahedrons by 2.
	 */
	for (auto vertex = polyhedron.vertices_begin();
			vertex != polyhedron.vertices_end(); ++vertex)
	{
		unsigned long int degree = vertex->vertex_degree();
		if (degree == 3)
		{
			numTetrahedrons -= 2;
		}
	}

	DEBUG_PRINT("Counted number of tetrahedrons must be %ld", numTetrahedrons);

	DEBUG_END;
	return numTetrahedrons;
}
#endif

/**
 * Finds covering tetrahedrons for a given polyhedron.
 *
 * @param polyhedron	Convex hull of the set of directions
 */
static list<TetrahedronVertex> findCoveringTetrahedrons(
		Polyhedron_3& polyhedron)
{
	DEBUG_START;

	/* Check that all facets of the polyhedron are triangles. */
	for (auto halfedge = polyhedron.halfedges_begin();
			halfedge != polyhedron.halfedges_end(); ++halfedge)
	{
		if (!halfedge->is_triangle())
		{
			ERROR_PRINT("Facet #%ld is not triangle, it has %ld edges.",
					(long int) halfedge->facet()->id,
					(long int) halfedge->facet_degree());
			DEBUG_END;
			exit(EXIT_FAILURE);
		}
	}

	auto comparer = [](TetrahedronVertex a, TetrahedronVertex b)
	{
		long int a0 = a.v0->id, a1 = a.v1->id, a2 = a.v2->id, a3 = a.v3->id;
		long int b0 = b.v0->id, b1 = b.v1->id, b2 = b.v2->id, b3 = b.v3->id;

		if (a0 < b0)
		{
			return true;
		}
		else if (a0 == b0)
		{
			if (a1 < b1)
			{
				return true;
			}
			else if (a1 == b1)
			{
				if (a2 < b2)
				{
					return true;
				}
				else if (a2 == b2)
				{
					if (a3 < b3)
					{
						return true;
					}
				}
			}
		}
		return false;
	};


	set<TetrahedronVertex, decltype(comparer)> tetrahedrons(comparer);

	auto vertexComparer = [](Polyhedron_3::Vertex_handle a,
			Polyhedron_3::Vertex_handle b)
	{
		return a->id < b->id;
	};

	set<Polyhedron_3::Vertex_handle, decltype(vertexComparer)>
				vertexSet(vertexComparer);

	for (auto halfedge = polyhedron.halfedges_begin();
			halfedge != polyhedron.halfedges_end(); ++halfedge)
	{

		vertexSet.clear();

		vertexSet.insert(halfedge->vertex());
		vertexSet.insert(halfedge->prev()->vertex());
		vertexSet.insert(halfedge->next()->vertex());
		vertexSet.insert(halfedge->opposite()->next()->vertex());

		ASSERT(vertexSet.size() == 4);

		TetrahedronVertex tetrahedron;
		auto itVertex = vertexSet.begin();
		tetrahedron.v0 = *(itVertex++);
		tetrahedron.v1 = *(itVertex++);
		tetrahedron.v2 = *(itVertex++);
		tetrahedron.v3 = *(itVertex++);

		tetrahedrons.insert(tetrahedron);
	}

	for (auto vertex = polyhedron.vertices_begin();
		vertex != polyhedron.vertices_end(); ++vertex)
	{
		DEBUG_PRINT("hull_v[%ld] = (%lf, %lf, %lf)", vertex->id,
			vertex->point().x(), vertex->point().y(),
			vertex->point().z());
	}

	/*
	 * Construct the list from sorted set and return it.
	 */
	list<TetrahedronVertex> listTetrahedrons;
	long int DEBUG_VARIABLE i = 0;
	for (auto itTetrahedron = tetrahedrons.begin();
			itTetrahedron != tetrahedrons.end(); ++itTetrahedron)
	{
		listTetrahedrons.push_back(*itTetrahedron);
		DEBUG_PRINT("Tetrahedron #%ld: [%ld %ld %ld %ld]", i++,
			itTetrahedron->v0->id, itTetrahedron->v1->id,
			itTetrahedron->v2->id, itTetrahedron->v3->id);
	}

	DEBUG_END;
	return listTetrahedrons;
}

static double determinantEigen(Vector_3 v1, Vector_3 v2, Vector_3 v3)
{
	DEBUG_START;
	Eigen::MatrixXd A(5, 5);
	A << v1.x(), v1.y(), v1.z(), 0., 0.,
		v2.x(), v2.y(), v2.z(), 0., 0.,
		v3.x(), v3.y(), v3.z(), 0., 0.,
		0., 0., 0., 1., 0.,
		0., 0., 0., 0., 1.;
#ifndef NDEBUG
	std::cerr << A << std::endl;
#endif
	double det = A.determinant();
	DEBUG_PRINT("det calculated by Eigen is: %le", det);
	DEBUG_END;
	return det;
}

/**
 * Builds matrix of constraints from the polyhedron (which represents a convex
 * hull of the set of directions for which the support values are given).
 *
 * @param polyhedron	Convex hull of the set of directions
 * @param numConditions	The number of constraints (output)
 * @param matrix		The matrix of constraints (output)
 */
static SparseMatrix buildMatrixByPolyhedron(Polyhedron_3 polyhedron,
		bool ifScaleMatrix, double distDirectionMinimal)
{
	DEBUG_START;

	int numHvalues = polyhedron.size_of_vertices();

	auto listTetrahedrons = findCoveringTetrahedrons(polyhedron);

#ifndef NDEBUG
	/* Check whether the proper number of tetrahedrons has been found. */
	ASSERT(listTetrahedrons.size() == countCoveringTetrahedrons(polyhedron));
#endif

	int numConditions = listTetrahedrons.size();
	DEBUG_PRINT("Found %d covering tetrahedrons", numConditions);

	/*
	 * Create Eigen sparse matrix with
	 * N = numHvalues columns
	 * M = numConditions rows
	 */
	SparseMatrix matrix(numConditions, numHvalues);
	typedef Eigen::Triplet<double> Triplet;
	std::vector<Triplet> triple;
	triple.reserve(4 * numConditions);

	int iCondition = 0;
	for (auto &tetrahedron : listTetrahedrons)
	{
		DEBUG_PRINT("---- Preparing %d-th condition ----", iCondition);
		Point_3 zero(0., 0., 0.);
		Vector_3 u1(zero, tetrahedron.v0->point());
		DEBUG_PRINT("u1 = (%le, %le, %le)", u1.x(), u1.y(), u1.z());
		Vector_3 u2(zero, tetrahedron.v1->point());
		DEBUG_PRINT("u2 = (%le, %le, %le)", u2.x(), u2.y(), u2.z());
		Vector_3 u3(zero, tetrahedron.v2->point());
		DEBUG_PRINT("u3 = (%le, %le, %le)", u3.x(), u3.y(), u3.z());
		Vector_3 u4(zero, tetrahedron.v3->point());
		DEBUG_PRINT("u4 = (%le, %le, %le)", u4.x(), u4.y(), u4.z());

		int iVertex1 = tetrahedron.v0->id;
		DEBUG_PRINT("iVertex1 = %d", iVertex1);
		int iVertex2 = tetrahedron.v1->id;
		DEBUG_PRINT("iVertex2 = %d", iVertex2);
		int iVertex3 = tetrahedron.v2->id;
		DEBUG_PRINT("iVertex3 = %d", iVertex3);
		int iVertex4 = tetrahedron.v3->id;
		DEBUG_PRINT("iVertex4 = %d", iVertex4);

		DEBUG_PRINT("u1 - u2 = (%le, %le, %le)", u1.x() - u2.x(),
			u1.y() - u2.y(), u1.z() - u2.z());
		double d12 = (u1 - u2).squared_length();
		DEBUG_PRINT("||u1 - u2|| = %le", d12);

		DEBUG_PRINT("u1 - u3 = (%le, %le, %le)", u1.x() - u3.x(),
			u1.y() - u3.y(), u1.z() - u3.z());
		double d13 = (u1 - u3).squared_length();
		DEBUG_PRINT("||u1 - u3|| = %le", d13);

		DEBUG_PRINT("u1 - u4 = (%le, %le, %le)", u1.x() - u4.x(),
			u1.y() - u4.y(), u1.z() - u4.z());
		double d14 = (u1 - u4).squared_length();
		DEBUG_PRINT("||u1 - u4|| = %le", d14);

		DEBUG_PRINT("u2 - u3 = (%le, %le, %le)", u2.x() - u3.x(),
			u2.y() - u3.y(), u2.z() - u3.z());
		double d23 = (u2 - u3).squared_length();
		DEBUG_PRINT("||u2 - u3|| = %le", d23);

		DEBUG_PRINT("u2 - u4 = (%le, %le, %le)", u2.x() - u4.x(),
			u2.y() - u4.y(), u2.z() - u4.z());
		double d24 = (u2 - u4).squared_length();
		DEBUG_PRINT("||u2 - u4|| = %le", d24);

		DEBUG_PRINT("u3 - u4 = (%le, %le, %le)", u3.x() - u4.x(),
			u3.y() - u4.y(), u3.z() - u4.z());
		double d34 = (u3 - u4).squared_length();
		DEBUG_PRINT("||u3 - u4|| = %le", d34);
		
		if (d12 < distDirectionMinimal
			|| d13 < distDirectionMinimal
			|| d14 < distDirectionMinimal
			|| d23 < distDirectionMinimal
			|| d24 < distDirectionMinimal
			|| d34 < distDirectionMinimal)
		{
			ERROR_PRINT("Too close directions detected!");
			continue;
		}

		/*
		 * Usual condition looks as follows:
		 *
		 * | h1   u1x   u1y   u1z | | 1   u1x   u1y   u1z |
		 * | h2   u2x   u2y   u2z | | 1   u2x   u2y   u2z |
		 * | h3   u3x   u3y   u3z | | 1   u3x   u3y   u3z |  >  0
		 * | h4   u4x   u4y   u4z | | 1   u4x   u4y   u4z |
		 *
		 * where h1, h2, h3 and h4 are support values which have
		 * numbers iVertex1, iVertex2, iVertex3 and iVertex4 here.
		 */

		double det1 = CGAL::determinant(u2, u3, u4);
		double det2 = CGAL::determinant(u1, u3, u4);
		double det3 = CGAL::determinant(u1, u2, u4);
		double det4 = CGAL::determinant(u1, u2, u3);

		double DEBUG_VARIABLE edet1 = determinantEigen(u2, u3, u4);
		double DEBUG_VARIABLE edet2 = determinantEigen(u1, u3, u4);
		double DEBUG_VARIABLE edet3 = determinantEigen(u1, u2, u4);
		double DEBUG_VARIABLE edet4 = determinantEigen(u1, u2, u3);

		DEBUG_PRINT("det1 = %le, edet1 = %le, diff = %le", det1, edet1,
			det1 - edet1);
		ASSERT(fabs(det1 - edet1) <= 10. * EPS_MIN_DOUBLE);
		DEBUG_PRINT("det2 = %le, edet2 = %le, diff = %le", det2, edet2,
			det2 - edet2);
		ASSERT(fabs(det2 - edet2) <= 10. * EPS_MIN_DOUBLE);
		DEBUG_PRINT("det3 = %le, edet3 = %le, diff = %le", det3, edet3,
			det3 - edet3);
		ASSERT(fabs(det3 - edet3) <= 10. * EPS_MIN_DOUBLE);
		DEBUG_PRINT("det4 = %le, edet4 = %le, diff = %le", det4, edet4,
			det4 - edet4);
		ASSERT(fabs(det4 - edet4) <= 10. * EPS_MIN_DOUBLE);

		det2 = -det2;
		det4 = -det4;

		double det = det1 + det2 + det3 + det4;
		if (det < 0)
		{
			det1 = -det1;
			det2 = -det2;
			det3 = -det3;
			det4 = -det4;
			det = -det;
		}

		if (ifScaleMatrix)
		{
			double normRow = sqrt(det1 * det1 + det2 * det2 +
					det3 * det3 + det4 * det4);
			double coeff = 1. / normRow;
			det1 *= coeff;
			det2 *= coeff;
			det3 *= coeff;
			det4 *= coeff;
		}
		DEBUG_PRINT("%d: det1 = %le, det2 = %le, det3 = %le, "
			"det4 = %le, det = %le", iCondition, det1, det2, det3,
			det4, det);
		ASSERT(det > 0);

		DEBUG_PRINT("Pushing [%d][%d]-th element = %le", iCondition, iVertex1, det1);
		triple.push_back(Triplet(iCondition, iVertex1, det1));
		DEBUG_PRINT("Pushing [%d][%d]-th element = %le", iCondition, iVertex2, det2);
		triple.push_back(Triplet(iCondition, iVertex2, det2));
		DEBUG_PRINT("Pushing [%d][%d]-th element = %le", iCondition, iVertex3, det3);
		triple.push_back(Triplet(iCondition, iVertex3, det3));
		DEBUG_PRINT("Pushing [%d][%d]-th element = %le", iCondition, iVertex4, det4);
		triple.push_back(Triplet(iCondition, iVertex4, det4));
#if 0
		matrix.insert(iCondition, iVertex1) = det1;
		matrix.insert(iCondition, iVertex2) = det2;
		matrix.insert(iCondition, iVertex3) = det3;
		matrix.insert(iCondition, iVertex4) = det4;
#endif

		++iCondition;
	}
	matrix.setFromTriplets(triple.begin(), triple.end());

#if 0
	char *s = (char*) malloc(sizeof(int) * sizeof(char) + 4);
	for (int iCondition = 0; iCondition < numConditions; ++iCondition)
	{
		DEBUG_PRINT("Printing row #%d", iCondition);
		ofstream fileRow;
		sprintf(s, "%d.txt", iCondition);
		char *name = makeNameWithSuffix("matrix-row-", s);
		fileRow.open(name);
		ASSERT(fileRow);
		free(name);
		SparseMatrix row = matrix.row(iCondition);
		fileRow << row;
	}
#endif
	DEBUG_END;
	return matrix;
}


Vector_3 Recoverer::findMaxVertices(Polyhedron_3 polyhedron,
	int& imax0, int& imax1, int& imax2)
{
	DEBUG_START;
	int iVertex = polyhedron.vertices_begin()->id;
	iXmax = iYmax = iZmax = iVertex;
	
	/*
	 * These 3 variables are added to hadle the case when some of iXmax, iYmax
	 * or iZmax are equal.
	 */
	int iXmaxKeeper = iXmax;
	int iYmaxKeeper = iYmax;
	int iZmaxKeeper = iZmax;

	Point_3 pointFirst = polyhedron.vertices_begin()->point();
	double xmax = pointFirst.x(), ymax = pointFirst.y(), zmax = pointFirst.z();

	for (auto vertex = polyhedron.vertices_begin();
			vertex != polyhedron.vertices_end(); ++vertex)
	{
		iVertex = vertex->id;
		Point_3 point = vertex->point();

		if (point.x() > xmax)
		{
			xmax = point.x();
			iXmaxKeeper = iXmax;
			iXmax = iVertex;
		}

		if (point.y() > ymax)
		{
			ymax = point.y();
			iYmaxKeeper = iYmax;
			iYmax = iVertex;
		}
		
		if (point.z() > zmax)
		{
			zmax = point.z();
			iZmaxKeeper = iZmax;
			iZmax = iVertex;
		}
	}

	DEBUG_PRINT("%d-th vertex has maximal X coordinate = %lf", iXmax, xmax);
	DEBUG_PRINT("%d-th vertex has maximal Y coordinate = %lf", iYmax, ymax);
	DEBUG_PRINT("%d-th vertex has maximal Z coordinate = %lf", iZmax, zmax);

	/* Sort indices of vertices with maximal coordinates. */
	set<int> iMaxes;
	iMaxes.insert(iXmax);
	iMaxes.insert(iYmax);
	iMaxes.insert(iZmax);
	
	/*
	 * TODO: Handle case when iXmax = iYmax = iZmax and
	 * iMaxKeeper = iYmaxKeeper = iZmaxKeeper
	 */
	if (iMaxes.size() < 3)
	{
		if (iXmax == iYmax)
		{
			iYmax = iYmaxKeeper;
			iMaxes.insert(iYmax);
		}
		if (iYmax == iZmax)
		{
			iZmax = iZmaxKeeper;
			iMaxes.insert(iZmax);
		}
		if (iXmax == iZmax)
		{
			iXmax = iXmaxKeeper;
			iMaxes.insert(iXmax);
		}
	}
	ASSERT(iMaxes.size() == 3);

	auto itIMaxes = iMaxes.begin();
	imax0 = *(itIMaxes++);
	imax1 = *(itIMaxes++);
	imax2 = *(itIMaxes++);
	DEBUG_PRINT("And here are they sorted: %d, %d, %d", imax0, imax1, imax2);

	DEBUG_END;
	return Vector_3(1. / xmax, 1. / ymax, 1. / zmax);
}

void Recoverer::regularizeSupportMatrix(
		SupportFunctionEstimationData* data, Polyhedron_3 polyhedron)
{
	DEBUG_START;

	SparseMatrix Q = data->supportMatrix().transpose();
	VectorXd hvalues = data->supportVector();

	int imax0 = 0, imax1 = 0, imax2 = 0;
	Vector_3 vMax = findMaxVertices(polyhedron, imax0, imax1, imax2);
	vectorMaxHValues = Vector_3(hvalues(iXmax), hvalues(iYmax), hvalues(iZmax));
	vectorRegularizing = Vector_3(
		hvalues(iXmax) * vMax.x(),
		hvalues(iYmax) * vMax.y(),
		hvalues(iZmax) * vMax.z());

	/* Regularize h-values vector. */
	Point_3 zero(0., 0., 0.);
	for (int iValue = 0; iValue < Q.cols() - 3; ++iValue)
	{
		/* Get vertex corresponding to the column of the matrix. */
		auto vertex = polyhedron.vertices_begin() + iValue;
		Point_3 point = vertex->point();
		Vector_3 vector(zero, point);
		double shift = -vector * vectorRegularizing;
		if (iValue < imax0)
			hvalues(iValue) += shift;
		else if (iValue < imax1 - 1)
			hvalues(iValue) = hvalues(iValue + 1) + shift;
		else if (iValue < imax2 - 2)
			hvalues(iValue) = hvalues(iValue + 2) + shift;
		else
			hvalues(iValue) = hvalues(iValue + 3) + shift;
	}
	DEBUG_PRINT("h-values have been successfully regularized.");
	
	/*
	 * TODO: Implement cutting of 3 columns from matrix Q here
	 */

	Q.transpose();

	DEBUG_END;
	return;
}

#define MAX_ACCEPTABLE_LINF_ERROR 1e-6

/**
 * Checks whether vectors vx, vy, vz are really the eigenvectors of the matrix Q
 * corresponding to the eigenvalue 0 (i. e. basis kernel vectors)
 *
 * @param polyhedron	The polyhedron (convex hull of support directions)
 * @param Q			The support matrix
 */
static bool checkSupportMatrix(Polyhedron_3 polyhedron, SparseMatrix Q)
{
	DEBUG_START;
	/* Set eigenvectors vx, vy, vz. */
	int numVertices = polyhedron.size_of_vertices();
	DEBUG_PRINT("Allocating 6 arrays of of size %d of type double",
			numVertices);
	VectorXd eigenVectorX(numVertices), eigenVectorY(numVertices),
		eigenVectorZ(numVertices);

	auto itVertex = polyhedron.vertices_begin();
	for (int iVertex = 0; iVertex < numVertices; ++iVertex)
	{
		ASSERT(itVertex->id == iVertex);
		Point_3 point = itVertex->point();
		eigenVectorX(iVertex) = point.x();
		eigenVectorY(iVertex) = point.y();
		eigenVectorZ(iVertex) = point.z();
		++itVertex;
	}

	/*
	 * Check that eigenVectorX, eigenVectorY, eigenVectorZ really lie in the
	 * kernel of Q.
	 */
	VectorXd productX = Q * eigenVectorX;
	double errorX2 = productX.norm();
	DEBUG_PRINT("||eigenVectorX^{T} Q^{T}||_2 = %.16lf", errorX2);

	VectorXd productY = Q * eigenVectorY;
	double errorY2 = productY.norm();
	DEBUG_PRINT("||eigenVectorY^{T} Q^{T}||_2 = %.16lf", errorY2);

	VectorXd productZ = Q * eigenVectorZ;
	double errorZ2 = productZ.norm();
	DEBUG_PRINT("||eigenVectorZ^{T} Q^{T}||_2 = %.16lf", errorZ2);

	DEBUG_END;
	return (errorX2 < MAX_ACCEPTABLE_LINF_ERROR) &&
		(errorY2 < MAX_ACCEPTABLE_LINF_ERROR) &&
		(errorZ2 < MAX_ACCEPTABLE_LINF_ERROR);
}

/** Item of support function measurement. */
struct SupportItem
{
	Vector3d u; /**< the direction. */
	double h;   /**< the support value. */
	SupportItem() : u(Vector3d(0., 0., 0.)), h(0) {}
	SupportItem(Vector3d u_, double h_) : u(u_), h(h_) {}
};

auto supportItem_comparer = [](SupportItem a, SupportItem b)
{
	return a.u < b.u;
};

typedef std::set<SupportItem, decltype(supportItem_comparer)> SupportItemSet;

SupportFunctionEstimationData* Recoverer::buildSFEData(
		ShadeContourDataPtr SCData)
{
	DEBUG_START;
#ifndef NDEBUG
	char *name = makeNameWithSuffix(outputName,
		".contours_initial.dat");
	SCData->fprintDefault(name);
	free(name);
#endif

	/* 1. Balance contours if it is required. */
	if (ifBalancing)
	{
		balanceAllContours(SCData);
	}

	/* 2. Extract support planes from shadow contour data. */
	vector<Plane> supportPlanes = extractSupportPlanes(SCData);
	DEBUG_PRINT("Number of extracted support planes: %ld",
		(long unsigned int) supportPlanes.size());

	/* 3. Get normal vectors of support planes and normalize them. */
	vector<Vector3d> directions;
	int iValue = 0;
	long int numHvaluesInit = supportPlanes.size();
	hvaluesInit = new double[numHvaluesInit];
	SupportItemSet supportItems(supportItem_comparer);
	for (auto &plane : supportPlanes)
	{
		DEBUG_PRINT("Processing support plane "
			"(%lf)x + (%lf)y + (%lf)z + (%lf) = 0",
			plane.norm.x, plane.norm.y, plane.norm.z,
			plane.dist);

		/* Assert that d != 0*/
		ASSERT(fpclassify(plane.dist) != FP_ZERO);
		ASSERT(fabs(plane.dist) > EPS_MIN_DOUBLE);

		/* Assert that ||nu|| == 1 */
		double DEBUG_VARIABLE norm = sqrt(qmod(plane.norm));
		DEBUG_PRINT("norm = %lf, norm - 1. = %le", norm, norm - 1.);
		ASSERT(fabs(norm - 1.) <= 10. * EPS_MIN_DOUBLE);
		double supportValue = -plane.dist;

		DEBUG_PRINT("Adding %d-th direction vector (%lf, %lf, %lf)",
			iValue, plane.norm.x, plane.norm.y, plane.norm.z);
		directions.push_back(plane.norm);
		ASSERT(supportValue > 0.);
		DEBUG_PRINT("Adding %d-th support value %lf",
			iValue, supportValue);
		hvaluesInit[iValue++] = supportValue;

		/*
		 * Save new support direction (will be used in
		 * recoverCorrectedData function).
		 */
		ASSERT(plane.dist < 0.);
		SupportItem item = {plane.norm, supportValue};
		supportItems.insert(item);
	}

	/* 4. Construct convex hull of the set of normal vectors. */
	Polyhedron_3 polyhedron = constructHullAndIDmaps(directions);
	checkPolyhedronIDs(polyhedron);

#ifndef NDEBUG
	PolyhedronPtr polyhedronTmp(new Polyhedron(polyhedron));
	name = makeNameWithSuffix(outputName,
		".polyhedron_of_directions.ply");
	polyhedronTmp->fprint_ply_autoscale(DEFAULT_MAX_COORDINATE,
		name, "polyhedron_of_directions");
	free(name);
#endif

	/* 5. Build matrix by the polyhedron. */
	SparseMatrix Q = buildMatrixByPolyhedron(polyhedron, ifScaleMatrix,
		distDirectionMinimal);
//	SparseMatrix Q = Qt.transpose();

#ifndef NDEBUG
	ofstream fileMatrix;
	name = makeNameWithSuffix(outputName, ".support-matrix.mat");
	DEBUG_PRINT("Printing to %s", name);
	fileMatrix.open(name);
	ASSERT(fileMatrix);
	free(name);
	fileMatrix << Q;
	fileMatrix.close();
#endif
	/* 
	 * 5.1. Build vector of support values associated with new order of
	 * vertices.
	 */
	long int numHvalues = Q.cols();
	VectorXd hvalues(numHvalues);
	
	int i = 0;
	supportDirections.clear();
	ASSERT(supportDirections.empty());
	vector<Plane> supportPlanesProcessed;
	for (auto vertex = polyhedron.vertices_begin();
		vertex != polyhedron.vertices_end(); ++vertex)
	{
		auto point = vertex->point();
		Vector3d v(point.x(), point.y(), point.z());
		SupportItem item = {v, 0};
		auto result = supportItems.find(item);
		ASSERT(result != supportItems.end() && "Cannot found an item");
		ASSERT(fpclassify(v.x - result->u.x) == FP_ZERO);
		ASSERT(fpclassify(v.y - result->u.y) == FP_ZERO);
		ASSERT(fpclassify(v.z - result->u.z) == FP_ZERO);
		hvalues(i) = result->h;
		supportDirections.push_back(result->u);
		supportPlanesProcessed.push_back(Plane(result->u, -result->h));
		DEBUG_PRINT("   constructed hvalue = %lf for direction "
			"(%lf, %lf, %lf)", result->h, result->u.x,
			result->u.y, result->u.z);
		++i;
	}
	ASSERT(supportDirections.size() == polyhedron.size_of_vertices());

	/*
	 * 5.2. Build strting point for support function estimation.
	 * To do this, we construct naive polyhedron and calculate its support
	 * values
	 */

	PolyhedronPtr p = buildPolyhedronFromPlanes(supportPlanesProcessed);
#ifndef NDEBUG
	name = makeNameWithSuffix(outputName,
		".polyhedron_starting_point.ply");
	p->fprint_ply_autoscale(DEFAULT_MAX_COORDINATE,
		name, "polyhedron_starting_point");
	free(name);
#endif
	
	DEBUG_PRINT("Constructed starting polyhedron with %d vertices and %d "
		"facets from %ld planes", p->numVertices, p->numFacets,
		supportPlanes.size());
	VectorXd startingVector(numHvalues);
	iValue = 0;
	for (auto &direction: supportDirections)
	{
		double max = 0.;
		for (int iVertex = 0; iVertex < p->numVertices; ++iVertex)
		{
			double product = direction * p->vertices[iVertex];
			max = product > max ? product : max;
		}
		DEBUG_PRINT("starting vector, value #%d = %le", iValue, max);
		ASSERT(max >= 0.);
		startingVector(iValue) = max;
		++iValue;
	}
	DEBUG_PRINT("============== Preliminary estimation =================");
	printEstimationReport(Q, hvalues, startingVector, estimator);
	DEBUG_PRINT("========== End of preliminary estimation ==============");

/* TODO: Fix this method and use it. */
#if 0

	for (long int i = 0; i < numHvalues; ++i)
	{
		DEBUG_PRINT("Constructing hvalue #%ld.", i);
		double hvalue = 0.;
		auto it = mapIDinverse[i].begin();
		while (it != mapIDinverse[i].end())
		{
			ASSERT(*it >= 0);
			ASSERT(*it < numHvaluesInit);
			DEBUG_PRINT("   id %ld, hvalue = %lf", *it, hvaluesInit[*it]);
			hvalue += hvaluesInit[*it];
			++it;
		}
		hvalue /= mapIDinverse[i].size();
		Vector3d u = supportDirections[i];
		DEBUG_PRINT("   constructed hvalue = %lf for direction "
			"(%lf, %lf, %lf)", hvalue, u.x, u.y, u.z);
		hvalues(i) = hvalue;
	}
#endif /* 0 */
	
	DEBUG_PRINT("Q is %d x %d matrix, numHvaluesInit = %ld", Q.rows(), Q.cols(),
		numHvaluesInit);
	
	SupportFunctionEstimationData *data = new SupportFunctionEstimationData(
			numHvalues, Q.rows(), Q, hvalues, startingVector);
	checkPolyhedronIDs(polyhedron);

	/* 5.1. Check that vx, vy, and vz are really eigenvectors of our matrix. */
	bool ifCheckSuccessful = checkSupportMatrix(polyhedron, Q);
	ASSERT_PRINT(ifCheckSuccessful, "Bad matrix.");

	/*
	 * 6. Regularize the undefinite matrix using known 3 kernel basis vectors.
	 * v1 = (u1x, ..., uMx)
	 * v2 = (u1y, ..., uMy)
	 * v3 = (u1z, ..., uMz)
	 *
	 * TODO: Regularization is not supported in Ipopt solver for now. We need
	 * to fix it.
	 */
	if (ifRegularize)
	{
#ifdef USE_IPOPT
		if (estimator == IPOPT_ESTIMATOR)
		{
			ERROR_PRINT("Regularization of support matrix for "
				"Ipopt estimator is not implemented yet.");
			DEBUG_END;
			exit(EXIT_FAILURE);
		}
#endif
		regularizeSupportMatrix(data, polyhedron);
		checkPolyhedronIDs(polyhedron);
	}

	DEBUG_END;
	return data;
}

void Recoverer::setEstimator(RecovererEstimator e)
{
	DEBUG_START;
	estimator = e;
	DEBUG_END;
}

#define ACCEPTED_TOL 1e-6

static void printEstimationReport(SparseMatrix Q, VectorXd h0, VectorXd h,
	RecovererEstimator estimator)
{
	DEBUG_START;
	MAIN_PRINT("Estimation report:");
	double DEBUG_VARIABLE L1 = 0.;
	double DEBUG_VARIABLE L2 = 0.;
	double DEBUG_VARIABLE Linf = 0.;
	ASSERT(h0.size() == h.size());
	for (int i = 0; i < h.size(); ++i)
	{
		double DEBUG_VARIABLE delta = h0(i) - h(i);
		DEBUG_PRINT("h[%d] :  %lf - %lf = %lf", i, h0(i), h(i), delta);
		L1 += fabs(delta);
		L2 += delta * delta;
		Linf = delta > Linf ? delta : Linf;
	}
	MAIN_PRINT("L1 = %lf", L1);
	MAIN_PRINT("L2 = %lf", L2);
	MAIN_PRINT("Linf = %lf", Linf);

	DEBUG_PRINT("-------------------------------");
	VectorXd Qh0 = Q * h0;
	VectorXd Qh  = Q * h;
	ASSERT(Qh0.size() == Qh.size());
	for (int i = 0; i < Qh.size(); ++i)
	{
		if (fabs (Qh0(i)) > ACCEPTED_TOL
			|| fabs (Qh(i)) > ACCEPTED_TOL)
		{
			if (Qh0(i) < 0. && Qh(i) >= 0)
				DEBUG_PRINT("Q * h[%d] : " COLOUR_RED "%le"
					COLOUR_NORM " -> %le", i,
					Qh0(i), Qh(i));
			else if (Qh0(i) >= 0. && Qh(i) < 0)
				DEBUG_PRINT("Q * h[%d] : %le" COLOUR_RED
					" -> %le" COLOUR_NORM, i,
					Qh0(i), Qh(i));
			else if (Qh0(i) < 0. && Qh(i) < 0)
				DEBUG_PRINT("Q * h[%d] : " COLOUR_RED "%le"
					" -> %le" COLOUR_NORM, i,
					Qh0(i), Qh(i));
			else
				DEBUG_PRINT("Q * h[%d] : %le -> %le", i,
					Qh0(i), Qh(i));
		}
		if (Qh(i) >= -ACCEPTED_TOL || estimator == ZERO_ESTIMATOR)
		{
			continue;
		}
		double sum = 0.;
		for (int k = 0; k < Q.outerSize(); ++k)
		{
			for (Eigen::SparseMatrix<double>::InnerIterator
				it(Q, k); it; ++it)
			{
				if (it.row() == i)
				{
					double local = it.value()
						* h(it.col());
					DEBUG_PRINT("Q[%d][%d] = %le",
						it.row(), it.col(),
						it.value());
					DEBUG_PRINT(" *   h[%d] = %le",
						it.col(), h(it.col()));
					DEBUG_PRINT(" = %le", local);
					DEBUG_PRINT("---------------");
					sum += local;
				}
			}
		}
		ASSERT(fpclassify(Qh(i) - sum) == FP_ZERO);
		ASSERT(Qh(i) >= -ACCEPTED_TOL);
	}
	DEBUG_END;
}

PolyhedronPtr Recoverer::produceFinalPolyhedron(
	SupportFunctionEstimationData *estData,
	VectorXd estimate)
{
	DEBUG_START;
	vector<Vector3d> points;
	int numValues = estData->numValues();
	for (int i = 0; i < numValues; ++i)
	{
		points.push_back(supportDirections[i] / estimate(i));
	}

	/* Construct convex hull in the dual space. */
	PolyhedronPtr polyhedronDual = constructConvexHull(points);

	/* Map dual polyhedron to the primal space. */
	PolyhedronPtr polyhedron = buildDualPolyhedron(polyhedronDual);

	DEBUG_END;
	return polyhedron;
}

/*
 * This function does not work correctly!
 * TODO: Fix it and add possibility to choose between it and
 * recoverCorrectedData to try different methods.
 */
ShadeContourDataPtr Recoverer::produceCorrectedData(
	SupportFunctionEstimationData *estData,
	VectorXd estimate)
{
	DEBUG_START;
	ShadeContourDataPtr data(shadowDataPrep);

	int numSidesTotal = 0;
	ASSERT(data->numContours > 0);
#ifndef NDEBUG
	ASSERT(shadowDataPrep->contours[0].ns > 0);
	for (int iContour = 0; iContour < shadowDataPrep->numContours; ++iContour)
	{
		DEBUG_PRINT("Checking shadowDataPrep, contour #%d", iContour);
		ASSERT(shadowDataPrep->contours[iContour].ns > 0);
		auto DEBUG_VARIABLE normal =
			shadowDataPrep->contours[iContour].plane.norm;
		ASSERT((fpclassify(normal.x) != FP_ZERO
			|| fpclassify(normal.y) != FP_ZERO
			|| fpclassify(normal.z) != FP_ZERO)
			&& "normal is null vector");
	}
#endif /* NDEBUG */

	for (int iContour = 0; iContour < data->numContours; ++iContour)
        {
		numSidesTotal += data->contours[iContour].ns;
	}
	DEBUG_PRINT("Total number of sides: %d", numSidesTotal);
	DEBUG_PRINT("mapIDsize = %ld", mapIDsize);
	ASSERT(numSidesTotal == mapIDsize);

	int iAux = 0;
	for (int iContour = 0; iContour < data->numContours; ++iContour)
	{
		SContour *contour = &data->contours[iContour];
		auto DEBUG_VARIABLE normal = contour->plane.norm;
		ASSERT((fpclassify(normal.x) != FP_ZERO
			|| fpclassify(normal.y) != FP_ZERO
			|| fpclassify(normal.z) != FP_ZERO)
			&& "normal is null vector");
		for (int iSide = 0; iSide < contour->ns; ++iSide)
		{
			SideOfContour *side = &contour->sides[iSide];
			int iSideNext = (iSide + contour->ns + 1) 
				% contour->ns;
			SideOfContour *sideNext = &contour->sides[iSideNext];
			int iValue = mapID[iAux + iSide];
			ASSERT(iAux + iSideNext >= 0);
			ASSERT(iSideNext < contour->ns);
			DEBUG_PRINT("iAux + iSideNext = %d, mapIDsize = %ld",
				iAux + iSideNext, mapIDsize);
			ASSERT(iAux + iSideNext < mapIDsize);
			int iValueNext = mapID[iAux + iSideNext];

			Vector3d vec = side->A2;
			double hValueOld = estData->supportVector()(iValue);
			double hValueNew = estimate(iValue);
			double factor = hValueNew / hValueOld;
			Vector3d vecNew = vec * factor;

			double hValueNextOld = estData->supportVector()(iValueNext);
			double hValueNextNew = estimate(iValueNext);
			double factorNext = hValueNextNew / hValueNextOld;
			Vector3d vecNewNext = vec * factorNext;

			Vector3d tangentNext = sideNext->A1 - sideNext->A2;
			Vector3d vecNewPrj = vecNew * (vecNew * tangentNext);
			Vector3d tangent = side->A2 - side->A1;
			Vector3d vecNewNextPrj = vecNewNext * (vecNewNext
				* tangent);
			
			Vector3d vecCorrected = vec + vecNewPrj + vecNewNextPrj;
			side->A2 = vecCorrected;
			sideNext->A1 = vecCorrected;
		}
		iAux += contour->ns;
	}
	DEBUG_END;
	return data;
}

PolyhedronPtr Recoverer::run(ShadeContourDataPtr SCData)
{
	DEBUG_START;
	initData(SCData);

	SupportFunctionEstimator *sfe = NULL;
	CGALSupportFunctionEstimator *sfeCGAL = NULL;
#ifdef USE_IPOPT
	IpoptSupportFunctionEstimator *sfeIpopt = NULL;
#endif /* USE_IPOPT */

	/* Build problem data. */
	SupportFunctionEstimationData *data = buildSFEData(SCData);

	switch (estimator)
	{
	case ZERO_ESTIMATOR:
		DEBUG_PRINT("Zero estimator is on!");
		break;
#ifdef USE_TSNNLS
	case TSNNLS_ESTIMATOR:
		sfe = static_cast<SupportFunctionEstimator*>(new
				TsnnlsSupportFunctionEstimator(data));
		break;
#endif /* USE_TSNNLS */
#ifdef USE_IPOPT
	case IPOPT_ESTIMATOR:
		sfeIpopt = new IpoptSupportFunctionEstimator(data);
		sfeIpopt->setMode(IPOPT_ESTIMATION_QUADRATIC);
		sfe = static_cast<SupportFunctionEstimator*>(sfeIpopt);
		break;
	case IPOPT_ESTIMATOR_LINEAR:
		sfeIpopt = new IpoptSupportFunctionEstimator(data);
		sfeIpopt->setMode(IPOPT_ESTIMATION_LINEAR);
		sfe = static_cast<SupportFunctionEstimator*>(sfeIpopt);
		break;
#endif /* USE_IPOPT */
	case CGAL_ESTIMATOR:
		sfeCGAL = new CGALSupportFunctionEstimator(data);
		sfeCGAL->setMode(CGAL_ESTIMATION_QUADRATIC);
		sfe = static_cast<SupportFunctionEstimator*>(sfeCGAL);
		break;
	case CGAL_ESTIMATOR_LINEAR:
		sfeCGAL = new CGALSupportFunctionEstimator(data);
		sfeCGAL->setMode(CGAL_ESTIMATION_LINEAR);
		sfe = static_cast<SupportFunctionEstimator*>(sfeCGAL);
		break;
	default:
		ERROR_PRINT("Error: type of estimator is not set.");
		DEBUG_END;
		return NULL;
		break;
	}
	if (estimator != ZERO_ESTIMATOR)
	{
		ASSERT(sfe);
	}

	/* Run support function estimation. */
	VectorXd estimate = estimator == ZERO_ESTIMATOR ? data->supportVector()
		: sfe->run();

	printEstimationReport(data->supportMatrix(), data->supportVector(),
		estimate, estimator);

	DEBUG_END;
	return produceFinalPolyhedron(data, estimate);
}
