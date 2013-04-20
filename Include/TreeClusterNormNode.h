/*
 * TreeClusterNormNode.h
 *
 * Created on:  01.05.2012
 *     Author:  Kaligin Nikolai <nkaligin@yandex.ru>
 */

#ifndef TREECLUSTERNORMNODE_H
#define TREECLUSTERNORMNODE_H

#include "PolyhedraCorrectionLibrary.h"

class TreeClusterNormNode {
public:
	ClusterNorm* cluster;

	TreeClusterNormNode* parent;

	TreeClusterNormNode* child0;
	TreeClusterNormNode* child1;

	TreeClusterNormNode();
	TreeClusterNormNode(
			const TreeClusterNormNode& orig);
	virtual ~TreeClusterNormNode();

	void fprint(
			FILE* file,
			int level);
//    void fprint_dendrogramma(FILE* file, int level);
	void fprint_dendrogramma_lev(
			FILE* file,
			int level,
			int fix);
private:

};

#endif /* TREECLUSTERNORMNODE_H */
