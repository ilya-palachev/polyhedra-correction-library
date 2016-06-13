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
 * @file ShadowContourData.h
 * @brief Declaration of class that contains array of shadow contours.
 */

#ifndef SHADECONTOURDATA_H_FORWARD
#define SHADECONTOURDATA_H_FORWARD

#include <memory>
class ShadowContourData;
/**
 * Shared pointer to the shadow contour data.
 */
typedef std::shared_ptr<ShadowContourData> ShadowContourDataPtr;

#endif /* SHADECONTOURDATA_H_FORWARD */

#ifndef SHADECONTOURDATA_H_
#define SHADECONTOURDATA_H_

#include "DataContainers/PData/PData.h"
#include "DataContainers/SupportFunctionData/SupportFunctionData.h"

/* Forward declarations. */
class SContour;
class Polyhedron;

/**
 * Container of shadow contour data used for polyhedral reconstruction.
 * Usually this data is obtained from experimental measurements or from
 * synthetic generation from simple models.
 */
class ShadowContourData :
	public std::enable_shared_from_this<ShadowContourData>,
	public PData
{
public:
	/**
	 * The number if contours.
	 */
	int numContours;

	/**
	 * The array that contains shadow contours.
	 */
	SContour* contours;

	/** Empty constructor. */
	ShadowContourData();

	/**
	 * Constructor that does nothing but setting parent polyhedron to the
	 * given reference.
	 *
	 * @param p	The reference to the parent polyhedron
	 *
	 * TODO: shadow contour data can be not associated with any polyhedron!
	 */
	ShadowContourData(PolyhedronPtr p);

	/**
	 * Copy constructor from shared pointer
	 *
	 * @param data	The data to be copied
	 */
	ShadowContourData(const ShadowContourDataPtr data);

	/**
	 * Copy constructor from reference
	 *
	 * @param data	The data to be copied
	 */
	ShadowContourData(const ShadowContourData& data);

	/**
	 * Constructor of empty data set by the number of contours.
	 *
	 * @param numContoursNeeded	The needed number of contours.
	 */
	ShadowContourData(const int numContoursNeeded);

	/**
	 * Destructor for the freeing of data.
	 */
	~ShadowContourData();

	/**
	 * Scans shadow contour data from the file specified by its name.
	 *
	 * @param fileNameContours	The name of file with shadow contour data to be
	 * read in.
	 */
	bool fscanDefault(const char* fileNameContours);

	/**
	 * Scans shadow contour data from the file descriptor
	 *
	 * @param file	File descriptor of file with shadow contour data to be read
	 * in.
	 */
	bool fscanDefault(FILE* file);

	/**
	 * Dumps internal structures specifying shadow contour data to the given
	 * file descriptor.
	 *
	 * @param file	File descriptor to be written to.
	 */
	void fprint(FILE* file);

	/**
	 * Writes shadow contour data to the file in standard format.
	 *
	 * @param file	File descriptor to be written to.
	 */
	void fprintDefault(FILE* file);

	/**
	 * Writes shadow contour data to the file in standard format.
	 *
	 * @param file  File name to be written to.
	 */
	void fprintDefault(const char *fileName);

	/**
	 * Writes shadow contour data to the file in standard format.
	 *
	 * @param stream	Output stream.
	 * @param data		The data.
	 *
	 * @return		The stream ready for further outputs.
	 */
	friend std::ostream &operator<<(std::ostream &stream,
			ShadowContourData &data);

	/**
	 * Overloaded operator "==" for comparing of shadow contour data.
	 *
	 * @param contourData	Shadow contour data to be compared with.
	 */
	bool operator ==(const ShadowContourData& contourData) const;

	/**
	 * Overloaded operator "!=" for comparing of shadow contour data.
	 *
	 * @param contourData	Shadow contour data to be compared with.
	 */
	bool operator !=(const ShadowContourData& contourData) const;

	/**
	 * Calculates support function data based on contours that are
	 * contained in this data.
	 *
	 * @return Support function data from contours.
	 */
	SupportFunctionDataPtr calculateSupportData();

	/**
	 * Reports whether the data is empty.
	 *
	 * @return true/false if data is empty or not.
	 */
	bool empty();
};

#endif /* SHADECONTOURDATA_H_ */
