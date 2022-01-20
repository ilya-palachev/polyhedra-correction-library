/*
 * Copyright (c) 2009-2016 Ilya Palachev <iliyapalachev@gmail.com>
 *
 * This file is part of Polyhedra Correction Library.
 *
 * Polyhedra Correction Library is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Polyhedra Correction Library is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Polyhedra Correction Library.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include "Recoverer/TangientPointInformation.h"

#define NOT_INITIALIZED

TangientPointInformation::TangientPointInformation() :
	iPoint_(NOT_INITIALIZED),
	point_(),
	iDirection_(NOT_INITIALIZED),
	direction_(),
	indices_(),
	planes_(),
	supportValue_(0.),
	inverse_()
{
	DEBUG_START;
	DEBUG_END;
}

TangientPointInformation::~TangientPointInformation()
{
	DEBUG_START;
	DEBUG_END;
}

Eigen::Matrix3d TangientPointInformation::calculateInverse(void)
{
	DEBUG_START;
	Eigen::Matrix3d fromDirections;
	for (int i = 0; i < 3; ++i)
	{
		fromDirections(i, 0) = planes_[i].a();
		fromDirections(i, 1) = planes_[i].b();
		fromDirections(i, 2) = planes_[i].c();
	}

	Eigen::Matrix3d inverse = fromDirections.inverse();
	DEBUG_END;
	return inverse;
}

TangientPointInformation::TangientPointInformation(int iDirection, Vector_3 direction,
												   Polyhedron_3::Vertex_iterator vertex)
{
	DEBUG_START;
	iPoint_ = vertex->id;
	iDirection_ = iDirection;
	point_ = vertex->point() - CGAL::ORIGIN;
	direction_ = direction;
	int iHalfedge = 0;
	auto halfedge = vertex->vertex_begin();
	auto halfedgeFirst = halfedge;
	do
	{
		auto facet = halfedge->facet();
		indices_[iHalfedge] = facet->id;
		planes_[iHalfedge] = facet->plane();
		++halfedge;
		++iHalfedge;
	} while (halfedge != halfedgeFirst);
#ifndef NDEBUG
	std::cerr << "... The best is #" << vertex->id << " : " << *this << std::endl;
#endif
	supportValue_ = direction * (vertex->point() - CGAL::Origin());
	inverse_ = calculateInverse();
	DEBUG_END;
}

std::ostream &operator<<(std::ostream &stream, TangientPointInformation &info)
{
	DEBUG_START;
	stream << "information:" << std::endl;
	stream << "   tangient point is #" << info.iPoint_ << ": " << info.point_ << std::endl;
	stream << "   support direction is #" << info.iDirection_ << ": " << info.direction_ << std::endl;
	stream << "   facets:" << std::endl;
	for (int i = 0; i < 3; ++i)
	{
		stream << "      " << info.indices_[i] << ": " << info.planes_[i] << std::endl;
	}
	DEBUG_END;
	return stream;
}

GradientSlice *TangientPointInformation::calculateFirstDerivative(void)
{
	DEBUG_START;

	double products[3];
	for (int i = 0; i < 3; ++i)
	{
		Vector_3 column = Vector_3(inverse_(0, i), inverse_(1, i), inverse_(2, i));
		products[i] = direction_ * column;
	}

	GradientSlice *result = new GradientSlice[3];
	for (int i = 0; i < 3; ++i)
	{
		result[i].id_ = indices_[i];
		result[i].values_[0] = -point_.x() * products[i];
		result[i].values_[1] = -point_.y() * products[i];
		result[i].values_[2] = -point_.z() * products[i];
		result[i].values_[3] = products[i];
	}

	DEBUG_END;
	return result;
}

HessianSlice *TangientPointInformation::calculateSecondDerivative(void)
{
	DEBUG_START;

	/* FIXME: make this part of code static. */
	Eigen::Matrix3d **E = new Eigen::Matrix3d *[3];
	for (int i = 0; i < 3; ++i)
	{
		E[i] = new Eigen::Matrix3d[3];
		for (int j = 0; j < 3; ++j)
		{
			for (int k = 0; k < 3; ++k)
				for (int l = 0; l < 3; ++l)
					E[i][j](k, l) = 0.;
			E[i][j](i, j) = 1.;
		}
	}

	Eigen::Vector3d *e = new Eigen::Vector3d[3];
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			e[i](j) = 0.;
		}
		e[i](i) = 1.;
	}

	HessianSlice *result = new HessianSlice[9];

	Eigen::Vector3d tangient;
	tangient(0) = point_.x();
	tangient(1) = point_.y();
	tangient(2) = point_.z();
	Eigen::Matrix3d matrix;
	for (int s = 0; s < 3; ++s)
	{
		for (int t = 0; t < 3; ++t)
		{
			auto slice = &result[3 * s + t];
			slice->idRow_ = s;
			slice->idColumn_ = t;
			for (int i = 0; i < 3; ++i)
			{
				for (int j = 0; j < 3; ++j)
				{
					matrix = E[s][i] * inverse_ * E[t][j] + E[t][j] * inverse_ * E[s][i];
					matrix = inverse_ * matrix;
					auto v = matrix * tangient;
					double value = -direction_ * Vector_3(v(0), v(1), v(2));
					slice->values_(i, j) = value;
				}
				matrix = inverse_ * E[s][i] * inverse_;
				auto v = matrix * e[t];
				double value = direction_ * Vector_3(v(0), v(1), v(2));
				slice->values_(i, 3) = value;
			}
			slice->values_(3, 3) = 0.;
		}
	}

	DEBUG_END;
	return result;
}
