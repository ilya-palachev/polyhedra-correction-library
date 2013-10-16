/* 
 * Copyright (c) 2009-2013 Ilya Palachev <iliyapalachev@gmail.com>
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

#ifndef EDGECONTOURASSOCIATION_H_
#define EDGECONTOURASSOCIATION_H_

class EdgeContourAssociation
{
public:
	int indContour;
	int indNearestSide;
	bool ifProperDirection;
	double weight;

	EdgeContourAssociation();
	EdgeContourAssociation(int indContour_orig, int indNearestSide_orig,
			bool ifProperDirection_orig, double weight_orig);
	EdgeContourAssociation(int indContour_orig);
	EdgeContourAssociation(const EdgeContourAssociation& orig);

	~EdgeContourAssociation();
};

#endif /* EDGECONTOURASSOCIATION_H_ */
