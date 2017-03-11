/*
 * Copyright (c) 2009-2017 Ilya Palachev <iliyapalachev@gmail.com>
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

/**
 * @file NativeQuadraticEstimator.cpp
 * @brief Native quadratic estimation engine (implementation).
 */
#include <Eigen/LU>
#include "NativeQuadraticEstimator.h"
#include "NativeEstimatorCommonFunctions.h"
#include <CGAL/intersections.h>

NativeQuadraticEstimator::NativeQuadraticEstimator(
		SupportFunctionEstimationDataPtr data) :
	SupportFunctionEstimator(data)
{
	DEBUG_START;
	DEBUG_END;
}

NativeQuadraticEstimator::~NativeQuadraticEstimator()
{
	DEBUG_START;
	DEBUG_END;
}
/** Intersects three planes in order to obtain a point. */
Point_3 intersect(Plane_3 alpha, Plane_3 beta, Plane_3 gamma)
{
	DEBUG_START;
	/*
	 * Just for debug (TODO: remove this in release build)
	 * CGAL intersection sometimes (1%) produces very incorrect
	 * results, that's why it was decided to calculate intersection with Eigen
	 * instead.
	 */
	auto result = CGAL::intersection(alpha, beta, gamma);
	ASSERT(result && "Intersection failed");
	const CGAL::Point_3<CGAL::Epick> *p =
		boost::get<CGAL::Point_3<CGAL::Epick>>(&*result);
	ASSERT(!boost::get<CGAL::Line_3<CGAL::Epick>>(&*result)
			&& "Intersection is a line");
	ASSERT(!boost::get<CGAL::Plane_3<CGAL::Epick>>(&*result)
			&& "Intersection is a plane");
	ASSERT(p && "Intersection is not a point");
	/* return Point_3(p->x(), p->y(), p->z()); */

	/* The actual intersection starts from here */
	Eigen::Matrix3d matrix;
	matrix << alpha.a(), alpha.b(), alpha.c(),
	       beta.a(), beta.b(), beta.c(),
	       gamma.a(), gamma.b(), gamma.c();
	Eigen::Vector3d rightSide(-alpha.d(), -beta.d(), -gamma.d());
	Eigen::Vector3d solution = matrix.inverse() * rightSide;
	DEBUG_END;
	return Point_3(solution(0), solution(1), solution(2));
}

bool validateTangientPoint(Point_3 tangient, Plane_3 alpha, Plane_3 beta,
		Plane_3 gamma, Plane_3 outer)
{
	DEBUG_START;
	Point_3 p = intersect(alpha, beta, gamma);
	Vector_3 normal = outer.orthogonal_vector();
	double value = normal * (tangient - CGAL::Origin());
	double valueSame =  normal * (p - CGAL::Origin());
	double difference = fabs(value - valueSame);
	if (difference >= 1e-9)
	{
		std::cerr << p << std::endl;
		std::cerr << "difference: " << difference << std::endl;
		std::cerr << "value: " << value << std::endl;
		std::cerr << "value: " << valueSame << std::endl;
		std::cerr << "   alpha * p = " <<
			alpha.orthogonal_vector() *
			(p - CGAL::Origin()) + alpha.d()
			<< std::endl;
		std::cerr << "   beta * p = " <<
			beta.orthogonal_vector() *
			(p - CGAL::Origin()) + beta.d()
			<< std::endl;
		std::cerr << "   gamma * p = " <<
			gamma.orthogonal_vector() *
			(p - CGAL::Origin()) + gamma.d()
			<< std::endl;
		DEBUG_END;
		return false;
	}
	DEBUG_END;
	return true;
}

struct WorkingState
{
	/* Support directions (immutable) */
	std::vector<Vector_3> directions;

	/* Initial positions of planes */
	std::vector<Plane_3> planesInitial;

	/* Initial support values */
	VectorXd valuesInitial;

	/* Current positions of planes */
	std::vector<Plane_3> planes;

	/* Current support values */
	VectorXd values;

	/* Current plane, to which we are moving */
	int iPlane;

	/* IDs of moved planes */
	std::vector<int> IDs;

	/* IDs of outer planes */
	std::vector<int> outerIDs;

	/* Current state of polyhedron */
	Polyhedron_3 polyhedron;

	/* Map from facet IDs to plane IDs */
	std::vector<int> index;
}

static void prepareProcess(WorkingState &WS)
{
	DEBUG_START;
	DEBUG_END;
}

static void initState(WorkingState &WS, SupportFunctionEstimationDataPtr data)
{
	DEBUG_START;
	SupportFunctionDataPtr supportData = data->supportData();
	WS.planes = supportData->supportPlanes();
	WS.planesInitial = WS.planes;
	WS.directions = supportData->supportDirections<Vector3d>();
	WS.values = supportData->supportValues();
	WS.valuesInitial = WS.values;

	WS.polyhedon = Polyhedron_3(WS.planes);
	WS.polyhedron.initialize_indices(WS.planes);
	WS.index = WS.polyhedron.indexPlanes_;
	std::cerr << "Intersection contains " << WS.polyhedron.size_of_facets()
		<< " of " << WS.planes.size() << " planes." << std::endl;
	DEBUG_END;
}

static VectorXd runL2Estimation(SupportFunctionEstimationDataPtr data)
{
	DEBUG_START;

	WorkingState WS;

	WS.outerIDs = collectInnerPointsIDs(
			supportData->getShiftedDualPoints_3(0.));
	unsigned numDifferent = 0;
	/* Find the nearest outer plane to the intersection: */
	/* TODO: Replace this with more safe criterion (e.g. with curvature) */
	unsigned iNearest = 0;
	double minDistance = 1e16;
	for (int &iOuter: outerIndex)
	{
		/* TODO: Re-write all common functions used here */
		auto pair = intersection.findTangientVertex(directions[iOuter]);
		auto tangient = pair.first;
		double value = pair.second;

		/* TODO: It mustn't take pointer to polyhedron! */
		auto planesIDs = findTangientPointPlanesIDs(&intersection,
				tangient, index);
		ASSERT(planesIDs.size() == 3 && "Too high degree of vertex");

		std::vector<int> IDs(planesIDs.begin(), planesIDs.end());
		numDifferent += !validateTangientPoint(tangient->point(),
				planes[IDs[0]], planes[IDs[1]], planes[IDs[2]],
				planes[iOuter]);
		double distance = values(iOuter) - value;
		ASSERT(distance > 0 && "Must be outer point");
		if (distance < minDistance)
		{
			minDistance = distance;
			iNearest = iOuter;
		}
	}
	std::cerr << "Number of different values: " << numDifferent << std::endl;
	ASSERT(!numDifferent && "Different values were found");
	std::cout << "Minimal distance: " << minDistance << std::endl;
	std::cout << "Nearest plane: " << iNearest << std::endl;

	auto solution = calculateSolution(supportData, values);
	DEBUG_END;
	return solution;
}

VectorXd NativeQuadraticEstimator::run()
{
	DEBUG_START;
	VectorXd solution = runL2Estimation(data);
	DEBUG_END;
	return solution;
}
