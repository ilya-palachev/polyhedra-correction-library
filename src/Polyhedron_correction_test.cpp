#include <math.h>
#include "Polyhedron.h"

int Polyhedron::corpol_test(int ncont) 
{
    SContour* contours = new SContour [ncont];
    
    int nume;
    Edge* edges;
    
    make_cube(1.);
    preprocess_polyhedron();
    
    int numeMax = 0;
    for (int i = 0; i < numf; ++i)
    {
        numeMax += facet[i].nv;
    }
    numeMax = numeMax / 2;
    
    edges = new Edge [numeMax];

    preprocess_edges(nume, numeMax, edges);

    corpol_test_create_contours(nume, edges, ncont, contours);
    
#ifndef NDEBUG
    printf("------------\n Begin of print contours in corpol_test\n");
    for (int i = 0; i < nume; ++i)
    {
        contours[i].my_fprint(stdout);
    }
    printf("------------\n End of print contours in corpol_test\n");
#endif
    
    if (edges != NULL)
    {
        delete[] edges;
        edges = NULL;
    }
    return 0;
}

int Polyhedron::corpol_test_create_contours(
        int nume, Edge* edges,
        int ncont, SContour* contours) 
{
    int* buf = new int[nume];
    int* buf2 = new int[nume];
    
    for (int icont = 0; icont < ncont; ++icont)
    {
#ifndef NDEBUG
        printf("corpol_test_create_contours : Creating contour #%d\n", icont);
#endif
        double angle = 2 * M_PI * (icont + 0.001) / ncont;
        Vector3d nu = Vector3d(cos(angle), sin(angle), 0);
        Plane planeOfProjection = Plane(nu, 0);
        
        int ne = 0;
        
        for (int iedge = 0; iedge < nume; ++iedge)
        {
#ifndef NDEBUG
        	printf("%s : processing edge # %d\n", __func__, iedge);
        	edges[iedge].my_fprint(stdout);
#endif
            Plane pi0 = facet[edges[iedge].f0].plane;
            Plane pi1 = facet[edges[iedge].f1].plane;
            double sign0 = pi0.norm * nu;
            double sign1 = pi1.norm * nu;
#ifndef NDEBUG
            printf ("pi0: (%lf) x  +  (%lf) y  +  (%lf) z  +  (%lf)  =  0\n", pi0.norm.x, pi0.norm.y, pi0.norm.z, pi0.dist);
            printf ("pi1: (%lf) x  +  (%lf) y  +  (%lf) z  +  (%lf)  =  0\n", pi1.norm.x, pi1.norm.y, pi1.norm.z, pi1.dist);
            printf ("prj: (%lf) x  +  (%lf) y  +  (%lf) z  +  (%lf)  =  0\n", planeOfProjection.norm.x,
            		planeOfProjection.norm.y, planeOfProjection.norm.z, planeOfProjection.dist);
            printf("sign0 = %lf\n", sign0);
            printf("sign1 = %lf\n", sign1);
#endif
            if (sign0 * sign1 <= 0)
            {
                buf[ne] = iedge;
                ++ne;
            }
        }
        
#ifndef NDEBUG
        printf("\t Printing buf :");
        for (int i = 0; i < ne; ++i)
        {
            int curEdge = buf[i];
            int curV0 = edges[curEdge].v0;
            int curV1 = edges[curEdge].v1;
            printf ("%d(%d, %d) ", curEdge, curV0, curV1);
        }
        printf("\n");
#endif
        ///////////////////////////////////////////////////////////////
        // Some facets can be orthogonal to the plane of projection. //
        // In this case we must eliminate all invisible edges        //
        // from the buffer.                                          //
        ///////////////////////////////////////////////////////////////

        for (int ifacet = 0; ifacet < numf; ++ifacet)
        {
        	Plane plane = facet[ifacet].plane;
        	if (fabs(plane.norm * nu) >= EPS_COLLINEARITY)
        	{
        		// We are processing only facets which are orthogonal
        		// to the plane of projection
        		continue;
        	}

        	int nv = facet[ifacet].nv;
        	int* index = facet[ifacet].index;

        	// 1. Find the closest vertex to the plane projection.
        	int ivertexMax = -1;
        	double scalarMax = 0.;
        	for (int ivertex = 0; ivertex < nv; ++ivertex)
        	{
        		int v0 = index[ivertex];

        		double scalar = nu * vertex[v0];
        		if (ivertexMax == -1 || scalarMax < scalar)
        		{
        			ivertexMax = ivertex;
        			scalarMax = scalar;
        		}
        	}

        	int v0Max = index[scalarMax];
        	int v1Max = index[scalarMax + 1];
        	Vector3d vector0Max = planeOfProjection.project(vertex[v0Max]);
        	Vector3d vector1Max = planeOfProjection.project(vertex[v1Max]);
        	Vector3d mainEdge = vector1Max - vector0Max;

        	// 2. Go around the facet, beginning from the closest vertex.
        	for (int ivertex = 0; ivertex < nv; ++ivertex)
        	{
        		int v0 = index[ivertex];
        		int v1 = index[ivertex + 1];
        		Vector3d curEdge = vertex[v1] - vertex[v0];

        		// 3. In case of non-positive scalar product
        		// -- eliminate the edge from the buffer
        		if (curEdge * mainEdge <= 0)
        		{
        			int v0elim = v0;
        			int v1elim = v1;
        			int iedgeFound = -1;
        			for (int i = 0; i < ne; ++i)
        			{
        				v0 = edges[buf[i]].v0;
        				v1 = edges[buf[i]].v1;
        				if ( (v0 == v0elim && v1 == v1elim) ||
        						(v0 == v1elim && v1 == v0elim) )
        				{
        					iedgeFound = i;
        					break;
        				}
        			}

        			if (iedgeFound == -1)
        			{
        				printf("%s: iedgeFound not found\n", __func__);
        				return -1;
        			}

        			for (int i = iedgeFound; i < ne - 1; ++i)
        			{
        				buf[i] = buf[i - 1];
        			}
        			--ne;
        		}
        	}
        }
        
        int curEdge = buf[0];
        int firstEdge = curEdge;
        int curV0 = edges[curEdge].v0;
        int curV1 = edges[curEdge].v1;
        int curV = curV0;
        buf2[0] = curEdge;
        int iedge = 1;
        
        do
        {
            bool iffound = false;
            for (int i = 0; i < ne; ++i)
            {
                curV0 = edges[buf[i]].v0;
                curV1 = edges[buf[i]].v1;
                if ( ( curV0 != curV && curV1 != curV ) || buf[i] == curEdge)
                {
                    continue;
                }
                else
                {
                    curEdge = buf[i];
                    buf2[iedge++] = curEdge;
                    
                    if (curV0 == curV)
                    {
                        curV = curV1;
                    }
                    else if (curV1 == curV)
                    {
                        curV = curV0;
                    }
                    iffound = true;
                    break;
                }
            }
            
            if (!iffound)
            {
                printf("corpol_test_create_contours: Error. Cannot find next "
                        "edge for edge #%d (%d, %d)\n", curEdge, 
                        edges[curEdge].v0, edges[curEdge].v1);
                break;
            }
            
        } while (curEdge != firstEdge);
        

#ifndef NDEBUG
        printf("\t Printing buf2 :");
        for (int i = 0; i < iedge; ++i)
        {
            int curEdge = buf2[i];
            int curV0 = edges[curEdge].v0;
            int curV1 = edges[curEdge].v1;
            printf ("%d(%d, %d) ", curEdge, curV0, curV1);
        }
        printf("\n");
     
#endif
        
        if (iedge != ne)
        {
            printf("corpol_test_create_contours: Error. Only %d of %d"
                    "edges has been added to the contour...\n", iedge, ne);
            return -1;
        }
        
        contours[icont].sides = new SideOfContour[ne];
        SideOfContour* sides = contours[icont].sides;
        for (int i = 0; i < ne; ++i)
        {
            Vector3d A1 = vertex[edges[buf2[i]].v0];
            Vector3d A2 = vertex[edges[buf2[i]].v1];
            
            A1 = planeOfProjection.project(A1);
            A2 = planeOfProjection.project(A2);
            
            sides[i].A1 = A1;
            sides[i].A2 = A2;
            sides[i].confidence = 1.;
            sides[i].type = EEdgeRegular;
        }
    }
    
#ifndef NDEBUG
    for (int i = 0; i < nume; ++i)
    {
        contours[i].my_fprint(stdout);
    }
#endif
    return 0;
}

void Polyhedron::make_cube(double a)
{
#ifndef NDEBUG
    printf("I'm going to clear polyhedon!!!\n");
#endif //NDEBUG
    if (vertex != NULL)
    {
        delete[] vertex;
        vertex = NULL;
    }
#ifndef NDEBUG
    printf("vertex deleted !!\n");
#endif //NDEBUG
    if (facet != NULL)
    {
        delete[] facet;
        facet = NULL;
    }
#ifndef NDEBUG
    printf("facet deleted !!\n");
#endif //NDEBUG
    if (vertexinfo != NULL)
    {
        delete[] vertexinfo;
        vertexinfo = NULL;
    }
#ifndef NDEBUG
    printf("vertexinfo deleted !!\n");
#endif //NDEBUG
    if (edge_list != NULL)
    {
        delete[] edge_list;
        edge_list = NULL;
    }
#ifndef NDEBUG
    printf("edge_list deleted !!\n");
#endif //NDEBUG
#ifndef NDEBUG
    printf("Polyhedon has been cleared before recreating as a cube...\n");
#endif //NDEBUG
        
    numf = 6;
    numv = 8;
    
    vertex = new Vector3d [numv];
    facet = new Facet [numf];
    
    vertex[0] = Vector3d(-a, -a, -a);
    vertex[1] = Vector3d( a, -a, -a);
    vertex[2] = Vector3d( a,  a, -a);
    vertex[3] = Vector3d(-a,  a, -a);
    vertex[4] = Vector3d(-a, -a,  a);
    vertex[5] = Vector3d( a, -a,  a);
    vertex[6] = Vector3d( a,  a,  a);
    vertex[7] = Vector3d(-a,  a,  a);
    
    for (int i = 0; i < numf; ++i)
    {
        int nv = facet[i].nv = 4;
        facet[i].index = new int [3 * nv + 1];
    }
    
    facet[0].index[0] = 0;
    facet[0].index[1] = 3;
    facet[0].index[2] = 2;
    facet[0].index[3] = 1;
    facet[0].plane = Plane(Vector3d(0., 0., -1.), -a);
    
    facet[1].index[0] = 4;
    facet[1].index[1] = 5;
    facet[1].index[2] = 6;
    facet[1].index[3] = 7;
    facet[1].plane = Plane(Vector3d(0., 0., 1.), -a);
    
    facet[2].index[0] = 1;
    facet[2].index[1] = 2;
    facet[2].index[2] = 6;
    facet[2].index[3] = 5;
    facet[2].plane = Plane(Vector3d(1., 0., 0.), -a);   
    
    facet[3].index[0] = 2;
    facet[3].index[1] = 3;
    facet[3].index[2] = 7;
    facet[3].index[3] = 6;
    facet[3].plane = Plane(Vector3d(0., 1., 0.), -a);
    
    facet[4].index[0] = 0;
    facet[4].index[1] = 4;
    facet[4].index[2] = 7;
    facet[4].index[3] = 3;
    facet[4].plane = Plane(Vector3d(-1., 0., 0.), -a);
    
    facet[5].index[0] = 0;
    facet[5].index[1] = 1;
    facet[5].index[2] = 5;
    facet[5].index[3] = 4;
    facet[5].plane = Plane(Vector3d(0., -1., 0.), -a);

    for (int i = 0; i < numf; ++i)
    {
    	facet[i].poly = this;
    }

#ifndef NDEBUG
    printf("Polyhedron has been recreated as a cube.\n");
#endif //NDEBUG
}
