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
 * @file EdgeCorector.cpp
 * @brief The corrector of polyhedron, that is based on the edge contour data.
 */

#include "DebugAssert.h"
#include "Correctors/EdgeCorrector/EdgeCorrector.h"

bool EdgeCorector::get_nlp_info(Index& n, Index& m, Index& nnz_jac_g,
		Index& nnz_h_lag, IndexStyleEnum& index_style)
{
	Index N = edges.size(); /* Number of edges */
	Index K = planes.size(); /* Number of facets */
	n = 6 * N + 4 * K; /* 6 = 3 (coordinates) * 2 (ends of edge) */
	                   /* 4 = 4 (coefficients of plane equation) */
	m = 6 * N + K; /* = 4 * N (planarity) + K (normality) + */
	               /*   + 2 * N (fixed planes)*/

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

	return true;
}

bool EdgeCorector::get_bounds_info(Index n, Number *x_l, Number *x_u, Index m,
		Number *g_l, Number *g_u);
{
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

bool EdgeCorector::get_starting_point(Index n, bool init_x,
		Number *x, bool init_z, Number *z_L, Number *z_U, Index m,
		bool init_lambda, Number *lambda)
{
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
			Plane_3 plane = planes[iPlane];
			int iCoord = iCondition % 4;
			x[i] = getCordinate(plane, iCoord);
		}
	}
	return true;
}

static std::vector<Segment_3> getSegments(int N, const Number *x)
{
	std::vector<Segment_3> segments;
	for (int i = 0; i < N; ++i)
	{
		Point_3 source(x[6 * i + 0], x[6 * i + 1], x[6 * i + 2]);
		Point_3 target(x[6 * i + 3], x[6 * i + 4], x[6 * i + 5]);
		Segment_3 segment(source, target);
		segments.push_back(segment);
	}
}

static std::vector<Plane_3> getPlanes(int N, int K, const Number *x)
{
	std::vector<Plane_3> planes;
	x += 6 * N;
	for (int i = 0; i < K; ++i)
	{
		Plane_3 plane(x[4 * i + 0], x[4 * i + 1], x[4 * i + 2],
				x[4 * i + 3]);
		planes.push_back(plane);
	}
}

double signedDistance(const Plane_3 &plane, const Point_3 &point)
{
	return plane.a() * point.x() + plane.b() * point.y()
		+ plane.c() * point.z() + plane.d();
}

bool EdgeCorector::eval_f(Index n, const Number *x, bool new_x,
		Number &obj_value)
{
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
		value *= sqrt(info.initialEdge.squared_length());
		obj_value += value;
	}
	return true;
}

bool EdgeCorrector::eval_grad_f(Index n, const Number *x, bool new_x,
		Number *grad_f)
{
	Index N = edges.size(); /* Number of edges */

	for (int i = 0; i < n; ++i)
		grad_f[i] = 0.;

	auto segments = getSegments(N, x);
	for (int i = 0; i < 6 * N; ++i)
	{
		int iEdge = i / 6;
		int iEnd = (i % 6) / 3;
		int iCoord = (i % 6) % 3;
		Point_3 end = segments[iEdge][iEnd];
		for (const EdgePlane_3 &plane : info.planes)
		{
			double dist = signedDistance(plane, end);
			grad_f[i] += dist * getCoordinate(plane, iCoord);
		}
		grad_f[i] *= 2. * sqrt(info.initialEdge.squared_length());
	}

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
	Index N = edges.size(); /* Number of edges */
	Index K = planes.size(); /* Number of facets */
	auto segments = getSegments(N, x);
	auto newPlanes = getPlanes(N, K, x);

	for (Index i = 0; i < n; ++i)
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
			g[i] = p.a() * p.a() + p.b() + p.b() + p.c() + p.c();
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

	return true;
}

struct Triplet
{
	Index row;
	Index col;
	Number value;
}

static Triplet getPlanarityTriplet(int N, int i,
		const std::vector<Segment_3> &segments,
		const std::vector<Plane_3> &planes)
{
	Triplet triplet;
	triplet.row = i / 7;

	int iEdge = i / 28;
	const EdgeInfo &info = edges[iEdge];
	Segment_3 edge = segments[iEdge];

	int iEnd = (i % 28) / 14;
	Point_3 end = edge[iEnd];

	int iFacet = ((i % 28) % 14) / 7;
	int facetID = getFacetID(info, iFacet);
	Plane_3 plane = newPlanes[facetID];

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

static Triplet getNormalityTriplet(int N, int i,
		const std::vector<Plane_3> &planes)
{
	Triplet triplet;
	int iPlane = i / 3;
	triplet.row = 28 * N + iPlane;
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

static Triplet getFixedPlanesTriplet(int N, int K, int i,
		const std::vector<EdgeInfo> &edges,
		const std::vector<Segment_3> &segments)
{
	Triplet triplet;
	int iEdge = i / 6;
	Vector_3 v = edges[iEdge].direction().vector();

	int iEnd = (i % 6) / 3;
	triplet.row = 28 * N + 3 * K + 2 * iEdge + iEnd;

	int iCoord = (i % 6) % 3;
	triplet.col = i;
	triplet.value = v[iCoord];
	
	return triplet;
}

bool EdgeCorrector::eval_jac_g(Index n, const Number *x, bool new_x, Index m,
		Index nnz_jac_g, Index *iRow, Index *jCol,
		Number *jacValues)
{
	ASSERT((iRow && jCol && !jacValues) || (!iRow && !jCol && jacValues));
	Index N = edges.size(); /* Number of edges */
	Index K = planes.size(); /* Number of facets */
	auto segments = getSegments(N, x);
	auto newPlanes = getPlanes(N, K, x);
	for (Index i = 0; i < nnz_jac_g; ++i)
	{
		Triplet triplet;

		if (i < 28 * N) /* planarity constraints */
			triplet = getPlanarityTriplet(N, i, segments,
					newPlanes);
		else if (i < 28 * N + 3 * K) /* normality constraints */
			triplet = getNormalityTriplet(N, i - 28 * N, newPlanes);
		else /* fixed planes constraints */
			triplet = getFixedPlanesTriplet(N, K,
					i - 28 * N - 3 * K,
					edges, segments);

		if (jacValue)
			jacValues[i] = triplet.value;
		else
		{
			iRow[i] = triplet.row;
			jCol[i] = triplet.col;
		}
	}

	return true;
}

bool eval_h(Index n, const Number *x, bool new_x, Number obj_factor,
		Index m, const Number *lambda, bool new_lambda,
		Index nnz_h_lag, Index *iRow, Index *jCol,
		Number *hValues)
{
	ASSERT((iRow && jCol && !hValues) || (!iRow && !jCol && hValues));
	Index N = edges.size(); /* Number of edges */
	Index K = planes.size(); /* Number of facets */

	/* Incident edge IDs for each plane */
	static bool initialized = false;
	static std::vector<std::vector<int>> incidence(planes.size());

	for (Index i = 0; i < nnz_h_lag; ++i)
	{
		Triplet triplet;

		if (i < 30 * N) /* functional and upper-right part planarity */
		{
			int iEdge = i / 30;
			const EdgeInfo& &info = edges[iEdge];

		}
		else /* lower-left part planarity and normality */
		{
		}
	}
	return true;
}
