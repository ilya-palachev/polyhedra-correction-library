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

#include "Correctors/EdgeCorrector/EdgeCorrector.h"

bool EdgeCorector::get_nlp_info(Index& n, Index& m, Index& nnz_jac_g,
		Index& nnz_h_lag, IndexStyleEnum& index_style)
{
	Index N = data.size(); /* Number of edges */
	Index K = p.size_of_facets(); /* Number of facets */
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
	 * each planarity condition gives 3 non-zeros
	 */
}
