#include "math.h"
#include "Vector3d.h"

//#include "matrix_invert.h"
#include "Gauss_string.h"

Vector3d & Vector3d::norm(double d)
{
	double t = x * x + y * y + z * z;
	//    if ( t )
	if (_nequal(t, 0))
	{
		d /= sqrt(t);
		x *= d;
		y *= d;
		z *= d;
	}
	return *this;
}

Vector3d Vector3d::perpendicular() const
{
	double ax = fabs(x);
	double ay = fabs(y);
	double az = fabs(z);
	if (ax >= ay)
	{
		if (ax >= az)
		{
			//            if ( ! ax ) return Vector3d (1.,0.,0.);
			if (_equal(ax, 0))
				return Vector3d(1., 0., 0.);
			double d = 1. / sqrt(ax * ax + ay * ay);
			return Vector3d(-y * d, x * d, 0.);
		}
	}
	else
	{
		if (ay >= az)
		{
			double d = 1. / sqrt(ay * ay + az * az);
			return Vector3d(0., -z * d, y * d);
		}
	}
	double d = 1. / sqrt(ax * ax + az * az);
	return Vector3d(z * d, 0., -x * d);
}

double length(const Vector3d & v)
{
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

void reper(const Vector3d & x, Vector3d & y, Vector3d & z)
{
	y = x.perpendicular();
	z = x % y;
}

///////////////////////////////////////////////////////////////////////////////

Plane::Plane(const Vector3d & x, const Vector3d & y, const Vector3d & z)
{
	const Vector3d xy = x - y;
	const double d1 = qmod(xy);
	const Vector3d yz = y - z;
	const double d2 = qmod(yz);
	const Vector3d zx = z - x;
	const double d3 = qmod(zx);
	const double d123 = d1 + d2 + d3;
	norm = xy % yz;
	if (norm * norm < 1e-22 * d123 * d123)
	{
		if (d1 >= d2)
			if (d1 >= d3)
				norm = xy.perpendicular();
			else
				norm = zx.perpendicular();
		else if (d2 >= d3)
			norm = yz.perpendicular();
		else
			norm = zx.perpendicular();
	}
	else
		norm.norm();
	dist = ((x + y + z) * norm) / -3.;
}

// �������� ����� �� ���������

Vector3d Plane::project(const Vector3d & v) const
{
	return v - (*this % v) * norm;
}

// ����������� ���� ����������

//bool intersection(const Plane & plane1, const Plane & plane2, const Plane & plane3, Vector3d & point) {
//    const double d =
//            plane1.norm.x * (plane2.norm.y * plane3.norm.z - plane3.norm.y * plane2.norm.z) +
//            plane2.norm.x * (plane3.norm.y * plane1.norm.z - plane1.norm.y * plane3.norm.z) +
//            plane3.norm.x * (plane1.norm.y * plane2.norm.z - plane2.norm.y * plane1.norm.z);
//
//    //    if ( d == 0 ) return false;
//    if (_equal(d, 0)) return false;
//
//    point.x =
//            plane1.dist * (plane2.norm.y * plane3.norm.z - plane3.norm.y * plane2.norm.z) +
//            plane2.dist * (plane3.norm.y * plane1.norm.z - plane1.norm.y * plane3.norm.z) +
//            plane3.dist * (plane1.norm.y * plane2.norm.z - plane2.norm.y * plane1.norm.z);
//
//    point.y =
//            plane1.norm.x * (plane2.dist * plane3.norm.z - plane3.dist * plane2.norm.z) +
//            plane2.norm.x * (plane3.dist * plane1.norm.z - plane1.dist * plane3.norm.z) +
//            plane3.norm.x * (plane1.dist * plane2.norm.z - plane2.dist * plane1.norm.z);
//
//    point.z =
//            plane1.norm.x * (plane2.norm.y * plane3.dist - plane3.norm.y * plane2.dist) +
//            plane2.norm.x * (plane3.norm.y * plane1.dist - plane1.norm.y * plane3.dist) +
//            plane3.norm.x * (plane1.norm.y * plane2.dist - plane2.norm.y * plane1.dist);
//
//    point *= -1. / d;
//    return true;
//}

//bool intersection(const Plane & plane1, const Plane & plane2, const Plane & plane3, Vector3d & point) {
//    A = Matrix(3, 3);
//    A(1, 1) = plane1.norm.x;
//    A(1, 2) = plane1.norm.y;
//    A(1, 3) = plane1.norm.z;
//    A(2, 1) = plane2.norm.x;
//    A(2, 2) = plane2.norm.y;
//    A(2, 3) = plane2.norm.z;
//    A(3, 1) = plane3.norm.x;
//    A(3, 2) = plane3.norm.y;
//    A(3, 3) = plane3.norm.z;
//    B = Matrix(3, 1);
//    B(1, 1) = - plane1.dist;
//    B(2, 1) = - plane2.dist;
//    B(3, 1) = - plane3.dist;
//    B = B / A;
//    printf("%lf, %lf, %lf\n", B(1, 1), B(2, 1), B(3, 1));
//}
bool intersection(const Plane & plane1, const Plane & plane2,
		const Plane & plane3, Vector3d & point)
{
	double *A, *b/*, *INV*/;
	bool ret;
	A = new double[9];
	b = new double[3];
//    INV = new double[9];
	A[0] = plane1.norm.x;
	A[1] = plane1.norm.y;
	A[2] = plane1.norm.z;
	A[3] = plane2.norm.x;
	A[4] = plane2.norm.y;
	A[5] = plane2.norm.z;
	A[6] = plane3.norm.x;
	A[7] = plane3.norm.y;
	A[8] = plane3.norm.z;
	b[0] = -plane1.dist;
	b[1] = -plane2.dist;
	b[2] = -plane3.dist;
//    invert(A, INV, 3);
	ret = Gauss_string(3, A, b);
//    point.x = b[0] * A[0] + b[1] * A[1] + b[2] * A[2];
//    point.y = b[0] * A[3] + b[1] * A[4] + b[2] * A[5];
//    point.z = b[0] * A[6] + b[1] * A[7] + b[2] * A[8];
	point.x = b[0];
	point.y = b[1];
	point.z = b[2];
	return ret;
}

// ����������� ���� ����������
bool intersection(const Plane & plane1, const Plane & plane2, Vector3d & dir,
		Vector3d & point)
{
	const Vector3d v = plane1.norm % plane2.norm;
	const double s = v * v;
	if (s < 1e-16)
	{
		return false;
	}
	dir = v;
	dir /= sqrt(s);

	const double ax = fabs(plane1.norm.x);
	const double ay = fabs(plane1.norm.y);
	const double az = fabs(plane1.norm.z);
	if (ax >= ay && ax >= az)
	{
		const double c = plane2.norm.x / plane1.norm.x;
		const double y = plane2.norm.y - c * plane1.norm.y;
		const double z = plane2.norm.z - c * plane1.norm.z;
		const double d = plane2.dist - c * plane1.dist;
		if (y * z < 0)
		{
			point.y = d / (z - y);
			point.z = -point.y;
		}
		else
		{
			point.y = point.z = -d / (z + y);
		}
		point.x = -(point.y * plane1.norm.y + point.z * plane1.norm.z
				+ plane1.dist) / plane1.norm.x;
	}
	else if (ay >= az)
	{
		const double c = plane2.norm.y / plane1.norm.y;
		const double x = plane2.norm.x - c * plane1.norm.x;
		const double z = plane2.norm.z - c * plane1.norm.z;
		const double d = plane2.dist - c * plane1.dist;
		if (x * z < 0)
		{
			point.x = d / (z - x);
			point.z = -point.x;
		}
		else
		{
			point.x = point.z = -d / (z + x);
		}
		point.y = -(point.x * plane1.norm.x + point.z * plane1.norm.z
				+ plane1.dist) / plane1.norm.y;
	}
	else
	{
		const double c = plane2.norm.z / plane1.norm.z;
		const double y = plane2.norm.y - c * plane1.norm.y;
		const double x = plane2.norm.x - c * plane1.norm.x;
		const double d = plane2.dist - c * plane1.dist;
		if (y * x < 0)
		{
			point.y = d / (x - y);
			point.x = -point.y;
		}
		else
		{
			point.y = point.x = -d / (x + y);
		}
		point.z = -(point.y * plane1.norm.y + point.x * plane1.norm.x
				+ plane1.dist) / plane1.norm.z;
	}
	return true;
}

// ����������� ��������� � ������

bool intersection(const Plane & plane, const Vector3d & dir,
		const Vector3d & point, Vector3d & res)
{
	const double s = plane.norm * dir;
	if (fabs(s) < 1e-8)
	{
		return false;
	}
	res = point - dir * ((plane % point) / s);
	return true;
}

// ��������� ���������� ����� �������

bool cut(const Plane & plane, Segment3d & seg)
{
	const double a = plane % seg.a;
	const double b = plane % seg.b;
	if (a <= 0)
	{
		if (b > 0)
		{
			const double c = b - a;
			seg.b = seg.a * (b / c) - seg.b * (a / c);
		}
	}
	else
	{
		if (b > 0)
			return false;
		const double c = b - a;
		seg.a = seg.a * (b / c) - seg.b * (a / c);
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////

Spin::Spin(Vector3d axis, double angle)
{
	if (!axis)
	{
		t = 1.;
		x = y = z = 0.;
	}
	else
	{
		angle *= 0.5;
		t = cos(angle);
		axis.norm(sin(angle));
		x = axis.x;
		y = axis.y;
		z = axis.z;
	}
}

Spin::Spin(Vector3d v1, Vector3d v2)
{
	t = v1.norm() * v2.norm();
	if (!v1 || !v2 || t >= 1.)
	{
		t = 1.;
		x = y = z = 0.;
		return;
	}
	t += 1.;
	Vector3d v = v1 % v2;
	double d = t * t + v * v;
	if (t <= 0 || d < 1e-18)
	{
		v = v1.perpendicular();
		t = 0.;
		x = v.x;
		y = v.y;
		z = v.z;
		return;
	}
	d = 1. / sqrt(d);
	t *= d;
	x = v.x * d;
	y = v.y * d;
	z = v.z * d;
}

Spin::Spin(const Vector3d & vx, const Vector3d & vy, const Vector3d & vz)
{
	double la = 0.25 * (1. + vx.x - vy.y - vz.z);
	double mu = 0.25 * (1. - vx.x + vy.y - vz.z);
	double nu = 0.25 * (1. - vx.x - vy.y + vz.z);
	double ro = 0.25 * (1. + vx.x + vy.y + vz.z);
	la = (la <= 0) ? 0. : sqrt(la);
	mu = (mu <= 0) ? 0. : sqrt(mu);
	nu = (nu <= 0) ? 0. : sqrt(nu);
	ro = (ro <= 0) ? 0. : sqrt(ro);
	if (la >= mu)
		if (la >= nu)
		{
			if (vx.y + vy.x < 0)
				mu = -mu;
			if (vy.z - vz.y < 0)
				ro = -ro;
			if (vz.x + vx.z < 0)
				nu = -nu;
		}
		else
		{
			if (vx.y - vy.x < 0)
				ro = -ro;
			if (vy.z + vz.y < 0)
				mu = -mu;
			if (vz.x + vx.z < 0)
				la = -la;
		}
	else if (nu >= mu)
	{
		if (vx.y - vy.x < 0)
			ro = -ro;
		if (vy.z + vz.y < 0)
			mu = -mu;
		if (vz.x + vx.z < 0)
			la = -la;
	}
	else
	{
		if (vx.y + vy.x < 0)
			la = -la;
		if (vy.z + vz.y < 0)
			nu = -nu;
		if (vz.x - vx.z < 0)
			ro = -ro;
	}
	x = la;
	y = mu;
	z = nu;
	t = ro;
}

Spin & Spin::operator *=(const Spin & s)
{
	double st = t * s.t - x * s.x - y * s.y - z * s.z;
	double sx = t * s.x + x * s.t + y * s.z - z * s.y;
	double sy = t * s.y + y * s.t + z * s.x - x * s.z;
	double sz = t * s.z + z * s.t + x * s.y - y * s.x;
	double d = st * st + sx * sx + sy * sy + sz * sz;
	if (fabs(d - 1.) > 1e-14)
	{
		d = 1. / sqrt(d);
		t = st * d;
		x = sx * d;
		y = sy * d;
		z = sz * d;
	}
	else
	{
		t = st;
		x = sx;
		y = sy;
		z = sz;
	}
	return *this;
}

Vector3d Spin::getAxis() const
{
	//    if ( !x && !y && !z )
	if (_equal(x, 0) && _equal(y, 0) && _equal(z, 0))
	{
		return Vector3d(1., 0., 0.);
	}
	else
	{
		Vector3d v(x, y, z);
		return v.norm();
	}
}

double Spin::getAngle() const
{
	return fabs(t) >= 1. ? 0. : 2. * acos(t);
}

void Spin::getReper(Vector3d & ax, Vector3d & ay, Vector3d & az) const
{
	const double xx = x * x;
	const double yy = y * y;
	const double zz = z * z;
	const double tt = t * t;
	const double xy = x * y;
	const double yz = y * z;
	const double zx = z * x;
	const double xt = x * t;
	const double yt = y * t;
	const double zt = z * t;
	ax.x = tt + xx - yy - zz;
	ax.y = 2. * (xy - zt);
	ax.z = 2. * (zx + yt);
	ay.x = 2. * (xy + zt);
	ay.y = tt + yy - zz - xx;
	ay.z = 2. * (yz - xt);
	az.x = 2. * (zx - yt);
	az.y = 2. * (yz + xt);
	az.z = tt + zz - xx - yy;
}

///////////////////////////////////////////////////////////////////////////////

