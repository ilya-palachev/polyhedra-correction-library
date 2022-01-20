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
#include "Polyhedron_3/Polyhedron_3.h"
#include "DataContainers/EdgeInfo/EdgeInfo.h"

using Ipopt::Index;
using Ipopt::IpoptCalculatedQuantities;
using Ipopt::IpoptData;
using Ipopt::Number;
using Ipopt::SolverReturn;
using Ipopt::TNLP;

class EdgeCorrector : public TNLP
{
	bool doEdgeLengthScaling;
	const std::vector<Plane_3> &planes;
	const std::vector<EdgeInfo> &edges;
	std::vector<Segment_3> resultingSegments;
	std::vector<Plane_3> resultingPlanes;

public:
	EdgeCorrector(bool doEdgeLengthScaling, const std::vector<Plane_3> &planes, const std::vector<EdgeInfo> &edges) :
		doEdgeLengthScaling(doEdgeLengthScaling), planes(planes), edges(edges)
	{
	}

	std::vector<Segment_3> getResultingSegments()
	{
		return resultingSegments;
	}

	std::vector<Plane_3> getResultingPlanes()
	{
		return resultingPlanes;
	}

	bool get_nlp_info(Index &n, Index &m, Index &nnz_jac_g, Index &nnz_h_lag, IndexStyleEnum &index_style);

	bool get_bounds_info(Index n, Number *x_l, Number *x_u, Index m, Number *g_l, Number *g_u);

	bool get_starting_point(Index n, bool init_x, Number *x, bool init_z, Number *z_L, Number *z_U, Index m,
							bool init_lambda, Number *lambda);

	bool eval_f(Index n, const Number *x, bool new_x, Number &obj_value);

	bool eval_grad_f(Index n, const Number *x, bool new_x, Number *grad_f);

	bool eval_g(Index n, const Number *x, bool new_x, Index m, Number *g);

	bool eval_jac_g(Index n, const Number *x, bool new_x, Index m, Index nnz_jac_g, Index *iRow, Index *jCol,
					Number *jacValues);

	bool eval_h(Index n, const Number *x, bool new_x, Number obj_factor, Index m, const Number *lambda, bool new_lambda,
				Index nnz_h_lag, Index *iRow, Index *jCol, Number *hValues);

	void finalize_solution(SolverReturn status, Index n, const Number *x, const Number *z_L, const Number *z_U, Index m,
						   const Number *g, const Number *lambda, Number obj_value, const IpoptData *ip_data,
						   IpoptCalculatedQuantities *ip_cq);
};

#endif /* EDGE_CORRECTOR_H */
