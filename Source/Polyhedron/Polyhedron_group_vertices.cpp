#include "PolyhedraCorrectionLibrary.h"

int Polyhedron::groupVertices(int idSavedVertex)
{
	VertexGrouper* vertexGrouper = new VertexGrouper(this);
	int numGroupedPoints = vertexGrouper->run(idSavedVertex);
	delete vertexGrouper;
	return numGroupedPoints;
}
