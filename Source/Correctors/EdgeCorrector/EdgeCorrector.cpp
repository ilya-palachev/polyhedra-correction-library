/*
 * Copyright (c) 2017 Ilya Palachev <iliyapalachev@gmail.com>
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
 * @file EdgeCorrector.cpp
 * @brief The corrector of polyhedron, that is based on the edge contour data.
 */

#include "DebugAssert.h"
#include "Correctors/EdgeCorrector/EdgeCorrector.h"

#define EDGE_LENGTH_SCALING 0

bool EdgeCorrector::get_nlp_info(Index& n, Index& m, Index& nnz_jac_g,
		Index& nnz_h_lag, IndexStyleEnum& index_style)
{
	DEBUG_START;
	Index N = edges.size(); /* Number of edges */
	std::cout << "    Number of edges:     " << N << std::endl;

	Index K = planes.size(); /* Number of facets */
	std::cout << "    Number of planes:    " << K << std::endl;

	n = 6 * N + 4 * K; /* 6 = 3 (coordinates) * 2 (ends of edge) */
	                   /* 4 = 4 (coefficients of plane equation) */
	std::cout << "    Number of variables: " << n  << ", including:"
		<< std::endl;
	std::cout << "        Coordinates of edges ends: " << 3 * N
		<< std::endl;
	std::cout << "        Coordinates of planes coefficients: " << 4 * K
		<< std::endl;

	m = 6 * N + K; /* = 4 * N (planarity) + K (normality) + */
	               /*   + 2 * N (fixed planes)*/
	std::cout << "    Number of constraints: " << m << ", including:"
		<< std::endl;
	std::cout << "        Planarity constraints: " << 4 * N << std::endl;
	std::cout << "        Normality constraints: " << K << std::endl;
	std::cout << "        Fixed planes contraints: " << 2 * N << std::endl;

	/*
	 * Each planarity condition gives 7 non-zeros,
	 * each normality condition gives 3 non-zeros,
	 * each fixed planes condition gives 3 non-zeros.
	 */
	nnz_jac_g = 34 * N + 3 * K; /* = 7 * (4 * N) + 3 * K + 3 * (2 * N) */

	/*
	 * Hessian of functional gives 9 non-zeros for each end of edge, i.e.
	 * 9 * 2 * N = 18 * N non-zeros,
	 * each planarity condition gives 3+3=6 (for simmetry) non-zeros,
	 * each normality condition gives 3 non-zeros,
	 * all fixed planes conditions are linear.
	 */
	nnz_h_lag = 42 * N + 3 * K; /* 18 * N + 6 * (4 * N) + 3 * K */

	DEBUG_END;
	return true;
}

bool EdgeCorrector::get_bounds_info(Index n, Number *x_l, Number *x_u, Index m,
		Number *g_l, Number *g_u)
{
	DEBUG_START;
	const Number TNLP_INFINITY = 2e19;
	for (Index i = 0; i < n; ++i)
	{
		x_l[i] = -TNLP_INFINITY;
		x_u[i] = +TNLP_INFINITY;
	}

	Index N = edges.size(); /* Number of edges */
	Index K = planes.size(); /* Number of facets */
	for (Index j = 0; j < m; ++j)
	{
		if (j < 4 * N) /* planarity */
			g_l[j] = g_u[j] = 0.;
		else if (j < 4 * N + K) /* normality */
			g_l[j] = g_u[j] = 1.;
		else /* fixed planes */
		{
			int iCondition = j - (4 * N + K);
			int iEdge = iCondition / 2;
			int iEnd = iCondition % 2;
			Segment_3 edge = edges[iEdge].initialEdge;
			Vector_3 v = edge.direction().vector();
			Point_3 end = edge.point(iEnd);
			g_l[j] = g_u[j] = v * (end - CGAL::Origin());
		}
	}
	DEBUG_END;
	return true;
}

double getCoordinate(const Plane_3 &plane, int i)
{
	switch(i)
	{
	case 0:
		return plane.a();
	case 1:
		return plane.b();
	case 2:
		return plane.c();
	case 3:
		return plane.d();
	default:
		ASSERT(0 && "Impossible");
		return 0.;
	}
}

bool EdgeCorrector::get_starting_point(Index n, bool init_x,
		Number *x, bool init_z, Number *z_L, Number *z_U, Index m,
		bool init_lambda, Number *lambda)
{
	DEBUG_START;
	ASSERT(x && init_x && !init_z && !init_lambda);
	Index N = edges.size(); /* Number of edges */
	Index K = planes.size(); /* Number of facets */
	for (Index i = 0; i < n; ++i)
	{
		if (i < 6 * N)
		{
			int iEdge = i / 6;
			Segment_3 edge = edges[iEdge].initialEdge;
			int iEnd = (i % 6) / 3;
			Point_3 end = edge.point(iEnd);
			int iCoord = (i % 6) % 3;
			x[i] = end[iCoord];
		}
		else
		{
			int iCondition = i - 6 * N;
			int iPlane = iCondition / 4;
			ASSERT(iPlane < K);
			Plane_3 plane = planes[iPlane];

			int iCoord = iCondition % 4;
			x[i] = getCoordinate(plane, iCoord);
		}
	}
	DEBUG_END;
	return true;
}

static std::vector<Segment_3> getSegments(int N, const Number *x)
{
	std::vector<Segment_3> segments;

	if (!x)
	{
		for (int i = 0; i < N; ++i)
			segments.push_back(Segment_3(Point_3(0., 0., 0.),
						Point_3(0., 0., 0.)));
		return segments;
	}

	for (int i = 0; i < N; ++i)
	{
		Point_3 source(x[6 * i + 0], x[6 * i + 1], x[6 * i + 2]);
		Point_3 target(x[6 * i + 3], x[6 * i + 4], x[6 * i + 5]);
		Segment_3 segment(source, target);
		segments.push_back(segment);
	}

	return segments;
}

static std::vector<Plane_3> getPlanes(int N, int K, const Number *x)
{
	std::vector<Plane_3> planes;

	if (!x)
	{
		for (int i = 0; i < K; ++i)
			planes.push_back(Plane_3(0., 0., 0., 0.));
		return planes;
	}

	for (int i = 0; i < K; ++i)
	{
		int ii = 6 * N + 4 * i;
		Plane_3 plane(x[ii], x[ii + 1], x[ii + 2], x[ii + 3]);
		planes.push_back(plane);
	}

	return planes;
}

double signedDistance(const Plane_3 &plane, const Point_3 &point)
{
	return plane.a() * point.x() + plane.b() * point.y()
		+ plane.c() * point.z() + plane.d();
}

bool EdgeCorrector::eval_f(Index n, const Number *x, bool new_x,
		Number &obj_value)
{
	DEBUG_START;
	Index N = edges.size(); /* Number of edges */
	auto segments = getSegments(N, x);
	obj_value = 0.;
	for (int i = 0; i < N; ++i)
	{
		double value = 0.;
		const EdgeInfo &info = edges[i];
		for (int iEnd = 0; iEnd < 2; ++iEnd)
		{
			Point_3 end = segments[i][iEnd];
			for (const EdgePlane_3 &plane : info.planes)
			{
				double dist = signedDistance(plane, end);
				value += dist * dist;
			}
		}
#if EDGE_LENGTH_SCALING
		double length = sqrt(info.initialEdge.squared_length());
		ASSERT(length > 0.);
		value *= length;
#endif
		obj_value += value;
	}
	ASSERT(obj_value > 0.);

	DEBUG_END;
	return true;
}

bool EdgeCorrector::eval_grad_f(Index n, const Number *x, bool new_x,
		Number *grad_f)
{
	DEBUG_START;
	Index N = edges.size(); /* Number of edges */

	for (int i = 0; i < n; ++i)
		grad_f[i] = 0.;

	auto segments = getSegments(N, x);
	for (int i = 0; i < 6 * N; ++i)
	{
		int iEdge = i / 6;
		const EdgeInfo &info = edges[iEdge];

		int iEnd = (i % 6) / 3;
		int iCoord = (i % 6) % 3;
		Point_3 end = segments[iEdge][iEnd];
		for (const EdgePlane_3 &plane : info.planes)
		{
			double dist = signedDistance(plane, end);
			grad_f[i] += dist * getCoordinate(plane, iCoord);
		}
		grad_f[i] *= 2.;
#if EDGE_LENGTH_SCALING
		grad_f[i] *= sqrt(info.initialEdge.squared_length());
#endif
	}

	DEBUG_END;
	return true;
}

static int getFacetID(const EdgeInfo &info, int iFacet)
{
	int facetID = 0;
	switch (iFacet)
	{
	case 0:
		facetID = info.facetID1;
		break;
	case 1:
		facetID = info.facetID2;
		break;
	default:
		ASSERT(0 && "Impossible");
		break;
	}

	return facetID;
}

bool EdgeCorrector::eval_g(Index n, const Number *x, bool new_x, Index m,
		Number *g)
{
	DEBUG_START;
	Index N = edges.size(); /* Number of edges */
	Index K = planes.size(); /* Number of facets */
	auto segments = getSegments(N, x);
	auto newPlanes = getPlanes(N, K, x);

	for (Index i = 0; i < m; ++i)
	{
		if (i < 4 * N) /* planarity constraints */
		{
			int iEdge = i / 4;
			const EdgeInfo &info = edges[iEdge];
			Segment_3 edge = segments[iEdge];
			int iEnd = (i % 4) / 2;
			Point_3 end = edge[iEnd];
			int iFacet = (i % 4) % 2;
			int facetID = getFacetID(info, iFacet);
			Plane_3 plane = newPlanes[facetID];
			g[i] = signedDistance(plane, end);
		}
		else if (i < 4 * N + K) /* normality constraints */
		{
			int iPlane = i - 4 * N;
			Plane_3 p = newPlanes[iPlane];
			g[i] = p.a() * p.a() + p.b() * p.b() + p.c() * p.c();
		}
		else /* fixed planes constraints */
		{
			int ii = i - 4 * N - K;
			int iEdge = ii / 2;
			const EdgeInfo &info = edges[iEdge];
			Vector_3 v = info.initialEdge.direction().vector();
			Segment_3 edge = segments[iEdge];
			int iEnd = ii % 2;
			Point_3 end = edge[iEnd];
			g[i] = v * (end - CGAL::Origin());
		}
	}

	DEBUG_END;
	return true;
}

struct MyTriplet
{
	Index row;
	Index col;
	Number value;
};

static MyTriplet getPlanarityTriplet(int N, int i,
		const std::vector<EdgeInfo> &edges,
		const std::vector<Segment_3> &segments,
		const std::vector<Plane_3> &planes)
{
	MyTriplet triplet;
	triplet.row = i / 7;

	int iEdge = i / 28;
	const EdgeInfo &info = edges[iEdge];
	Segment_3 edge = segments[iEdge];

	int iEnd = (i % 28) / 14;
	Point_3 end = edge[iEnd];

	int iFacet = ((i % 28) % 14) / 7;
	int facetID = getFacetID(info, iFacet);
	Plane_3 plane = planes[facetID];

	int iElement = ((i % 28) % 14) % 7;
	ASSERT(iElement == i % 7);

	switch (iElement)
	{
	case 0:
		triplet.col = 6 * iEdge + 3 * iEnd + 0;
		triplet.value = plane.a();
		break;
	case 1:
		triplet.col = 6 * iEdge + 3 * iEnd + 1;
		triplet.value = plane.b();
		break;
	case 2:
		triplet.col = 6 * iEdge + 3 * iEnd + 2;
		triplet.value = plane.c();
		break;
	case 3:
		triplet.col = 6 * N + 4 * facetID + 0;
		triplet.value = end.x();
		break;
	case 4:
		triplet.col = 6 * N + 4 * facetID + 1;
		triplet.value = end.y();
		break;
	case 5:
		triplet.col = 6 * N + 4 * facetID + 2;
		triplet.value = end.z();
		break;
	case 6:
		triplet.col = 6 * N + 4 * facetID + 3;
		triplet.value = 1.;
		break;
	default:
		ASSERT(0 && "Impossible");
		break;
	}

	return triplet;
}

static MyTriplet getNormalityTriplet(int N, int i,
		const std::vector<Plane_3> &planes)
{
	MyTriplet triplet;
	int iPlane = i / 3;
	triplet.row = 4 * N + iPlane;
	Plane_3 plane = planes[iPlane];

	int iCoord = i % 3;

	switch(iCoord)
	{
	case 0:
		triplet.col = 6 * N + 4 * iPlane + 0;
		triplet.value = 2. * plane.a();
		break;
	case 1:
		triplet.col = 6 * N + 4 * iPlane + 1;
		triplet.value = 2. * plane.b();
		break;
	case 2:
		triplet.col = 6 * N + 4 * iPlane + 2;
		triplet.value = 2. * plane.c();
		break;
	default:
		ASSERT(0 && "Impossible");
		break;
	}

	return triplet;
}

static MyTriplet getFixedPlanesTriplet(int N, int K, int i,
		const std::vector<EdgeInfo> &edges)
{
	MyTriplet triplet;
	int iEdge = i / 6;
	Vector_3 v = edges[iEdge].initialEdge.direction().vector();

	int iEnd = (i % 6) / 3;
	triplet.row = 4 * N + K + 2 * iEdge + iEnd;

	int iCoord = (i % 6) % 3;
	triplet.col = i;
	triplet.value = v[iCoord];
	
	return triplet;
}

bool EdgeCorrector::eval_jac_g(Index n, const Number *x, bool new_x, Index m,
		Index nnz_jac_g, Index *iRow, Index *jCol,
		Number *jacValues)
{
	DEBUG_START;
	ASSERT((iRow && jCol && !jacValues && !x)
		|| (!iRow && !jCol && jacValues && x));
	Index N = edges.size(); /* Number of edges */
	Index K = planes.size(); /* Number of facets */
	auto segments = getSegments(N, x);
	auto newPlanes = getPlanes(N, K, x);
	for (Index i = 0; i < nnz_jac_g; ++i)
	{
		MyTriplet triplet;

		if (i < 28 * N) /* planarity constraints */
		{
			triplet = getPlanarityTriplet(N, i, edges, segments,
					newPlanes);
			ASSERT(triplet.row < 4 * N);
			ASSERT(triplet.col < n);
		}
		else if (i < 28 * N + 3 * K) /* normality constraints */
		{
			triplet = getNormalityTriplet(N, i - 28 * N, newPlanes);
			ASSERT(triplet.row >= 4 * N && triplet.row < 4 * N + K);
			ASSERT(triplet.col >= 6 * N && triplet.col < n);
		}
		else /* fixed planes constraints */
		{
			triplet = getFixedPlanesTriplet(N, K,
					i - 28 * N - 3 * K, edges);
			ASSERT(triplet.row >= 4 * N + K && triplet.row < m);
			ASSERT(triplet.col < 6 * N);
		}

		if (jacValues)
		{
			//std::cout << "jac_g: value " << triplet.value << std::endl;
			jacValues[i] = triplet.value;
		}
		else
		{
			//std::cout << "jac_g: row " << triplet.row << " col "
			//	<< triplet.col << std::endl;
			ASSERT(triplet.row < m);
			iRow[i] = triplet.row;
			ASSERT(triplet.col < n);
			jCol[i] = triplet.col;
		}
	}

	DEBUG_END;
	return true;
}

static double getSumForH(const EdgeInfo &info, int i, int j)
{
	double sum = 0.;
	for (const EdgePlane_3 &plane : info.planes)
	{
		Vector_3 v(plane.a(), plane.b(), plane.c());
		sum += v[i] * v[j];
	}
#if EDGE_LENGTH_SCALING
	sum *= sqrt(info.initialEdge.squared_length());
#endif

	return sum;
}

typedef std::vector<std::set<int>> IncidenceStructure;

static MyTriplet getUpperHTriplet(Number obj_factor, const Number *lambda,
		const std::vector<EdgeInfo> &edges, int i,
		IncidenceStructure &IS)
{
	Index N = edges.size(); /* Number of edges */
	MyTriplet triplet;

	int iEdge = i / 30;
	const EdgeInfo &info = edges[iEdge];

	int iEnd = (i % 30) / 15;
	int iCoord = (i % 15) / 5;
	triplet.row = 6 * iEdge + 3 * iEnd + iCoord;

	int iDeriv = i % 5;
	if (iDeriv < 3) /* functional */
	{
		triplet.col = 6 * iEdge + 3 * iEnd + iDeriv;
		triplet.value = 2. * obj_factor * getSumForH(info, iCoord, iDeriv);
	}
	else /* upper-right part planarity */
	{
		iDeriv -= 3;
		unsigned facetID = (iDeriv == 0)
			? info.facetID1
			: info.facetID2;
		triplet.col = 6 * N + 4 * facetID + iCoord;
		triplet.value = lambda ? lambda[4 * iEdge + 2 * iEnd + iDeriv]
			: 0.;
		IS[facetID].insert(iEdge);
	}

	return triplet;
}

static MyTriplet getLowerHTriplet(Number obj_factor, const Number *lambda,
		const std::vector<EdgeInfo> &edges, int i,
		const IncidenceStructure &IS, int &iFacet)
{
	Index N = edges.size(); /* Number of edges */

	MyTriplet triplet;

	int numIncident = IS[iFacet].size();
	int nnzInRow = 2 * numIncident + 1;
	int nnzForFacet = 3 * nnzInRow;
	ASSERT(i < nnzForFacet && "Wrong logic in Hessian");

	int iCoord = i / nnzInRow;
	triplet.row = 6 * N + 4 * iFacet + iCoord;

	int iDeriv = i % nnzInRow;
	if (iDeriv == nnzInRow - 1) /* normality */
	{
		triplet.value = lambda ? 2. * lambda[4 * N + iFacet] : 0.;
		triplet.col = triplet.col; /* diagonal element */
	}
	else /* lower-left part planarity */
	{
		/* FIXME: Optimize this! */
		int iEdge = *std::next(IS[iFacet].begin(), iDeriv / 2);
		int iEnd = iDeriv % 2;
		int iEndFacetID = 0;

		if (edges[iEdge].facetID1 == unsigned(iFacet))
		{
			iEndFacetID = 0;
		}
		else
		{
			ASSERT(edges[iEdge].facetID2 == unsigned(iFacet));
			iEndFacetID = 1;
		}
		triplet.value = lambda ? lambda[4 * iEdge + 2 * iEnd + iEndFacetID]
			: 0.;
		triplet.col = 6 * iEdge + 3 * iEnd + iCoord;
	}

	if (i == nnzForFacet - 1) /* last non-zero element for current plane */
		++iFacet;

	return triplet;
}

bool EdgeCorrector::eval_h(Index n, const Number *x, bool new_x,
		Number obj_factor, Index m, const Number *lambda,
		bool new_lambda, Index nnz_h_lag, Index *iRow, Index *jCol,
		Number *hValues)
{
	DEBUG_START;
	ASSERT((iRow && jCol && !hValues && !x && !lambda)
		|| (!iRow && !jCol && hValues && x && lambda));
	Index N = edges.size(); /* Number of edges */
	Index K = planes.size(); /* Number of facets */

	/*
	 * Incident edge IDs for each plane.
	 * FIXME: Make this static, and don't recalculate it each time.
	 */
	IncidenceStructure IS(planes.size());

	int iFacet = 0;
	int iBase = 30 * N;
	for (Index i = 0; i < nnz_h_lag; ++i)
	{
		MyTriplet triplet;

		if (i < 30 * N) /* functional and upper-right part planarity */
		{
			triplet = getUpperHTriplet(obj_factor, lambda, edges, i,
					IS);
			ASSERT(triplet.row >= 0 && triplet.row < 6 * N);
		}
		else /* lower-left part planarity and normality */
		{
			int iFacetOld = iFacet;
			triplet = getLowerHTriplet(obj_factor, lambda, edges,
					i - iBase, IS, iFacet);
			ASSERT(triplet.row >= 6 * N && triplet.row < 6 * N + 4 * K);
			if (iFacetOld + 1 == iFacet)
			{
				ASSERT(iFacet < K || i == nnz_h_lag - 1);
				iBase = i + 1;
			}
			else
				ASSERT(iFacetOld == iFacet && "Impossible");
		}

		if (hValues)
		{
			if (triplet.row != triplet.col)
				triplet.value *= 0.5;
			hValues[i] = triplet.value;
		}
		else
		{
			iRow[i] = triplet.row;
			jCol[i] = triplet.col;
		}
	}

	DEBUG_END;
	return true;
}

void EdgeCorrector::finalize_solution(SolverReturn status, Index n,
		const Number *x, const Number *z_L, const Number *z_U, Index m,
		const Number *g, const Number *lambda, Number obj_value,
		const IpoptData *ip_data,
		IpoptCalculatedQuantities *ip_cq)
{
	DEBUG_START;
	Index N = edges.size(); /* Number of edges */
	Index K = planes.size(); /* Number of facets */
	auto resultingSegments = getSegments(N, x);
	auto resultingPlanes = getPlanes(N, K, x);

	switch (status)
	{
	case Ipopt::SUCCESS:
		MAIN_PRINT("SUCCESS");
		break;
	case Ipopt::MAXITER_EXCEEDED:
		MAIN_PRINT("MAXITER_EXCEEDED");
		break;
	case Ipopt::CPUTIME_EXCEEDED:
		MAIN_PRINT("CPUTIME_EXCEEDED");
		break;
	case Ipopt::STOP_AT_TINY_STEP:
		MAIN_PRINT("STOP_AT_TINY_STEP");
		break;
	case Ipopt::STOP_AT_ACCEPTABLE_POINT:
		MAIN_PRINT("STOP_AT_ACCEPTABLE_POINT");
		break;
	case Ipopt::LOCAL_INFEASIBILITY:
		MAIN_PRINT("LOCAL_INFEASIBILITY");
		break;
	case Ipopt::USER_REQUESTED_STOP:
		MAIN_PRINT("USER_REQUESTED_STOP");
		break;
	case Ipopt::FEASIBLE_POINT_FOUND:
		MAIN_PRINT("FEASIBLE_POINT_FOUND");
		break;
	case Ipopt::DIVERGING_ITERATES:
		MAIN_PRINT("DIVERGING_ITERATES");
		break;
	case Ipopt::RESTORATION_FAILURE:
		MAIN_PRINT("RESTORATION_FAILURE");
		break;
	case Ipopt::ERROR_IN_STEP_COMPUTATION:
		MAIN_PRINT("ERROR_IN_STEP_COMPUTATION");
		break;
	case Ipopt::INVALID_NUMBER_DETECTED:
		MAIN_PRINT("INVALID_NUMBER_DETECTED");
		break;
	case Ipopt::TOO_FEW_DEGREES_OF_FREEDOM:
		MAIN_PRINT("TOO_FEW_DEGREES_OF_FREEDOM");
		break;
	case Ipopt::INVALID_OPTION:
		MAIN_PRINT("INVALID_OPTION");
		break;
	case Ipopt::OUT_OF_MEMORY:
		MAIN_PRINT("OUT_OF_MEMORY");
		break;
	case Ipopt::INTERNAL_ERROR:
		MAIN_PRINT("INTERNAL_ERROR");
		break;
	case Ipopt::UNASSIGNED:
		MAIN_PRINT("UNASSIGNED");
		break;
	}

	DEBUG_END;
}
