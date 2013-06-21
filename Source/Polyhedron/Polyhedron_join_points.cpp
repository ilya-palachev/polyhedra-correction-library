#include "PolyhedraCorrectionLibrary.h"

int Polyhedron::join_points(int id)
{
	VertexGrouper* vertexGrouper = new VertexGrouper(this);
	int numGroupedPoints = vertexGrouper->join_points(id);
	delete vertexGrouper;
	return numGroupedPoints;
}
