#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Polyhedron.h"
#include "list_squares_method.h"
#include "Vector3d.h"
#include "array_operations.h"
#include "Gauss_string.h"

class SortedDouble {
public:
    int len;
    int num;
    double* array;
    
    SortedDouble(int Len);
    ~SortedDouble();
    
    void add(double x);
    double calclulate();
    
};

SortedDouble::SortedDouble(int Len) :
        len(Len), num(0)
{
    array = new double[len];
}

SortedDouble::~SortedDouble() {
    if (array != NULL)
        delete[] array;
}

void SortedDouble::add(double x) {

    int first, last, mid;
    
    if (num >= len) {
        printf("Error.OVERULL!!!\n");
        return;
    }
    
    first = 0; // Первый элемент в массиве
    last = num; // Последний элемент в массиве

    while (first < last) {
        mid = (first + last) / 2;
        if (x <= array[mid]) {
            last = mid;
        } else {
            first = mid + 1;
        }
    }
    insert_double(array, num, last, x);
    ++num;
}

double SortedDouble::calclulate() {
    int i;
    double s;
    
    s = 0;
    for (i = 0; i < num; ++i) {
        if (i % 2 == 0)
            s -= array[i];
        else
            s += array[i];
    }
    return s;
}

double Polyhedron::calculate_J11(int N) {
    
    int k, l;
    double xmin, xmax, ymin, ymax, zmin, zmax;
    double h, s, z, y, integral;
    
    get_boundary(xmin, xmax, ymin, ymax, zmin, zmax);
    xmin *= 1.001;
    xmax *= 1.001;
    ymin *= 1.001;
    ymax *= 1.001;
    zmin *= 1.001;
    zmax *= 1.001;
    
    h = 1. / N;
    
    s = 0.;
    
    for (k = 0; k < N; ++k) {
        z = k * h * (zmax - zmin) + zmin;
        for (l = 0; l < N; ++l) {
            y = l * h * (ymax - ymin) + ymin;
            integral = consection_x(y, z);
            s += h * h * integral * (z * z + y * y);
            if (fabs(integral) > 1e-16)
                    printf("integral[%d, %d] = %lf\n", k, l, integral);
        }
    }
    return s;
}

double Polyhedron::consection_x(double y, double z) {
    
    int i;
    double x;
    bool ifConsect;
    
    SortedDouble SD(20);
    
    for (i = 0; i < numVertices; ++i) {
        
        ifConsect = facets[i].consect_x(y, z, x);
        if (ifConsect == true) {
            printf("SD.add(%lf) y = %lf, z = %lf\n", x, y, z);
            SD.add(x);
        }
    }
    return SD.calclulate();
}

