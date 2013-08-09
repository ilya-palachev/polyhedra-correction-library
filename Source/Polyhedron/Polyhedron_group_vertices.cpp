#include "PolyhedraCorrectionLibrary.h"

int Polyhedron::groupVertices(int idSavedVertex)
{
	DEBUG_START;
	VertexGrouper* vertexGrouper = new VertexGrouper(this);
	int numGroupedPoints = vertexGrouper->run(idSavedVertex);
	delete vertexGrouper;
	return numGroupedPoints;
	DEBUG_END;
}
