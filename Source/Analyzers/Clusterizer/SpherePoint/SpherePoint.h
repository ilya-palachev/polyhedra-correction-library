/*
 * SpherePoint.h
 *
 * Created on:  01.05.2012
 *     Author:  Kaligin Nikolai <nkaligin@yandex.ru>
 *
 */

#ifndef SPHEREPOINT_H
#define SPHEREPOINT_H

class SpherePoint
{
public:
	Vector3d vector;
	double phi;
	double psi;

	SpherePoint();
	SpherePoint(const Vector3d& vector_orig);
	SpherePoint(const Vector3d& vector_orig, const double phi_orig,
			const double psi_orig);
	SpherePoint(const double phi_orig, const double psi_orig);
	SpherePoint(const SpherePoint& orig);
	~SpherePoint();
	SpherePoint& operator =(const SpherePoint& orig);
};

double distSpherePoint(SpherePoint& point0, SpherePoint& point1);

SpherePoint& MassCentre(int n, int* indexFacet, shared_ptr<Polyhedron> poly);
SpherePoint& MassCentre(int n, SpherePoint* points);

#endif /* SPHEREPOINT_H */
