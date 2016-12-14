/*
 * Copyright (c) 2009-2016 Ilya Palachev <iliyapalachev@gmail.com>
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
 * @file FixedTopology.h
 * @brief NLP for Ipopt engine. It's used for the correction of polyhedrons
 * in the fixed topology.
 */

#ifndef FIXEDTOPOLOGYNLP_H
#define FIXEDTOPOLOGYNLP_H
#include <coin/IpTNLP.hpp>
#include <coin/IpIpoptApplication.hpp>
#include "FixedTopology.h"

using namespace Ipopt;

class FixedTopologyNLP : public TNLP
{
	/** Support directions. */
	const std::vector<Vector_3> &u;

	/** Support values. */
	const std::vector<double> &h;

	/** Vectors "U" from planes of polyhedron facets {(U, x) = H}. */
	const std::vector<Vector_3> &U;

	/** The same as above, but changed during the optimization. */
	std::vector<Vector_3> directions;

	/** Values "H" from planes of polyhedron facets {(U, x) = H}. */
	const std::vector<double> &H;

	/** The same as above, but changed during the optimization. */
	std::vector<double> values;

	/** Vertices of the polyhedron. */
	const std::vector<Vector_3> &pointsInitial;

	/** The same as above, but changed during the optimization. */
	std::vector<Vector_3> points;

	/** Topology of the problem. */
	const FixedTopology *FT;

	/** The number of consistensy constraints. */
	unsigned numConsistencyConstraints;

	/** The number of planarity constraints. */
	unsigned numPlanarityConstraints;

	/** The number of convexity constraints. */
	unsigned numConvexityConstraints;

	/** The number of normality constraints. */
	unsigned numNormalityConstraints;
public:
	/** Simple by-value constructor. */
	FixedTopologyNLP(const std::vector<Vector_3> &u,
			const std::vector<double> &h,
			const std::vector<Vector_3> &U,
			const std::vector<double> &H,
			const std::vector<Vector_3> &pointsInitial,
			const FixedTopology *FT);

	std::vector<Vector_3> getDirections();

	std::vector<double> getValues();

	std::vector<Vector_3> getPoints();

	bool get_nlp_info(Index& n, Index& m, Index& nnz_jac_g,
			Index& nnz_h_lag, IndexStyleEnum& index_style);

	bool get_bounds_info(Index n, Number *x_l, Number *x_u, Index m,
			Number *g_l, Number *g_u);

	bool get_starting_point(Index n, bool init_x,
		Number *x, bool init_z, Number *z_L, Number *z_U, Index m,
		bool init_lambda, Number *lambda);

	void getVariables(const Number *x);

	bool eval_f(Index n, const Number *x, bool new_x, Number& obj_value);


	bool eval_grad_f(Index n, const Number *x, bool new_x, Number *grad_f);

	bool eval_g(Index n, const Number *x, bool new_x, Index m, Number *g);

	bool eval_jac_g(Index n, const Number *x, bool new_x, Index m,
			Index nnz_jac_g, Index *iRow, Index *jCol,
			Number *jacValues);


	bool eval_h(Index n, const Number *x, bool new_x, Number obj_factor,
			Index m, const Number *lambda, bool new_lambda,
			Index nnz_h_lag, Index *iRow, Index *jCol,
			Number *hValues);
	
	void finalize_solution(SolverReturn status, Index n, const Number *x,
			const Number *z_L, const Number *z_U, Index m,
			const Number *g, const Number *lambda, Number obj_value,
			const IpoptData *ip_data,
			IpoptCalculatedQuantities *ip_cq);
};

#endif /* FIXEDTOPOLOGYNLP_H */
