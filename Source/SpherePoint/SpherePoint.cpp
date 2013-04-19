/* 
 * File:   SpherePoint.cpp
 * Author: nk
 * 
 * Created on 1 Май 2012 г., 12:39
 */

//#include "SpherePoint.h"
#include "Polyhedron.h"

SpherePoint::SpherePoint(): 
        vector(Vector3d(0., 0., 0.)),
        phi(0.),
        psi(0.)
{}

SpherePoint::SpherePoint(const Vector3d& vector_orig) {
    double x = vector_orig.x;
    double y = vector_orig.y;
    double z = vector_orig.z;

    
    vector = vector_orig;
    phi = atan2(y, x);
    psi = atan2(z, sqrt(x * x + y * y));
}

SpherePoint::SpherePoint(const Vector3d& vector_orig, const double phi_orig, const double psi_orig):
        vector(vector_orig),
        phi(phi_orig),
        psi(psi_orig)
{}

SpherePoint::SpherePoint(const double phi_orig, const double psi_orig) {
    phi = phi_orig;
    psi = psi_orig;
    double cosinus = cos(psi);
    vector = Vector3d(cos(phi) * cosinus, sin(phi) * cosinus, sin(psi));
}

SpherePoint::SpherePoint(const SpherePoint& orig) : 
        vector(orig.vector),
        phi(orig.phi),
        psi(orig.psi)
{
}

SpherePoint::~SpherePoint() {
}

double dist(SpherePoint& point0, SpherePoint& point1) {
    //return asin(sqrt(qmod(point0.vector % point1.vector)));
    double sinus = sqrt(qmod(point0.vector % point1.vector));
    double cosinus = (point0.vector * point1.vector)/(sqrt(qmod(point0.vector)) * sqrt(qmod(point1.vector)));
    return atan2(sinus, cosinus);
}

SpherePoint& MassCentre(int n, SpherePoint* points, SpherePoint& newSpherePoint) {
    double sum_phi = 0.;
    double sum_psi = 0.;
    
    for (int i = 0; i < n; ++i) {
        sum_phi += points[i].phi;
        sum_psi += points[i].psi;
    }
    sum_phi /= n;
    sum_psi /= n;
    newSpherePoint = SpherePoint(sum_phi, sum_psi);
    return newSpherePoint;
}

SpherePoint& MassCentre(int n, int* indexFacet, Polyhedron* poly) {
    SpherePoint newSpherePoint;
    double sum_phi = 0.;
    double sum_psi = 0.;
    double areaOne;
    double areaTotal;
    
    Vector3d normal;
    
    areaTotal = 0;
    for (int i = 0; i < n; ++i) {
        normal = poly->facets[indexFacet[i]].plane.norm;
                
        newSpherePoint = SpherePoint(normal);
        
            areaOne = poly->area(indexFacet[i]);
        areaTotal += areaOne;
        
        sum_phi += areaOne * newSpherePoint.phi;
        sum_psi += areaOne * newSpherePoint.psi;
    }
    sum_phi /= areaTotal;
    sum_psi /= areaTotal;
    newSpherePoint = SpherePoint(sum_phi, sum_psi);
    return newSpherePoint;
    
}
