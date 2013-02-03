#ifndef FACET_H
#define FACET_H

#include "Vector3d.h"

class Facet
{
//private:
public:
	int* index;
	int nv;
	Plane plane;

	char rgb[3];

public:
	Facet();
	Facet(const int* index1, const int nv1, const Plane plane1);
	Facet& operator=(const Facet& facet1);
	~Facet();

	void print_v(int facet_id);
};

#endif // FACET_H
