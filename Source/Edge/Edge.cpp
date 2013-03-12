/* 
 * File:   Edge.cpp
 * Author: ilya
 * 
 * Created on 19 Ноябрь 2012 г., 8:17
 */

#include "PolyhedraCorrectionLibrary.h"
#include "Edge.h"

Edge::Edge() :
        id(-1), 
        v0(-1), 
        v1(-1), 
        f0(-1), 
        f1(-1), 
        numc(-1), 
        contourNums(NULL),
        contourNearestSide(NULL), 
        contourDirection(NULL) {}

Edge::Edge(int id_orig, int v0_orig, int v1_orig, int f0_orig, int f1_orig) :
        id(id_orig), 
        v0(v0_orig), 
        v1(v1_orig), 
        f0(f0_orig), 
        f1(f1_orig), 
        numc(-1), 
        contourNums(NULL),
        contourNearestSide(NULL),
        contourDirection(NULL) {}

Edge::Edge(int v0_orig, int v1_orig, int f0_orig, int f1_orig) :
        id(-1), 
        v0(v0_orig), 
        v1(v1_orig), 
        f0(f0_orig), 
        f1(f1_orig), 
        numc(-1), 
        contourNums(NULL),
        contourNearestSide(NULL),
        contourDirection(NULL) {}

Edge::Edge(const Edge& orig)  :
        id(orig.id), 
        v0(orig.v0), 
        v1(orig.v1), 
        f0(orig.f0), 
        f1(orig.f1), 
        numc(orig.numc), 
        contourNums(NULL),
        contourNearestSide(NULL),
        contourDirection(NULL) {
    
	if (numc < 1)
	{
		return;
	}

    contourNums = new int [numc];
    
    for (int i = 0; i < numc; ++i) {
        contourNums[i] = orig.contourNums[i];
    }
    
    for (int i = 0; i < numc; ++i) {
        contourNearestSide[i] = orig.contourNearestSide[i];
    }
    
    for (int i = 0; i < numc; ++i) {
        contourDirection[i] = orig.contourDirection[i];
    }

}

Edge::~Edge() {
    
#ifndef NDEBUG
    printf("Attention! edge #%d is deleting now\n", id);
#endif
    if (contourNums != NULL)
    {
        delete[] contourNums;
        contourNums = NULL;
    }
    
    if (contourNearestSide != NULL)
    {
        delete[] contourNearestSide;
        contourNearestSide = NULL;
    }
    
    if (contourDirection != NULL)
    {
        delete[] contourDirection;
        contourDirection = NULL;
    }
}

Edge& Edge::operator =(const Edge& orig)
{
    id = orig.id;
    v0 = orig.v0;
    v1 = orig.v1;
    f0 = orig.f0;
    f1 = orig.f1;
    numc = orig.numc;
    
    if (contourNums != NULL)
    {
        delete[] contourNums;
        contourNums = NULL;
    }
    
    if (contourNearestSide != NULL)
    {
        delete[] contourNearestSide;
        contourNearestSide = NULL;
    }
    
    if (contourDirection != NULL)
    {
        delete[] contourDirection;
        contourDirection = NULL;
    }
    
    if (numc < 1)
    {
        return *this;
    }
    contourNums = new int [numc];
    contourNearestSide = new int [numc];
    contourDirection = new bool [numc];
    
    for (int i = 0; i < numc; ++i)
    {
        contourNums[i] = orig.contourNums[i];
        contourNearestSide[i] = orig.contourNearestSide[i];
        contourDirection[i] = orig.contourDirection[i];
    }
    
    return *this;
}

