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
 * @file SupportFunctionDataItemExtractorByPoints.h
 * @brief Extractor of support function data item from given contour side
 * by using only the points of the given side.
 * declaration.
 */

#ifndef SUPPORT_FUNCTION_DATA_ITEM_EXTRACTOR_BY_POINTS_H
#define SUPPORT_FUNCTION_DATA_ITEM_EXTRACTOR_BY_POINTS_H

#include "DataConstructors/SupportFunctionDataConstructor/SupportFunctionDataItemExtractor.h"
#include "DebugPrint.h"
#include "Vector3d.h"

const double EXTRACTOR_BY_POINTS_GUARANTEED_PRECISION = 1e-14;

/** Extractor of support function data item from given contour side. */
class SupportFunctionDataItemExtractorByPoints
	: public SupportFunctionDataItemExtractor
{
public:
	/** Empty constructor. */
	SupportFunctionDataItemExtractorByPoints();

	/** Destructor. */
	virtual ~SupportFunctionDataItemExtractorByPoints();

	/**
	 * Extracts support function data item from a given shadow contour
	 * side.
	 *
	 * @param side	Shadow contour side.
	 * @return Support function data item extracted from given side.
	 */
	virtual SupportFunctionDataItem run(SideOfContour *side);
};

#endif /* SUPPORT_FUNCTION_DATA_ITEM_EXTRACTOR_BY_POINTS_H */
