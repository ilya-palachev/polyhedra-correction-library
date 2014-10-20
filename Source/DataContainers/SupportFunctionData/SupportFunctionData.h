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
 * @file SupportFunctionData.h
 * @brief General class for support function data container.
 */

#ifndef SUPPORT_FUNCTION_DATA_H
#define SUPPORT_FUNCTION_DATA_H

#include <vector>
#include <memory>

#include "DataContainers/SupportFunctionData/SupportFunctionDataItem.h"

class SupportFunctionData
{
private:
	/** Vector of support data items. */
	std::vector<SupportFunctionDataItem> items;

public:
	/** Empty constructor. */
	SupportFunctionData();

	/** Copy constructor. */
	SupportFunctionData(const SupportFunctionData &data);

	/** Copy constructor from pointer. */
	SupportFunctionData(const SupportFunctionData *data);

	/** Assignment operator. */
	SupportFunctionData &operator= (const SupportFunctionData &data);

	/** Destructor. */
	virtual ~SupportFunctionData();

	/** Subscription operator. */
	SupportFunctionDataItem &operator[] (const int iPosition);
};

typedef std::shared_ptr<SupportFunctionData> SupportFunctionDataPtr;

#endif /* SUPPORT_FUNCTION_DATA_H */
