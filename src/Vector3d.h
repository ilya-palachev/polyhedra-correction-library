
#ifndef VECTOR3D_H
#define VECTOR3D_H


#define GCC

#ifdef  GCC

#ifndef _equal
#define _equal(a,b)   ((a)<=(b)&&(a)>=(b))
#define _nequal(a,b)  ((a)<(b)||(a)>(b))
#endif  // _equal

#else

#define _equal(a,b)   ((a)==(b))
#define _nequal(a,b)  ((a)!=(b))

#endif // GCC



class Vector3d
{
public :
    double x, y, z;

    Vector3d () : x ( 0 ), y ( 0 ), z ( 0 ) {}
    Vector3d ( double a, double b, double c ) : x ( a ), y ( b ), z ( c ) {}

    Vector3d operator - () const
    {
        return Vector3d ( - x, - y, - z );
    }

    Vector3d & operator += ( const Vector3d & v )
    {
        x += v.x;
        y += v.y;
        z += v.z;
        return * this;
    }

    Vector3d & operator -= ( const Vector3d & v )
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return * this;
    }

    Vector3d & operator *= ( const double d )
    {
        x *= d;
        y *= d;
        z *= d;
        return * this;
    }

    Vector3d & operator /= ( const double d )
    {
        x /= d;
        y /= d;
        z /= d;
        return * this;
    }

    Vector3d & null ()
    {
        x = y = z = 0.;
        return * this;
    }

    bool operator ! () const
    {
//        return !x && !y && !z;
        return _equal(x,0) && _equal(y,0) && _equal(z,0);
    }

    Vector3d & norm ( double d = 1. );
    Vector3d   perpendicular () const;
};

inline Vector3d operator + ( const Vector3d& a, const Vector3d& b )
{
    return Vector3d ( a.x + b.x, a.y + b.y, a.z + b.z );
}

inline Vector3d operator - ( const Vector3d& a, const Vector3d& b )
{
    return Vector3d ( a.x - b.x, a.y - b.y, a.z - b.z );
}

inline Vector3d operator * ( const Vector3d& a, double d )
{
    return Vector3d ( a.x * d, a.y * d, a.z * d );
}

inline Vector3d operator / ( const Vector3d& a, double d )
{
    return Vector3d ( a.x / d, a.y / d, a.z / d );
}

inline Vector3d operator * ( double d, const Vector3d& a )
{
    return Vector3d ( a.x * d, a.y * d, a.z * d );
}

inline double operator * ( const Vector3d& a, const Vector3d& b )
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline double qmod ( const Vector3d& a )
{
    return a.x * a.x + a.y * a.y + a.z * a.z;
}

inline Vector3d operator % ( const Vector3d& a, const Vector3d& b )
{
    return Vector3d ( a.y * b.z - a.z * b.y,
                      a.z * b.x - a.x * b.z,
                      a.x * b.y - a.y * b.x );
}

inline bool operator != ( const Vector3d& a, const Vector3d& b )
{
//    return a.x != b.x || a.y != b.y || a.z != b.z;
    return _nequal(a.x,b.x) || _nequal(a.y,b.y) || _nequal(a.z,b.z);
}

inline bool operator == ( const Vector3d& a, const Vector3d& b )
{
//    return a.x == b.x && a.y == b.y && a.z == b.z;
    return _equal(a.x,b.x) && _equal(a.y,b.y) && _equal(a.z,b.z);
}

void reper ( const Vector3d & x, Vector3d & y, Vector3d & z );

double length ( const Vector3d & v );

///////////////////////////////////////////////////////////////////////////////

class Segment3d
{
public:
    Vector3d a, b;

    Segment3d () {}
    Segment3d ( const Vector3d & va, const Vector3d & vb ) : a(va), b(vb) {}
};

inline bool operator != ( const Segment3d & s1, const Segment3d & s2 )
{
    return ( s1.a != s2.a ) || ( s1.b != s2.b );
}

inline bool operator == ( const Segment3d & s1, const Segment3d & s2 )
{
    return ( s1.a == s2.a ) && ( s1.b == s2.b );
}

inline double length ( const Segment3d & s )
{
    return length ( s.b - s.a );
}

///////////////////////////////////////////////////////////////////////////////

class Plane
{
public:
    Vector3d norm;
    double   dist;

    Plane () : dist ( 0 ) {}
    Plane ( const Vector3d &, const Vector3d &, const Vector3d & );
    Plane ( const Vector3d & v, const double & d ) : norm ( v ),
                                                     dist ( d ) {}
    double operator % ( const Vector3d & v ) const
    {
        return norm.x * v.x + norm.y * v.y + norm.z * v.z + dist;
    }

    Plane operator - () const
    {
        return Plane ( - norm, - dist );
    }

    Vector3d project ( const Vector3d & v ) const; // �������� ����� �� ���������
};

inline bool operator == ( const Plane & p1, const Plane & p2 )
{
//    return p1.norm == p2.norm && p1.dist == p2.dist;
    return p1.norm == p2.norm && _equal(p1.dist,p2.dist);
}

inline bool operator != ( const Plane & p1, const Plane & p2 )
{
    return p1.norm != p2.norm || _nequal(p1.dist,p2.dist);
}

// ����������� ���� ����������
bool intersection ( const Plane & plane1, const Plane & plane2, const Plane & plane3, Vector3d & point );
// ����������� ���� ����������
bool intersection ( const Plane & plane1, const Plane & plane2, Vector3d & dir, Vector3d & point );
// ����������� ��������� � ������
bool intersection ( const Plane & plane, const Vector3d & dir, const Vector3d & point, Vector3d & res );
// ��������� ���������� ����� �������
bool cut ( const Plane & plane, Segment3d & seg );

///////////////////////////////////////////////////////////////////////////////

class Spin
{
    double t, x, y, z;
public:
    Spin () { t = 1.; x = y = z = 0.; }
    Spin ( Vector3d, double );
	Spin ( Vector3d v1, Vector3d v2 );
    Spin ( const Vector3d & x, const Vector3d & y, const Vector3d & z );
    void getReper ( Vector3d & ax, Vector3d & ay, Vector3d & az ) const;
    double getAngle () const;
    Vector3d getAxis () const;
    Vector3d operator() ( const Vector3d & v ) const
    {
        Vector3d ax, ay, az;
        getReper ( ax, ay, az );
        return Vector3d ( v*ax, v*ay, v*az );
    }
    Spin & operator*= ( const Spin & );
    Spin operator ~ () const
    {
        Spin s = *this;
        s.t = - t;
        return s;
    }
};

inline Spin operator * ( const Spin & s1, const Spin & s2 )
{
    Spin   s  = s1;
    return s *= s2;
}

///////////////////////////////////////////////////////////////////////////////

class Conform
{
public:
    Spin     spin;
    Vector3d trans;
    double   magn;

    Conform () : trans (0.,0.,0.), magn ( 1. ) {}

    explicit Conform ( const Spin & s ) : spin ( s ), trans (0.,0.,0.), magn ( 1.) {}

    Conform ( const Spin & s, const Vector3d & t, double m = 1. ) : spin(s), trans(t), magn(m) {}

    Conform operator~ () const
    {
        const Spin s ( ~spin );
        const double m = 1. / magn;
        return Conform ( s, (-m) * s ( trans ), m );
    }

    Plane operator() ( const Plane & p ) const
    {
        const Vector3d v = spin ( p.norm );
        return Plane ( v, magn * p.dist - v * trans );
    }

    Vector3d operator() ( const Vector3d & v ) const
    {
        return spin ( v ) * magn + trans;
    }

    Conform & operator*= ( const Conform & c )
    {
        trans += spin ( c.trans ) * magn;
        spin  *= c.spin;
        magn  *= c.magn;
        return * this;
    }

};

inline Conform operator * ( const Conform & c1, const Conform & c2 )
{
    Conform c = c1;
    return c *= c2;
}

///////////////////////////////////////////////////////////////////////////////

class Similarity
{
    double   magn;
    Vector3d ox, oy, oz;
    Vector3d ax, ay, az, trans;
public:
    explicit Similarity ( const Spin & spin ) : magn ( 1. ), trans ( 0., 0., 0. )
    {
        spin.getReper ( ox, oy, oz );
        ax = ox;
        ay = oy;
        az = oz;
    }

    explicit Similarity ( const Conform & c ) : magn ( c.magn ), trans ( c.trans )
    {
        c.spin.getReper ( ox, oy, oz );
        ax = ox * magn;
        ay = oy * magn;
        az = oz * magn;
    }

    Plane operator() ( const Plane & p ) const
    {
        const Vector3d v ( ox*p.norm, oy*p.norm, oz*p.norm );
        return Plane ( v, magn * p.dist - v * trans );
    }

    Vector3d operator () ( const Vector3d & p ) const
    {
        return Vector3d ( ax * p + trans.x, ay * p + trans.y, az * p + trans.z );
    }

    double getX ( const Vector3d & p ) const
    {
        return ax * p + trans.x;
    }

    double getY ( const Vector3d & p ) const
    {
        return ay * p + trans.y;
    }

    double getZ ( const Vector3d & p ) const
    {
        return az * p + trans.z;
    }
};

//**************************** 14.07.2008 ****************************
//
//                     �������������� ��������
//
//**************************** 14.07.2008 ****************************

inline Vector3d & operator *= ( Vector3d & v, const Similarity & sim )
{
    return v = sim ( v );
}

inline Vector3d & operator *= ( Vector3d & v, const Conform & conf )
{
    return v = conf ( v );
}

inline Vector3d & operator *= ( Vector3d & v, const Spin & spin )
{
    return v = spin ( v );
}

//**************************** 14.07.2008 ****************************
//
//                     �������������� ���������
//
//**************************** 14.07.2008 ****************************

inline Plane & operator *= ( Plane & plane, const Similarity & sim )
{
    return plane = sim ( plane );
}

inline Plane & operator *= ( Plane & plane, const Conform & conf )
{
    return plane = conf ( plane );
}

inline Plane & operator *= ( Plane & plane, const Spin & spin )
{
    plane.norm = spin ( plane.norm );
    return plane;
}

#endif


