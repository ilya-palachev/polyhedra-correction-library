#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <string.h>

#include "Polyhedron.h"
#include "Gauss_test.h"

int main(int argc, char** argv) {
    Polyhedron poly;
    printf("Polyhedron has been created...\n");
    poly.corpolTest(4, 0);
    return 0;
}









