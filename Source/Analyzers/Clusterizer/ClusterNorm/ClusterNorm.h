/*
 * Copyright (c) 2009-2016 Ilya Palachev <iliyapalachev@gmail.com>
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

#ifndef CLUSTERNORM_H
#define CLUSTERNORM_H

#include <memory>

#include "Polyhedron/Polyhedron.h"
#include "Analyzers/Clusterizer/SpherePoint/SpherePoint.h"

class ClusterNorm
{
public:
	int num;
	int numMax;
	SpherePoint P;
	int *indexFacet;
	PolyhedronPtr poly;

	ClusterNorm &operator+=(ClusterNorm &cluster0);
	ClusterNorm &operator=(const ClusterNorm &orig);

	ClusterNorm();
	ClusterNorm(int num_orig, int numMax_orig, SpherePoint P_orig, PolyhedronPtr poly_orig);
	ClusterNorm(int num_orig, int numMax_orig, SpherePoint P_orig, int *indexFacet_orig, PolyhedronPtr poly_orig);
	ClusterNorm(const ClusterNorm &orig);

	double area();
	~ClusterNorm();

	void fprint(FILE *file);
	void setColor(unsigned char red, char green, char blue);

private:
};

double distCluster(ClusterNorm &cluster0, ClusterNorm &cluster1);

#endif /* CLUSTERNORM_H */
