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
 * @file ShadowContourClusterizer.h
 * @brief Declaration of main clusterizing engine for the pre-processing of
 * shadow contour data.
 */

#ifndef SHADOWCONTOURCLUSTERIZER_H_
#define SHADOWCONTOURCLUSTERIZER_H_

#include <memory>

#include "Analyzers/PAnalyzer/PAnalyzer.h"
#include "Polyhedron/Polyhedron.h"
#include "DataContainers/ShadeContourData/ShadeContourData.h"

/**
 * Shadow contour clusterizer. Can detect different observations of the same
 * point at multiple shadow contours.
 */
class ShadowContourClusterizer : public PAnalyzer
{
public:

	/* Empty constructor. */
	ShadowContourClusterizer();

	/* Constructor that sets operated polyhedron (not used). */
	ShadowContourClusterizer(shared_ptr<Polyhedron> p);

	/* Empty destructor. */
	~ShadowContourClusterizer();

	/*
	 * Build the plot of all observation at all contours for visualization in
	 * GNU Plot program.
	 * 
	 * @param contourData	Shadow contour data (set of given contours)
	 * @param fileNamePlot	The of file used as output file parsed by GNU Plot.
	 */
	void buildPlot(ShadeContourDataPtr contourData, const char* fileNamePlot);
};

#endif /* SHADOWCONTOURCLUSTERIZER_H_ */
