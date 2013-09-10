/* 
 * File:   SideOfContour.h
 * Author: ilya
 *
 * Created on 13 Ноябрь 2012 г., 10:01
 */

#ifndef SIDEOFCONTOUR_H
#define	SIDEOFCONTOUR_H

// Following "enum" is inherited from Octonus:

enum EEdgeType
{
	EEdgeUnknown = -1,       // Unknown / not calculated
	EEdgeRegular = 0,        // Regular edges, regular confidence
	EEdgeDummy = 1,        // Dummy edge, not existing in reality 
						   // (originating e.g. from a picture crop)
	EEdgeDust = 2,       // Low confidence because of dust on stone surface
	EEdgeCavern = 3,     // Low confidence because of cavern in the stone
	EEdgeMaxPointErr = 4,    // The edge erroneously is lower than other edges, 
							 // which define a vertex (e.g. in the culet)
	EEdgeGlare = 5      // Low confidence because of photo glare
};

class SideOfContour
{
public:
	double confidence;
	EEdgeType type;
	Vector3d A1;
	Vector3d A2;

	// SideOfContour.cpp :
	SideOfContour();
	SideOfContour(const SideOfContour& orig);
	virtual ~SideOfContour();

	bool operator ==(const SideOfContour& side) const;
	bool operator !=(const SideOfContour& side) const;

	// SideOfContour_io.cpp :
	void my_fprint(FILE* file);
	void my_fprint_short(FILE* file);

};

#endif	/* SIDEOFCONTOUR_H */

