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

/**
 * @file FaceIndexed.h
 * @brief Extension of CGAL face with added index.
 */

#ifndef FACEINDEXED_H_
#define FACEINDEXED_H_

/** A face type with an ID member variable. */
template <class Refs, class Plane>
class FaceIndexed : public CGAL::HalfedgeDS_face_base<Refs, CGAL::Tag_true,
	Plane>
{
public:
	long int id;

	FaceIndexed() :
		CGAL::HalfedgeDS_face_base<Refs, CGAL::Tag_true, Plane>(),
		id(-1) {}

	FaceIndexed(const Plane_3& plane) :
		CGAL::HalfedgeDS_face_base<Refs, CGAL::Tag_true, Plane>(plane),
		id(-1) {}
};

#endif /* FACEINDEXED_H_ */
