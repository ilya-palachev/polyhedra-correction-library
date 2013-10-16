/* 
 * Copyright (c) 2009-2013 Ilya Palachev <iliyapalachev@gmail.com>
 * Copyright (c) 2009-2012 Nikolai Kaligin <nkaligin@yandex.ru>
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

#ifndef TREECLUSTERNORM_H
#define TREECLUSTERNORM_H

class TreeClusterNorm
{
public:
	TreeClusterNormNode* root;

	TreeClusterNorm();
	TreeClusterNorm(TreeClusterNormNode* root_orig);
	TreeClusterNorm(const TreeClusterNorm& orig);
	virtual ~TreeClusterNorm();

	void fprint(FILE* file);
//    void fprint_dendrogamma(FILE* file);
	void fprint_dendrogamma_lev(FILE* file);

private:

};

#endif /* TREECLUSTERNORM_H */
