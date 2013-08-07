#include "PolyhedraCorrectionLibrary.h"

bool Facet::consect_x(double y, double z, double& x)
{

	int i;
	double u, delta, alpha, sum;
	Vector3d A, A0, A1, normal;
	double a, b, c, d;

	normal = plane.norm;
	a = normal.x;
	b = normal.y;
	c = normal.z;
	d = plane.dist;

	if (fabs(a) < 1e-16)
		return false;

	u = -(b * y + c * z + d) / a;
	A = Vector3d(u, y, z);

	normal.norm(1.);
    DEBUG_PRINT("\t\t|n| = %lf,  ", sqrt(qmod(normal)));

	sum = 0.;
	for (i = 0; i < numVertices; ++i)
	{
		A0 = parentPolyhedron->vertices[indVertices[i % numVertices]] - A;
		A1 = parentPolyhedron->vertices[indVertices[(i + 1) % numVertices]] - A;
		delta = (A0 % A1) * normal;
		delta /= sqrt(qmod(A0) * qmod(A1));
		alpha = asin(delta);
        DEBUG_PRINT(" %lf ", alpha / M_PI * 180);
		sum += alpha;
	}

	DEBUG_PRINT(" = sum = %lf*\n", sum / M_PI * 180);
	DEBUG_PRINT("facet[%d].consect_x(%lf, %lf) : sum = %lf\n", id, y, z, sum);

	if (fabs(sum) < 2 * M_PI)
	{
		return false;
	}
	else
	{
		x = u;
		return true;
	}

}

