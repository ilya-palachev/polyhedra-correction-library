#ifndef MYPOINTC2_IOSTREAM_H
#define MYPOINTC2_IOSTREAM_H

std::ostream &
operator<<(std::ostream &os, const PCLPoint_2 &p)
{
	switch(os.iword(CGAL::IO::mode)) {
	case CGAL::IO::ASCII :
		return os << p.x() << ' ' << p.y() << ' ' << p.id();
	case CGAL::IO::BINARY :
		CGAL::write(os, p.x());
		CGAL::write(os, p.y());
		CGAL::write(os, p.id());
		return os;
	default:
		return os << "PCLPoint_2(" << p.x() << ", " << p.y() << ", " << p.id() << ')';
	}
}



std::istream &
operator>>(std::istream &is, PCLPoint_2 &p)
{
	double x, y;
	int id;
	switch(is.iword(CGAL::IO::mode)) {
	case CGAL::IO::ASCII :
		is >> x >> y >> id;
		break;
	case CGAL::IO::BINARY :
		CGAL::read(is, x);
		CGAL::read(is, y);
		CGAL::read(is, id);
		break;
	default:
		std::cerr << "" << std::endl;
		std::cerr << "Stream must be in ascii or binary mode" << std::endl;
		break;
	}
	if (is) {
		p = PCLPoint_2(x, y, id);
	}
	return is;
}
#endif //MYPOINTC2_IOSTREAM_H
