#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "PolyhedraCorrectionLibrary.h"

void Facet::clear_bad_vertexes() {
    for (int i = 0; i < nv - 1; ++i) {
        if (index[nv + 1 + i] == index[nv + 2 + i]) {
            poly->delete_vertex_polyhedron(index[i + 1]);
            this->delete_vertex(index[i + 1]);
            this->my_fprint_all(stdout);
            --i;
        }
    }
    if (index[2 * nv] == index[nv + 1]) {
        poly->delete_vertex_polyhedron(index[0]);
        this->delete_vertex(index[0]);
        this->my_fprint_all(stdout);
    }
}

void Facet::delete_vertex(int v) {
    int i, j, found = 0;
    for (i = 0; i < nv; ++i) {
        if (index[i] == v) {
            found = 1;
            for (j = i; j < nv - 1; ++j)
                index[2 * nv + 1 + j] = index[2 * nv + 2 + j];
            for (j = i; j < 2 * nv - 2; ++j)
                index[nv + 1 + j] = index[nv + 2 + j];
            for (j = i; j < 3 * nv - 2; ++j)
                index[j] = index[j + 1];
            --nv;
            if (i == 0)
                index[nv] = index[0];
        }
    }
    if (found == 1) {
        printf("\tdelete_vertex(%d) in facet %d\n", v, id);
        //        this->my_fprint_all(stdout);
    }
}

void Facet::add_before_position(int pos, int v, int next_f) {
    int* index1;
    int nv1 = nv + 1;
    int i;

    this->my_fprint(stdout);

    index1 = new int[3 * nv1 + 1];

    for (i = 0; i < pos; ++i) {
        index1[i] = index[i];
    }
    index1[pos] = v;
    for (i = pos + 1; i < nv1 + 1 + pos; ++i) {
        index1[i] = index[i - 1];
    }
    index1[nv1 + 1 + pos] = next_f;
    for (i = nv1 + 2 + pos; i < 2 * nv1 + 1 + pos; ++i) {
        index1[i] = index[i - 2];
    }
    index1[2 * nv1 + 1 + pos] = -1;
    for (i = 2 * nv1 + 1 + pos; i < 3 * nv1 + 1; ++i) {
        index1[i] = index[i - 3];
    }
    if (index != NULL)
        delete[] index;
    index = index1;
    nv = nv1;

    printf("add_before_position : ");
    this->my_fprint(stdout);
}

void Facet::add_after_position(int pos, int v, int next_f) {
    int* index1;
    int nv1 = nv + 1;
    int i;

    index1 = new int[3 * nv1 + 1];

    for (i = 0; i < pos + 1; ++i) {
        index1[i] = index[i];
    }
    index1[pos + 1] = v;
    for (i = pos + 2; i < nv1 + 1 + pos; ++i) {
        index1[i] = index[i - 1];
    }
    index1[nv1 + 2 + pos] = index1[nv1 + 1 + pos];
    index1[nv1 + 1 + pos] = next_f;
    for (i = nv1 + 3 + pos; i < 2 * nv1 + 2 + pos; ++i) {
        index1[i] = index[i - 2];
    }
    index1[2 * nv1 + 2 + pos] = -1;
    for (i = 2 * nv1 + 2 + pos; i < 3 * nv1 + 1; ++i) {
        index1[i] = index[i - 3];
    }
    if (index != NULL)
        delete[] index;
    index = index1;
    nv = nv1;

    printf("add_after_position : ");
    this->my_fprint(stdout);

}

void Facet::find_next_facet(int v, int& fid_next) {
    int i;
    for (i = 0; i < nv; ++i)
        if (index[i] == v)
            break;
    if (i == nv) {
        fid_next = -1;
        printf("Facet::find_next_facet : Error. Cannot find vertex %d at facet %d",
                v, id);
        return;
    }
    fid_next = index[nv + 1 + i];
}

void Facet::find_next_facet2(int v, int& fid_next) {
    int i;
    for (i = 0; i < nv; ++i)
        if (index[i] == v)
            break;
    if (i == nv) {
        fid_next = -1;
        printf("Facet::find_next_facet2 : Error. Cannot find vertex %d at facet %d",
                v, id);
        return;
    }
    i = i > 0 ? i - 1 : nv - 1;
    fid_next = index[nv + 1 + i];
}

void Facet::find_and_replace2(int from, int to) {
    int i;
    for (i = 0; i < nv; ++i)
        if (index[i] == to)
            break;
    if (i < nv) {
        this->delete_vertex(from);
        return;
    }
    for (int i = 0; i < nv + 1; ++i)
        if (index[i] == from)
            index[i] = to;
}

int Facet::find_total(int what) {
    int i;
    for (i = 0; i < 3 * nv + 1; ++i)
        if (index[i] == what)
            return i;
    return -1;
}

int Facet::find_vertex(int what) {
    int i;
    for (i = 0; i < nv; ++i)
        if (index[i] == what)
            return i;
    return -1;
}

void Facet::add(int what, int pos) {

    int i, *index_new;
    printf("add %d at position %d in facet %d\n", what, pos, id);
    printf("{{{\n");
    this->my_fprint_all(stdout);

    if (pos < nv) {
        index_new = new int[3 * nv + 4];

        for (i = 3 * nv + 3; i > 2 * (nv + 1) + 1 + pos; --i) {
            index_new[i] = index[i - 3];
        }
        index_new[2 * (nv + 1) + 1 + pos] = -1;
        for (i = 2 * (nv + 1) + pos; i > nv + 1 + 1 + pos; --i) {
            index_new[i] = index[i - 2];
        }
        index_new[nv + 1 + 1 + pos] = -1;
        for (i = nv + 1 + pos; i > pos; --i) {
            index_new[i] = index[i - 1];
        }
        index_new[pos] = what;
        for (i = pos - 1; i >= 0; --i) {
            index_new[i] = index[i];
        }
        if (index != NULL)
            delete[] index;
        index = index_new;
        ++nv;
    } else {
        index[pos] = what;
    }
    this->my_fprint_all(stdout);
    test_pair_neighbours();
    printf("}}}\n");
}

void Facet::remove(int pos) {
    int i;
    printf("remove position %d (vertex %d) in facet %d\n", pos, index[pos], id);
    printf("{{{\n");
    this->my_fprint_all(stdout);

    for (i = pos; i < nv + pos; ++i) {
        index[i] = index[i + 1];
    }
    for (i = nv + pos; i < pos + 2 * nv - 1; ++i) {
        index[i] = index[i + 2];
    }
    for (i = pos + 2 * nv - 1; i < 3 * nv - 2; ++i) {
        index[i] = index[i + 3];
    }
    --nv;
    this->my_fprint_all(stdout);
    test_pair_neighbours();
    printf("}}}\n");
}

void Facet::update_info() {

    int i, facet, pos, nnv;

    printf("update info in facet %d\n", id);
    printf("{{{\n");
    this->my_fprint_all(stdout);

    for (i = 0; i < nv; ++i) {
        facet = index[nv + 1 + i];
        pos = poly->facet[facet].find_vertex(index[i]);
        if (pos == -1) {
            printf("=======update_info: Error. Cannot find vertex %d in facet %d\n",
                    index[i], facet);
            poly->facet[facet].my_fprint_all(stdout);
            this->my_fprint_all(stdout);
            return;
        }
        index[2 * nv + 1] = pos;
        nnv = poly->facet[facet].nv;
        pos = (pos + nnv - 1) % nnv;
        if (poly->facet[facet].index[nnv + 1 + pos] != id) {
            printf("=======update_info: Error. Wrong neighbor facet for vertex %d in facet %d\n",
                    index[i], facet);
            poly->facet[facet].my_fprint_all(stdout);
            this->my_fprint_all(stdout);
            return;
        }
        poly->facet[facet].index[2 * nnv + 1 + pos] = i;
    }
    this->my_fprint_all(stdout);
    test_pair_neighbours();
    printf("}}}\n");
}

int Facet::test_structure() {

    int i, facet, pos, nnv;

    for (i = 0; i < nv; ++i) {
        facet = index[nv + 1 + i];
        pos = poly->facet[facet].find_vertex(index[i]);
        if (pos == -1) {
            printf("=======test_structure: Error. Cannot find vertex %d in facet %d\n",
                    index[i], facet);
            poly->facet[facet].my_fprint_all(stdout);
            this->my_fprint_all(stdout);
            return 1;
        }
        index[2 * nv + 1] = pos;
        nnv = poly->facet[facet].nv;
        pos = (pos + nnv - 1) % nnv;
        if (poly->facet[facet].index[nnv + 1 + pos] != id) {
            printf("=======test_structure: Error. Wrong neighbor facet for vertex %d in facet %d\n",
                    index[i], facet);            
            poly->facet[facet].my_fprint_all(stdout);
            this->my_fprint_all(stdout);
            return 1;
        }
        poly->facet[facet].index[2 * nnv + 1 + pos] = i;
    }
    return 0;
}

Vector3d& Facet::find_mass_centre() {
    int i;
    double c = 1. / (double) nv;
    Vector3d* v = new Vector3d(0., 0., 0.);
    Vector3d a;
    for (i = 0; i < nv; ++i) {
        a = poly->vertex[index[i]];
        *v += a;
        printf("facet[%d].index[%d] = (%lf, %lf, %lf)\n", id, i, a.x, a.y, a.z);
    }
    *v *= c;
    return *v;
}

void Facet::test_pair_neighbours() {
    int i, j;
    for (i = 0; i < nv; ++i) {
        for (j = 0; j < i; ++j) {
            if (index[nv + 1 + i] == index[nv + 1 + j]) {
                printf("WARNING!!! test_pair_neighbours in facet %d. neighbour[%d] = %d and neighbour[%d] = %d\n\n", 
                        id, j, index[nv + 1 + j], i, index[nv + 1 + i]);
            }
        }
            
    }
}
