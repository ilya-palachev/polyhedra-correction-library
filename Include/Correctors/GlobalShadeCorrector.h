/*
 * GlobalShadeCorrector.h
 *
 *  Created on: 12.05.2013
 *      Author: iliya
 */

#ifndef GLOBALSHADECORRECTOR_H_
#define GLOBALSHADECORRECTOR_H_

/*
 *
 */
struct GSCorrectorParameters
{
	double epsLoopStop;
};

const double EPS_LOOP_STOP_DEFAULT = 1e-6;

class GlobalShadeCorrector: public PCorrector
{
private:
	int numEdges;
	Edge* edges;

	int numContours;
	SContour* contours;

	GSCorrectorParameters parameters;

	list<int>* facetsNotAssociated;

	double* gradient;
	Plane* prevPlanes;

	int dim;

	GlobalShadeCorrector(const Polyhedron& polyhedron);

	void preprocess();
	void preprocessEdges();
	void preprocessAssociations();

	int findEdge(int v0, int v1);
	void addEdge(int numEdgesMax, int v0, int v1, int f0, int f1);

	void findNotAssociatedFacets();

	double calculateFunctional();

	void runCorrectionIteration();

	void calculateGradient();

	void derivativeTest_all();
	void derivativeTest_1();
	double calculateFunctionalDerivative_1(int iFacet, int iCoefficient);

//	//Poyhedron_correction.cpp
//	int correct_polyhedron();
//
//	list<int>* corpol_find_not_associated_facets();
//
//	double corpol_calculate_functional(Plane* prevPlanes);
//
//	void corpol_iteration(int dim, Plane* prevPlanes, double* gradient,
//			list<int>* facetsNotAssociated);
//
//	void corpol_calculate_gradient(int dim, Plane* prevPlanes, double* gradient,
//			list<int>* facetsNotAssociated);
//
//	// Polyhedron_correction_derivativeTest.cpp
//
//	void corpol_derivativeTest_all(Plane* prevPlanes, double* matrix,
//			list<int>* facetsNotAssociated);
//
//	void corpol_derivativeTest_1(Plane* prevPlanes, double* matrix,
//			list<int>* facetsNotAssociated);
//
//	double corpol_calculate_functional_derivative_1(Plane* prevPlanes,
//			int iFacet, int iCoefficient);
//
//	// Polyhedron_correction_preprocess.cpp
//	void corpol_preprocess();
//
//	void preprocess_edges();
//	inline void preprocess_edges_add(int numeMax, int v0, int v1, int f0,
//			int f1);
//
//	int preprocess_edges_find(int v0, int v1);
//
//	void corpol_prepFindAssociations();
//
//	void corpol_prepFindAssociations_init(Polyhedron* &polyhedronTmp,
//			double* &bufDouble);
//
//	void corpol_prepFindAssiciations_withContour(int iContour,
//			Polyhedron* polyhedronTmp, double* bufDouble);
//
//	void corpol_prepFindAssociations_withContour_forFacet(int iContour,
//			int iFacet, Polyhedron* polyhedronTmp, double* bufDouble);
//
//	double corpol_weightForAssociation(int iContour, int iEdge);
//
//	void corpol_prepAssociator(int iContour, int iFacet, int iEdge,
//			Polyhedron* polyhedronTmp, double* bufDouble);
//
//	inline int corpol_prepAssociator_init(int iContour, int iFacet, int iEdge,
//			Polyhedron* polyhedronTmp, Vector3d& v0_projected,
//			Vector3d& v1_projected);
//
//	double corpol_prepAssociator_visibility(int iContour, int iEdge);
//
//	int corpol_prepAssociator_checkVisibility(int iContour, int iFacet,
//			int iEdge);
//
//	inline int corpol_prepAssociator_checkAlreadyAdded(int iContour, int iFacet,
//			int iEdge);
//
//	inline int corpol_prepAssociator_checkExtinction(int iContour, int iFacet,
//			int iEdge, Vector3d v0_projected, Vector3d v1_projected);
//
//	inline void corpol_prepAssociator_project(int iContour, int iFacet,
//			int iEdge, Vector3d& v0_projected, Vector3d& v1_projected);
//
//	inline int corpol_prepAssociator_findNearest(int iContour,
//			Vector3d v_projected, Vector3d& v_nearest);
//
//	inline double corpol_prepAssociator_calcArea(int iContour,
//			Polyhedron* polyhedronTmp, int iSideDistMin0, int iSideDistMin1,
//			Vector3d v0_nearest, Vector3d v1_nearest, Orientation orientation);
//
//	inline void corpol_prepAssociator_add(int iContour, int iFacet, int iEdge,
//			int iSideDistMin0, int iSideDistMin1, Vector3d v0_nearest,
//			Vector3d v1_nearest, Orientation orientation, double* bufDouble);
//
//	inline void corpol_prepAssociator_add_findBounds(int iContour,
//			int iSideDistMin0, int iSideDistMin1, Vector3d v0_nearest,
//			Vector3d v1_nearest, Orientation orientation, int& iResultBegin,
//			int& iResultEnd, double* bufDouble);
//
//	bool corpol_edgeIsVisibleOnPlane(Edge& edge, Plane planeOfProjection);
//	bool corpol_collinear_visibility(int v0, int v1, Plane planeOfProjection,
//			int ifacet);

public:
	GlobalShadeCorrector(Polyhedron* input);
	~GlobalShadeCorrector();

	void runCorrection();
};

#endif /* GLOBALSHADECORRECTOR_H_ */
