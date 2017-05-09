/*
 * Copyright (c) 2009-2017 Ilya Palachev <iliyapalachev@gmail.com>
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
 * @file ContourModeRecoverer.h
 * @brief Recovering in so-called "contour mode"
 */

#ifndef CONTOUR_MODE_RECOVERER_H
#define CONTOUR_MODE_RECOVERER_H

#include "DataContainers/ShadowContourData/ShadowContourData.h"

class ContourModeRecoverer
{
	ShadowContourDataPtr data;
public:
	ContourModeRecoverer(ShadowContourDataPtr data) : data(data) {}
	void run();
};

#endif /* CONTOUR_MODE_RECOVERER_H */
