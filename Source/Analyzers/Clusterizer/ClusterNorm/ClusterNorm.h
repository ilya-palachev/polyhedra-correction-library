/*
 * ClusterNorm.h
 *
 * Created on:  01.05.2012
 *     Author:  Kaligin Nikolai <nkaligin@yandex.ru>
 */

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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CLUSTERNORM_H
#define CLUSTERNORM_H

class ClusterNorm
{
public:
	int num;
	int numMax;
	SpherePoint P;
	int* indexFacet;
	shared_ptr<Polyhedron> poly;

	ClusterNorm& operator+=(ClusterNorm& cluster0);
	ClusterNorm& operator=(const ClusterNorm& orig);

	ClusterNorm();
	ClusterNorm(int num_orig, int numMax_orig, SpherePoint P_orig,
			shared_ptr<Polyhedron> poly_orig);
	ClusterNorm(int num_orig, int numMax_orig, SpherePoint P_orig,
			int* indexFacet_orig, shared_ptr<Polyhedron> poly_orig);
	ClusterNorm(const ClusterNorm& orig);

	double area();
	~ClusterNorm();

	void fprint(FILE* file);
	void setColor(unsigned char red, char green, char blue);

private:

};

double distCluster(ClusterNorm& cluster0, ClusterNorm& cluster1);

#endif /* CLUSTERNORM_H */
