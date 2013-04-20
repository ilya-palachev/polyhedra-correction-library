/*
 * ClusterNorm.h
 *
 * Created on:  01.05.2012
 *     Author:  Kaligin Nikolai <nkaligin@yandex.ru>
 */

#ifndef CLUSTERNORM_H
#define CLUSTERNORM_H

#include "PolyhedraCorrectionLibrary.h"

class ClusterNorm {
public:
	int num;
	int numMax;
	SpherePoint P;
	int* indexFacet;
	Polyhedron* poly;

	ClusterNorm& operator+=(
			ClusterNorm& cluster0);
	ClusterNorm& operator=(
			const ClusterNorm& orig);

	ClusterNorm();
	ClusterNorm(
			int num_orig,
			int numMax_orig,
			SpherePoint P_orig,
			Polyhedron* poly_orig);
	ClusterNorm(
			int num_orig,
			int numMax_orig,
			SpherePoint P_orig,
			int* indexFacet_orig,
			Polyhedron* poly_orig);
	ClusterNorm(
			const ClusterNorm& orig);

	double area();
	~ClusterNorm();

	void fprint(
			FILE* file);
	void setColor(
			char red,
			char green,
			char blue);

private:

};

double distCluster(
		ClusterNorm& cluster0,
		ClusterNorm& cluster1);

#endif /* CLUSTERNORM_H */
