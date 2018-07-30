/*
 * Copyright (c) 2009-2018 Ilya Palachev <iliyapalachev@gmail.com>
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
 * @file HallTurlachDetector.h
 * @brief Detection of corners based on Hall-Turlach algorithm.
 * Article URL: https://ieeexplore.ieee.org/abstract/document/754588/
 */

#ifndef HALLTURLACHDETECTOR_H
#define HALLTURLACHDETECTOR_H

#include "PolyhedraCorrectionLibrary.h"
#include <cstdlib>

typedef std::vector<std::pair<double, double>> ItemsVector;

std::vector<std::pair<double, double>>
estimateCorners(ItemsVector items, unsigned mParameter, double tParameter,
                int lParameter, double qParameter, double sParameter,
                bool reverse);

#endif /* HALLTURLACHDETECTOR_H */
