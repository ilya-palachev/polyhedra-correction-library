/*
 * TreeClusterNorm.h
 *
 * Created on:  01.05.2012
 *     Author:  Kaligin Nikolai <nkaligin@yandex.ru>
 */

#ifndef TREECLUSTERNORM_H
#define TREECLUSTERNORM_H

class TreeClusterNorm
{
public:
	TreeClusterNormNode* root;

	TreeClusterNorm();
	TreeClusterNorm(TreeClusterNormNode* root_orig);
	TreeClusterNorm(const TreeClusterNorm& orig);
	virtual ~TreeClusterNorm();

	void fprint(FILE* file);
//    void fprint_dendrogamma(FILE* file);
	void fprint_dendrogamma_lev(FILE* file);

private:

};

#endif /* TREECLUSTERNORM_H */
