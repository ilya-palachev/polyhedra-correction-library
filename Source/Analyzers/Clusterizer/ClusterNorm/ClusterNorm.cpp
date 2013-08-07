/* 
 * File:   ClusterNorm.cpp
 * Author: nk
 * 
 * Created on 1 Май 2012 г., 12:42
 */
#include "PolyhedraCorrectionLibrary.h"

double distCluster(ClusterNorm& cluster0, ClusterNorm& cluster1);

ClusterNorm::ClusterNorm() :
				num(0),
				numMax(0),
				P(),
				indexFacet(NULL),
				poly(NULL)
{
	DEBUG_START;
	DEBUG_END;
}

double ClusterNorm::area()
{
	DEBUG_START;
	double cluster_area = 0.;
	double areaOneFacet;

	for (int i = 0; i < num; i++)
	{
		areaOneFacet = poly->areaOfFacet(indexFacet[i]);
        DEBUG_PRINT("areaOneFacet = %lf\n", areaOneFacet);
		cluster_area += areaOneFacet;

	}
	DEBUG_END;
	return cluster_area;
}

ClusterNorm::ClusterNorm(const ClusterNorm& orig) :

				num(orig.num),
				numMax(orig.numMax),
				P(orig.P),
				indexFacet(new int[orig.numMax]),
				poly(orig.poly)
{
	DEBUG_START;
	for (int i = 0; i < num; ++i)
	{
		indexFacet[i] = orig.indexFacet[i];
	}
	poly = new Polyhedron;
	for (int i = 0; i < num; ++i)
	{
		poly->facets[i] = orig.poly->facets[i];
	}
	DEBUG_END;
}

ClusterNorm::ClusterNorm(int num_orig, int numMax_orig, SpherePoint P_orig,
		Polyhedron* poly_orig) :
				num(num_orig),
				numMax(numMax_orig),
				P(P_orig),
				indexFacet(new int[numMax_orig]),
				poly(poly_orig)
{
	DEBUG_START;
	DEBUG_END;
}

ClusterNorm::ClusterNorm(int num_orig, int numMax_orig, SpherePoint P_orig,
		int* indexFacet_orig, Polyhedron* poly_orig) :
				num(num_orig),
				numMax(numMax_orig),
				P(P_orig),
				indexFacet(new int[numMax_orig]),
				poly(poly_orig)
{
	DEBUG_START;
	for (int i = 0; i < num; ++i)
	{
		indexFacet[i] = indexFacet_orig[i];
	}
	DEBUG_END;
}

ClusterNorm::~ClusterNorm()
{
	DEBUG_START;
	if (indexFacet != NULL)
	{
		delete[] indexFacet;
		indexFacet = NULL;
	}
	DEBUG_END;
}

ClusterNorm& ClusterNorm::operator +=(ClusterNorm& cluster1)
{
	DEBUG_START;
	int i, j;
	int newNum, newNumMax;

	SpherePoint newSpherePoint;

	int* newIndexFacet;

	REGULAR_PRINT(stderr, "cluster0 = {");

	this->fprint(stdout);

	REGULAR_PRINT(stderr, "}, cluster1 = {");

	cluster1.fprint(stdout);

	REGULAR_PRINT(stderr, "}\n");

	newNum = this->num + cluster1.num;
	newNumMax = this->numMax + cluster1.numMax;

	newIndexFacet = new int[newNumMax];

	for (i = 0, j = 0; i < this->num; ++i)
	{
		newIndexFacet[j] = this->indexFacet[j];
		++j;
	}
	for (i = 0; i < cluster1.num; ++i)
	{
		newIndexFacet[j] = cluster1.indexFacet[i];
		++j;
	}

	REGULAR_PRINT(stderr, "newIndexFacet = {");
    for (i = 0; i < newNum - 1; ++i) {
        DEBUG_PRINT("%d, ", newIndexFacet[i]);
    }
    REGULAR_PRINT(stderr, "%d}\n", newIndexFacet[newNum - 1]);

	newSpherePoint = MassCentre(newNum, newIndexFacet, poly);

	*this = ClusterNorm(newNum, newNumMax, newSpherePoint, newIndexFacet, poly);

	REGULAR_PRINT(stderr, "newCluster: = {");
    this->fprint(stdout);
    REGULAR_PRINT(stderr, "}\n");

    DEBUG_END;
	return *this;
}

ClusterNorm& ClusterNorm::operator =(const ClusterNorm& orig)
{
	DEBUG_START;
	num = orig.num;
	numMax = orig.numMax;
	P = orig.P;
	poly = orig.poly;

	if (indexFacet != NULL)
	{
		delete[] indexFacet;
		indexFacet = NULL;
	}

	indexFacet = new int[numMax];
	for (int i = 0; i < num; ++i)
	{
		indexFacet[i] = orig.indexFacet[i];
	}
	DEBUG_START;
	return *this;
}

double distCluster(ClusterNorm& cluster0, ClusterNorm& cluster1)
{
	DEBUG_START;
	DEBUG_END;
	return distSpherePoint(cluster0.P, cluster1.P);
}

void ClusterNorm::fprint(FILE* file)
{
	DEBUG_START;
    REGULAR_PRINT(file, "P = (%lf, %lf, %lf)  ", P.vector.x, P.vector.y, P.vector.z);

	if (num < 1)
		return;
	for (int i = 0; i < num; ++i)
	{
		REGULAR_PRINT(file, "%d, ", indexFacet[i]);
	}

	double a = area();
	REGULAR_PRINT(file, "; area = %lf", a);
	DEBUG_END;
}

void ClusterNorm::setColor(char red, char green, char blue)
{
	DEBUG_START;
	for (int i = 0; i < num; ++i)
	{
		poly->facets[indexFacet[i]].set_rgb(red, green, blue);
	}
	DEBUG_END;
}
