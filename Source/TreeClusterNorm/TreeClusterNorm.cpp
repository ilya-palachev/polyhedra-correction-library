/* 
 * File:   TreeClusterNorm.cpp
 * Author: nk
 * 
 * Created on 1 Май 2012 г., 12:43
 */
#include "PolyhedraCorrectionLibrary.h"

TreeClusterNorm::TreeClusterNorm() :
				root(NULL) {
}

TreeClusterNorm::TreeClusterNorm(
		const TreeClusterNorm& orig) :
				root(orig.root) {
}

TreeClusterNorm::TreeClusterNorm(
		TreeClusterNormNode* root_orig) :
				root(root_orig) {
}

TreeClusterNorm::~TreeClusterNorm() {
}

void TreeClusterNorm::fprint(
		FILE* file) {
	root->fprint(file, 0);
}

//void TreeClusterNorm::fprint_dendrogamma(FILE* file) {
//    root->fprint(file, 0);
//}

void TreeClusterNorm::fprint_dendrogamma_lev(
		FILE* file) {
	for (int i = 0; i < 4; i++) {
		root->fprint_dendrogramma_lev(file, 0, i);
		fprintf(file, "\n");
	}
	root->cluster->fprint(stdout);

}
