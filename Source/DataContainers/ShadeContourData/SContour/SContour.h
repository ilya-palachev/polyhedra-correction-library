/* 
 * File:   SContour.h
 * Author: ilya
 *
 * Created on 13 Ноябрь 2012 г., 9:50
 */

#ifndef SCONTOUR_H
#define	SCONTOUR_H

class SContour
{
public:
	int id; // Indetifier

	int ns; // Number of sides

	Plane plane; // Plane of contour. Usually it includes (0, 0, 0),
				 // so in its equation a*x + b*y + c*z + d = 0
				 // coefficient d = 0

	Polyhedron* poly; // Pointer to parent polyhedon which given contour
					  // is the part of.

	SideOfContour* sides; // Array of contour sides

	// SContour.cpp :

	SContour();
	SContour(const SContour& orig);
	~SContour();

	SContour& operator =(const SContour& scontour);

	// SContour_io.cpp :

	void my_fprint(FILE* file);
};

#endif	/* SCONTOUR_H */

