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
 * @file EdgeCorector.h
 * @brief The corrector of polyhedron, that is based on the edge contour data.
 */

#ifndef EDGE_CORRECTOR_H
#define EDGE_CORRECTOR_H
#include <coin/IpTNLP.hpp>
#include <coin/IpIpoptApplication.hpp>
#include "Polyhedron_3/Polyhedron_3.h"

class EdgeCorrector : public TNLP
{
	const Polyhedron_3 &p;
	const std::vector<EdgeInfo> &data;
public:
	EdgeCorrector(const Polyhedron_3 &p, const EdgeIndo &data):
		p(p), data(data) {}

	bool get_nlp_info(Index& n, Index& m, Index& nnz_jac_g,
			Index& nnz_h_lag, IndexStyleEnum& index_style);

	bool get_bounds_info(Index n, Number *x_l, Number *x_u, Index m,
			Number *g_l, Number *g_u);

	bool get_starting_point(Index n, bool init_x,
		Number *x, bool init_z, Number *z_L, Number *z_U, Index m,
		bool init_lambda, Number *lambda);

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

#endif /* EDGE_CORRECTOR_H */
