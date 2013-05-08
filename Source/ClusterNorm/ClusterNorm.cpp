/* 
 * File:   ClusterNorm.cpp
 * Author: nk
 * 
 * Created on 1 Май 2012 г., 12:42
 */
#include "PolyhedraCorrectionLibrary.h"

double distCluster(ClusterNorm& cluster0, ClusterNorm& cluster1);

ClusterNorm::ClusterNorm() :
		num(0), numMax(0), P(), indexFacet(NULL), poly(NULL)
{
}

double ClusterNorm::area()
{
	double cluster_area = 0.;
	double areaOneFacet;

//    printf("\n");
	for (int i = 0; i < num; i++)
	{
		areaOneFacet = poly->area(indexFacet[i]);
//        printf("areaOneFacet = %lf\n", areaOneFacet);       
		cluster_area += areaOneFacet;

	}
	//printf(" %lf ", cluster_area);
	return cluster_area;
}

ClusterNorm::ClusterNorm(const ClusterNorm& orig) :

		num(orig.num), numMax(orig.numMax), P(orig.P), indexFacet(
				new int[orig.numMax]), poly(orig.poly)
{
	for (int i = 0; i < num; ++i)
	{
		indexFacet[i] = orig.indexFacet[i];
	}
	poly = new Polyhedron;
	for (int i = 0; i < num; ++i)
	{
		poly->facets[i] = orig.poly->facets[i];
	}
}

ClusterNorm::ClusterNorm(int num_orig, int numMax_orig, SpherePoint P_orig,
		Polyhedron* poly_orig) :
		num(num_orig), numMax(numMax_orig), P(P_orig), indexFacet(
				new int[numMax_orig]), poly(poly_orig)
{
}

ClusterNorm::ClusterNorm(int num_orig, int numMax_orig, SpherePoint P_orig,
		int* indexFacet_orig, Polyhedron* poly_orig) :
		num(num_orig), numMax(numMax_orig), P(P_orig), indexFacet(
				new int[numMax_orig]), poly(poly_orig)
{
	for (int i = 0; i < num; ++i)
	{
		indexFacet[i] = indexFacet_orig[i];
	}
}

ClusterNorm::~ClusterNorm()
{
	if (indexFacet != NULL)
	{
		delete[] indexFacet;
		indexFacet = NULL;
	}
}

ClusterNorm& ClusterNorm::operator +=(ClusterNorm& cluster1)
{

	int i, j;
	int newNum, newNumMax;

	SpherePoint newSpherePoint;

	int* newIndexFacet;

	//printf("operator+=   : cluster0 = {");
	printf("cluster0 = {");

	this->fprint(stdout);

	printf("}, cluster1 = {");

	cluster1.fprint(stdout);

	printf("}\n");

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

//    printf("newIndexFacet = {");
//    for (i = 0; i < newNum - 1; ++i) {
//        printf("%d, ", newIndexFacet[i]);
//    }
//    printf("%d}\n", newIndexFacet[newNum - 1]);

	newSpherePoint = MassCentre(newNum, newIndexFacet, poly);

	*this = ClusterNorm(newNum, newNumMax, newSpherePoint, newIndexFacet, poly);

//    printf("newCluster: = {");
//    this->fprint(stdout);
//    printf("}\n");
//    

	return *this;

}

ClusterNorm& ClusterNorm::operator =(const ClusterNorm& orig)
{
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
}

double distCluster(ClusterNorm& cluster0, ClusterNorm& cluster1)
{
//    printf("\tcluster0.P = (%lf, %lf, %lf)\n", 
//            cluster0.P.vector.x,
//            cluster0.P.vector.y,
//            cluster0.P.vector.z);
//    printf("\tcluster1.P = (%lf, %lf, %lf)\n", 
//            cluster1.P.vector.x,
//            cluster1.P.vector.y,
//            cluster1.P.vector.z);
	return dist(cluster0.P, cluster1.P);
}

void ClusterNorm::fprint(FILE* file)
{
//    fprintf(file, "P = (%lf, %lf, %lf)  ", P.vector.x, P.vector.y, P.vector.z);

	if (num < 1)
		return;
	for (int i = 0; i < num; ++i)
	{
		fprintf(file, "%d, ", indexFacet[i]);
	}

	double a = area();
	fprintf(file, "; area = %lf", a);
}

void ClusterNorm::setColor(char red, char green, char blue)
{
	for (int i = 0; i < num; ++i)
	{
		poly->facets[indexFacet[i]].set_rgb(red, green, blue);
	}
}
