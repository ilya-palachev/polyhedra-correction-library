#include "Polyhedron.h"

#define NDEBUG

int Polyhedron::corpol_preprocess(
        int& nume, 
        Edge* &edges,
        int N, 
        SContour* contours)
{
	DBG_START;
    // + TODO: provide right orientation of facets
    int numeMax = 0;
    for (int i = 0; i < numf; ++i)
    {
        numeMax += facet[i].nv;
    }
    numeMax = numeMax / 2;
    
    edges = new Edge [numeMax];
    
    preprocess_edges(nume, numeMax, edges);
    corpol_prep_build_lists_of_visible_edges(nume, edges, N, contours);
    corpol_prep_map_between_edges_and_contours(nume, edges, N, contours);

    DBG_END;
    return 0;
}

int Polyhedron::preprocess_edges(int& nume, int numeMax, Edge* edges)
{
	DBG_START;
    nume = 0;
    for (int i = 0; i < numf; ++i)
    {
        int nv = facet[i].nv;
        int * index = facet[i].index;
        for (int j = 0; j < nv; ++j)
        {
            preed_add(nume, // Number of already added edges to the list
                    numeMax, // Number of edges which we are going add finally
                    edges, // Array of edges
                    index[j], // First vertex
                    index[j + 1], // Second vertex
                    i, // Current facet id
                    index[nv + 1 + j]); // Id of its neighbour
        }
    }
#ifndef NDEBUG
    for (int i = 0; i < nume; ++i)
    {
        edges[i].my_fprint(stdout);
    }
#endif

    DBG_END;
    return 0;
}

int Polyhedron::corpol_prep_build_lists_of_visible_edges(
        int nume, 
        Edge* edges, 
        int N, 
        SContour* contours)
{
	DBG_START;
    Plane pi0, pi1;
    double sign0, sign1;
    Vector3d nu;
    
    int * buf = new int[N];
    
    int nc;
    
    for (int i = 0; i < nume; ++i)
    {
        nc = 0;
        pi0 = facet[edges[i].f0].plane;
        pi1 = facet[edges[i].f1].plane;
        for (int j = 0; j < N; ++j)
        {
            nu = contours[j].plane.norm;
            sign0 = pi0.norm * nu + pi0.dist;
            sign1 = pi1.norm * nu + pi1.dist;
            if (sign0 * sign1 <= 0)
            {
                buf[nc] = j;
                ++nc;
            }
        }
        edges[i].numc = nc;
        
        edges[i].contourNums = new int[nc];
        for (int j = 0; j < nc; ++j)
        {
            edges[i].contourNums[j] = buf[j];
        }
        
        edges[i].contourNearestSide = new int[nc];
        edges[i].contourDirection = new bool[nc];
    }
    
    if (buf != NULL)
    {
        delete[] buf;
        buf = NULL;
    }
    DBG_END;
    return 0;
}

int Polyhedron::corpol_prep_map_between_edges_and_contours(
        int nume, 
        Edge* edges, 
        int N, 
        SContour* contours)
{
    // NOTE: Testing needed.
    for (int i = 0; i < nume; ++i)
    {
        int nc = edges[i].numc;
        int * cnums = edges[i].contourNums;
        Vector3d A0 = vertex[edges[i].v0];
        Vector3d A1 = vertex[edges[i].v1];
        for (int j = 0; j < nc; ++j)
        {
            int ns = contours[cnums[j]].ns;
            SideOfContour * sides = contours[cnums[j]].sides;
            int idmin = -1;
            double min = 0.;
            bool direction = true;
            for (int k = 0; k < ns; ++k)
            {
                Vector3d B0 = sides[k].A1;
                Vector3d B1 = sides[k].A2;
                double dist0 = qmod(A0 - B0) + qmod(A1 - B1);
                double dist1 = qmod(A0 - B1) + qmod(A1 - B0);
                if (dist0 < min || idmin == -1)
                {
                    min = dist0;
                    idmin = k;
                    direction = true;
                }
                if (dist1 < min)
                {
                    min = dist1;
                    idmin = k;
                    direction = false;
                }
            }
            edges[i].contourNearestSide[j] = idmin;
            edges[i].contourDirection[j] = direction;
        }
    }
    return 0;
}

void Polyhedron::preed_add(int& nume, int numeMax, Edge* edges, 
        int v0, int v1, int f0, int f1)
{
#ifndef NDEBUG
    printf("\n\n----------------\nTrying to add edge (%d, %d) \n", v0, v1);
    
    for (int i = 0; i < nume; ++i)
        printf("\tedges[%d] = (%d, %d)\n", i, edges[i].v0, edges[i].v1);
#endif 
    if (nume >= numeMax)
    {
#ifndef NDEBUG
    printf("Warning. List is overflow\n");
#endif
        return;
    }
    
    if (v0 > v1)
    {
        int tmp = v0;
        v0 = v1;
        v1 = tmp;
    }
    
    if (f0 > f1)
    {
        int tmp = f0;
        f0 = f1;
        f1 = tmp;
    }
    
    int retvalfind = preed_find(nume, edges, v0, v1);
#ifndef NDEBUG
    printf("retvalfind = %d\n", retvalfind);
#endif
    if (edges[retvalfind].v0 == v0 && 
        edges[retvalfind].v1 == v1)
    {
#ifndef NDEBUG
    printf("Edge (%d, %d) already presents at position %d ! \n", v0, v1, retvalfind);
#endif
        return;
    }
    
    // If not, add current edge to array of edges :
    for (int i = nume; i > retvalfind; --i)
    {
        edges[i] = edges[i - 1];
    }
#ifndef NDEBUG
    printf("Adding edge (%d, %d) at position #%d\n", v0, v1, retvalfind);
#endif
    edges[retvalfind].v0 = v0;
    edges[retvalfind].v1 = v1;
    edges[retvalfind].f0 = f0;
    edges[retvalfind].f1 = f1;
    edges[retvalfind].id = nume;
    ++nume;
}

int Polyhedron::preed_find(int nume, Edge* edges, int v0, int v1)
{
#ifndef NDEBUG
    printf("Trying to find edge (%d, %d) \n", v0, v1);
#endif

    if (v0 > v1)
    {
        int tmp = v0;
        v0 = v1;
        v1 = tmp;
    }
    
    // Binary search :
    int first = 0; // Первый элемент в массиве
    
    int last = nume; // Последний элемент в массиве

#ifndef NDEBUG
    printf("\tfirst = %d, last = %d \n", first, last);
#endif

    while (first < last) {
            int mid = (first + last) / 2;
            int v0_mid = edges[mid].v0;
            int v1_mid = edges[mid].v1;

            if (v0 < v0_mid || (v0 == v0_mid && v1 <= v1_mid)) {
                    last = mid;
            } else {
                    first = mid + 1;
            }
#ifndef NDEBUG
            printf("\tfirst = %d, last = %d, mid = %d \n", first, last, mid);
#endif
    }
    
#ifndef NDEBUG
    printf("Return value of \"find\" = %d \n", last);
#endif
    return last;
}
