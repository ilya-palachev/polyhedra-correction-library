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

/**
 * @file SupportFunctionDataItemExtractorByPlane.h
 * @brief Extractor of support function data item from given contour side
 * by using the plane of contour
 * declaration.
 */

#ifndef SUPPORT_FUNCTION_DATA_ITEM_EXTRACTOR_BY_PLANE_H
#define SUPPORT_FUNCTION_DATA_ITEM_EXTRACTOR_BY_PLANE_H

#include "DebugPrint.h"
#include "Vector3d.h"
#include "DataConstructors/SupportFunctionDataConstructor/SupportFunctionDataItemExtractor.h"

const double EXTRACTOR_BY_PLANE_GUARANTEED_PRECISION = 1e-14;

/** Extractor of support function data item from given contour side. */
class SupportFunctionDataItemExtractorByPlane :
	public SupportFunctionDataItemExtractor
{
private:
	/** The plane of the shadow contour. */
	Plane plane;

	/** Whether the plane was initialized. */
	bool ifPlaneInitialized;

public:
	/** Empty constructor. */
	SupportFunctionDataItemExtractorByPlane();

	/** Destructor. */
	virtual ~SupportFunctionDataItemExtractorByPlane();

	/**
	 * Extracts support function data item from a given shadow contour
	 * side.
	 *
	 * @param side	Shadow contour side.
	 * @return Support function data item extracted from given side.
	 */
	virtual SupportFunctionDataItem run(SideOfContour *side);

	/**
	 * Sets the plane field of class to given plane.
	 *
	 * @param p	The plane of shadow contour.
	 */
	void setPlane(const Plane p);
};

#endif /* SUPPORT_FUNCTION_DATA_ITEM_EXTRACTOR_BY_PLANE_H */
