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

/**
 * @file GlpkSFELinfBuilder.h
 * @brief Builder of GLPK L-inf support function estimation problem
 * function estimation.
 */

#ifdef USE_GLPK

#include <glpk.h>
#include "Recoverer/SupportFunctionEstimator.h"

#ifndef GLPKSFELINFBUILDER_H_
#define GLPKSFELINFBUILDER_H_

/**
 * Builder of GLPK L-inf support function estimation problem
 */
class GlpkSFELinfBuilder
{
public:
	/** Support function estimation data. */
	SupportFunctionEstimationDataPtr data;

	/**
	 * Default constructor
	 *
	 * @param data	Support function estimation data (input)
	 */
	GlpkSFELinfBuilder(SupportFunctionEstimationDataPtr data);

	/** Default destructor. */
	~GlpkSFELinfBuilder();

	/**
	 * Builds the GLPK L-inf support function estimation problem
	 *
	 * @return	The problem
	 */
	glp_prob *build(void);
};

#endif /* GLPKSFELINFBUILDER_H_ */
#endif /* USE_GLPK */

