/*
 * GraphDumperGEXF.h
 *
 *  Created on: Aug 19, 2013
 *      Author: ilya
 */

#ifndef GRAPHDUMPERGEXF_H_
#define GRAPHDUMPERGEXF_H_

class GraphDumperGEXF
{
private:


public:
	GraphDumperGEXF();
	~GraphDumperGEXF();

	void collect(Facet* facet);
	void dump(char* fileName);
};

#endif /* GRAPHDUMPERGEXF_H_ */
