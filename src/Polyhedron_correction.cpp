/* 
 * File:   Polyhedron_correction.cpp
 * Author: ilya
 * 
 * Created on 19 Ноябрь 2012 г., 07:39
 */

#include "Polyhedron.h"

const double eps_max_error = 1e-6;

int Polyhedron::correct_polyhedron(int N, SContour* contours)
{
	DBG_START;

    int dim = numf * 5;
    
    int nume;
    Edge* edges = NULL;

    corpol_preprocess(nume, edges, N, contours);
    
    double * matrix = new double [dim * dim];
    double * rightPart = new double [dim];
    double * solution = new double [dim];
    
    Plane * prevPlanes = new Plane [numf];
    
    DBGPRINT("%s: memory allocation done\n", __func__);

    for (int i = 0; i < numf; ++i)
    {
        prevPlanes[i] = facet[i].plane;
    }
    DBGPRINT("%s: memory initialization done\n", __func__);

    double error = corpol_calculate_functional(nume, edges, N, contours, 
            prevPlanes);
    
    DBGPRINT("%s: first functional calculation done. error = %lf\n",
    		__func__, error);

    int numIterations = 0;

    while (error > eps_max_error)
    {
    	DBGPRINT("%s: Iteration %d\n", __func__, numIterations);
    	corpol_iteration(nume, edges, N, contours, prevPlanes, matrix,
                rightPart, solution);
        
        for (int i = 0; i < numf; ++i)
        {
            prevPlanes[i] = facet[i].plane;
        }
        error = corpol_calculate_functional(nume, edges, N, contours, 
                prevPlanes);
    	++numIterations;
    }

    DBG_END;
    return 0;
}

double Polyhedron::corpol_calculate_functional(int nume, 
        Edge* edges, int N, SContour* contours, 
        Plane* prevPlanes)
{
//	DBG_START;
    double sum = 0;
    
    for (int i = 0; i < nume; ++i)
    {
//    	DBGPRINT("%s: processing edge #%d\n", __func__, i);
        int nc = edges[i].numc;
        int * cnums = edges[i].contourNums;
        int * cnearestSide = edges[i].contourNearestSide;
        int f0 = edges[i].f0;
        int f1 = edges[i].f1;
        Plane plane0 = facet[f0].plane;
        Plane plane1 = facet[f1].plane;
        Plane planePrev0 = prevPlanes[f0];
        Plane planePrev1 = prevPlanes[f1];
        
        for (int j = 0; j < nc; ++j)
        {
//        	DBGPRINT("\t%s: processing contour #%d\n", __func__, j);
            int curContour = cnums[j];
            int curNearestSide = cnearestSide[j];
            SideOfContour * sides = contours[curContour].sides;
            Plane planeOfProjection = contours[curContour].plane;
            
            double gamma_ij = - planePrev1.norm * planeOfProjection.norm / 
                ((planePrev0.norm - planePrev1.norm) * planeOfProjection.norm);
            
            double a_ij = gamma_ij * plane0.norm.x + 
                (1 - gamma_ij) * plane1.norm.x;
            
            double b_ij = gamma_ij * plane0.norm.y +
                (1 - gamma_ij) * plane1.norm.y;
            
            double c_ij = gamma_ij * plane0.norm.z +
                (1 - gamma_ij) * plane1.norm.z;
            
            double d_ij = gamma_ij * plane0.dist +
                (1 - gamma_ij) * plane1.dist;
            
            Vector3d A_ij0 = sides[curNearestSide].A1;
            Vector3d A_ij1 = sides[curNearestSide].A2;
            
            double summand0 = 
                a_ij * A_ij0.x + 
                b_ij * A_ij0.y + 
                c_ij * A_ij0.z +
                d_ij;
            
            summand0 *= summand0;
            
            double summand1 = 
                a_ij * A_ij1.x +
                b_ij * A_ij1.y +
                c_ij * A_ij1.z +
                d_ij;
            
            summand1 *= summand1;
            
            sum += summand0 + summand1;
        }
    }
//    DBG_END;
    return sum;
}

int Polyhedron::corpol_iteration(int nume, Edge* edges, int N, 
        SContour* contours, Plane* prevPlanes, double* matrix, 
        double* rightPart, double* solution)
{
    corpol_calculate_matrix(nume, edges, N, contours, prevPlanes, matrix,
            rightPart, solution);
    int ret = Gauss_string(5 * numf, matrix, rightPart);
    
    for (int i = 0; i < 5 * numf; ++i)
    {
        solution[i] = rightPart[i];
    }
    
    return ret;
}

void Polyhedron::corpol_calculate_matrix(int nume, Edge* edges, int N, 
        SContour* contours, Plane* prevPlanes, double* matrix, 
        double* rightPart, double* solution)
{
    for (int i = 0; i < (5 * numf) * (5 * numf); ++i)
    {
        matrix[i] = 0.;
    }
    
    for (int i = 0; i < 5 * numf; ++i)
    {
        rightPart[i] = 0.;
    }
    
    for (int iplane = 0; iplane < numf; ++iplane)
    {
        int nv = facet[iplane].nv;
        int * index = facet[iplane].index;
        
        Plane planePrevThis = prevPlanes[iplane];

        int i_ak_ak = 5 * numf * 5 * iplane + 5 * iplane;
        int i_bk_ak = i_ak_ak + 5 * numf;
        int i_ck_ak = i_ak_ak + 2 * 5 * numf;
        int i_dk_ak = i_ak_ak + 3 * 5 * numf;
        int i_lk_ak = i_ak_ak + 4 * 5 * numf;
        
        matrix[i_lk_ak] = prevPlanes[iplane].norm.x;
        matrix[i_lk_ak + 1] = prevPlanes[iplane].norm.y;
        matrix[i_lk_ak + 2] = prevPlanes[iplane].norm.z;
        matrix[i_lk_ak + 3] = matrix[i_lk_ak + 4] = 0;
        rightPart[5 * iplane + 4] = 1.;
        
        for (int iedge = 0; iedge < nv; ++iedge)
        {
            int v0 = index[iedge];
            int v1 = index[iedge + 1];
            int iplaneNeighbour = index[nv + 1 + iedge];

            int i_ak_an = 5 * numf * 5 * iplane + 5 * iplaneNeighbour;
            int i_bk_an = i_ak_an + 5 * numf;
            int i_ck_an = i_ak_an + 2 * 5 * numf;
            int i_dk_an = i_ak_an + 3 * 5 * numf;
            __attribute__((unused)) int i_lk_an = i_ak_an + 4 * 5 * numf;
            
            Plane planePrevNeighbour = prevPlanes[iplaneNeighbour];
            
            int edgeid = preed_find(nume, edges, v0, v1);
            
            if (edgeid == -1)
            {
                printf ("Error! edge (%d, %d) cannot be found\n", v0, v1);
                return;
            }
            
            int nc = edges[edgeid].numc;
            int * cnums = edges[edgeid].contourNums;
            int * cnearest = edges[edgeid].contourNearestSide;
            
            for (int icont = 0; icont < nc; ++icont)
            {
                int curContour = cnums[icont];
                int curNearestSide = cnearest[icont];
                SideOfContour * sides = contours[curContour].sides;
                Plane planeOfProjection = contours[curContour].plane;
                
                double gamma_ij = - planePrevNeighbour.norm * 
                        planeOfProjection.norm /
                        ((planePrevThis.norm - planePrevNeighbour.norm) *
                        planeOfProjection.norm);
                
                Vector3d A_ij1 = sides[curNearestSide].A1;
                Vector3d A_ij2 = sides[curNearestSide].A2;
                
                double x0 = A_ij1.x;
                double y0 = A_ij1.y;
                double z0 = A_ij1.z;
                double x1 = A_ij2.x;
                double y1 = A_ij2.y;
                double z1 = A_ij2.z;
                
                double xx = x0 * x0 + x1 * x1;
                double xy = x0 * y0 + x1 * y1;
                double xz = x0 * z0 + x1 * z1;
                double x = x0 + x1;
                double yy = y0 * y0 + y1 * y1;
                double yz = y0 * z0 + y1 * z1;
                double y = y0 + y1;
                double zz = z0 * z0 + z1 * z1;
                double z = z0 + z1;
                
                double gamma1 = gamma_ij * gamma_ij;
                double gamma2 = gamma_ij * (1 - gamma_ij);
                
                matrix[i_ak_ak] += gamma1 * xx;
                matrix[i_ak_ak + 1] += gamma1 * xy;
                matrix[i_ak_ak + 2] += gamma1 * xz;
                matrix[i_ak_ak + 3] += gamma1 * x;
                
                matrix[i_bk_ak] += gamma1 * xy;
                matrix[i_bk_ak + 1] += gamma1 * yy;
                matrix[i_bk_ak + 2] += gamma1 * yz;
                matrix[i_bk_ak + 3] += gamma1 * y;
                
                matrix[i_ck_ak] += gamma1 * xz;
                matrix[i_ck_ak + 1] += gamma1 * yz;
                matrix[i_ck_ak + 2] += gamma1 * zz;
                matrix[i_ck_ak + 3] += gamma1 * z;
                
                matrix[i_dk_ak] += gamma1 * x;
                matrix[i_dk_ak + 1] += gamma1 * y;
                matrix[i_dk_ak + 2] += gamma1 * z;
                matrix[i_dk_ak + 3] += gamma1;
                
                matrix[i_ak_an] += gamma2 * xx;
                matrix[i_ak_an + 1] += gamma2 * xy;
                matrix[i_ak_an + 2] += gamma2 * xz;
                matrix[i_ak_an + 3] += gamma2 * x;
                
                matrix[i_bk_an] += gamma2 * xy;
                matrix[i_bk_an + 1] += gamma2 * yy;
                matrix[i_bk_an + 2] += gamma2 * xz;
                matrix[i_bk_an + 3] += gamma2 * y;
                
                matrix[i_ck_an] += gamma2 * xz;
                matrix[i_ck_an + 1] += gamma2 * yz;
                matrix[i_ck_an + 2] += gamma2 * zz;
                matrix[i_ck_an + 3] += gamma2 * z;
                
                matrix[i_dk_an] += gamma2 * x;
                matrix[i_dk_an + 1] += gamma2 * y;
                matrix[i_dk_an + 2] += gamma2 * z;
                matrix[i_dk_an + 3] += gamma2;
            }
        }
    }
}

