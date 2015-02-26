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
 * @file SupportFunctionDataItemExtractor.cpp
 * @brief Extractor of support function data item from given contour side
 * - implementation.
 */

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "DataConstructors/SupportFunctionDataConstructor/SupportFunctionDataItemExtractor.h"

SupportFunctionDataItemExtractor::SupportFunctionDataItemExtractor()
{
	DEBUG_START;
	DEBUG_END;
}

SupportFunctionDataItemExtractor::~SupportFunctionDataItemExtractor()
{
	DEBUG_START;
	DEBUG_END;
}

SupportFunctionDataItem SupportFunctionDataItemExtractor::run(
		SideOfContour *side)
{
	DEBUG_START;
	ASSERT(0 && "Called empty extractor instead of implemented one");
	DEBUG_END;
	return SupportFunctionDataItem();
}
