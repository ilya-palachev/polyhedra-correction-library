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
 * @file GlpProbWrappers.h
 * @brief Wrappers around GLPK problem that allow to stream them to global
 * PCL dumper.
 */

#ifndef GLPPROBWRAPPERS_H_
#define GLPPROBWRAPPERS_H_

struct glp_prob_wrapper
{
	glp_prob *problem;

	glp_prob_wrapper(glp_prob *p) : problem(p)
	{
	}

	friend std::ostream &operator<<(std::ostream &stream, glp_prob_wrapper &p)
	{
		DEBUG_START;
		char *name = tmpnam(NULL);
		glp_write_lp(p.problem, NULL, name);
		std::ifstream tmpstream;
		tmpstream.open(name, std::ifstream::in);
		stream << tmpstream.rdbuf();
		tmpstream.close();
		DEBUG_END;
		return stream;
	}
};

struct glp_prob_wrapper_mps
{
	glp_prob *problem;

	glp_prob_wrapper_mps(glp_prob *p) : problem(p)
	{
	}

	friend std::ostream &operator<<(std::ostream &stream,
									glp_prob_wrapper_mps &p)
	{
		DEBUG_START;
		char *name = tmpnam(NULL);
		glp_write_mps(p.problem, GLP_MPS_FILE, NULL, name);
		std::ifstream tmpstream;
		tmpstream.open(name, std::ifstream::in);
		stream << tmpstream.rdbuf();
		tmpstream.close();
		DEBUG_END;
		return stream;
	}
};

#endif /* GLPPROBWRAPPERS_H_ */
