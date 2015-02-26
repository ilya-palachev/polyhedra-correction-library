/*
 * Copyright (c) 2009-2015 Ilya Palachev <iliyapalachev@gmail.com>
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

#ifdef USE_TSNNLS

#include "Recoverer/SupportFunctionEstimator.h"

#ifndef isnan
#define isnan
#endif
#ifndef finite
#define finite
#endif
#ifndef isinf
#define isinf
#endif
#include <libtsnnls/tsnnls.h>

#ifndef TSNNLSSUPPORTFUNCTIONESTIMATOR_H_
#define TSNNLSSUPPORTFUNCTIONESTIMATOR_H_

class TsnnlsSupportFunctionEstimator : public SupportFunctionEstimator
{
public:
	TsnnlsSupportFunctionEstimator(SupportFunctionEstimationData *data);
	~TsnnlsSupportFunctionEstimator();

	virtual VectorXd run(void);
};

#endif /* TSNNLSSUPPORTFUNCTIONESTIMATOR_H_ */
#endif /* USE_TSNNLS */
