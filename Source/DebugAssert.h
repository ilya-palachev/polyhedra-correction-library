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
 * @file DebugAssert.h
 * @brief Macros for debug and testing assertions.
 */

#ifndef DEBUGASSERT_H_
#define DEBUGASSERT_H_

#include <cassert>
#include <iostream>

/*
 * Define 2 implementations of macros ASSERT and ASSERT_PRINT: for release
 * build and for debug build.
 *
 * If file is compiler with macro NDEBUG enabled, then all assertions in it will
 * become dummy and will be cut by compiler at compile time.
 */
#ifndef NO_ASSERTS

/* Implementation for debug build. */

/**
 * Assertion, similar to standard Linux assertion.
 */
#define ASSERT(expression)                                                                                             \
	do                                                                                                                 \
	{                                                                                                                  \
		if (!(expression))                                                                                             \
		{                                                                                                              \
			std::cerr << "Assertion failed!" << std::endl                                                              \
					  << "[assertion expression:] " #expression << std::endl                                           \
					  << "[assertion location:] " << std::endl                                                         \
					  << "       file: " << __FILE__ << std::endl                                                      \
					  << "   function: " << __PRETTY_FUNCTION__ << std::endl                                           \
					  << "       line: " << __LINE__ << std::endl;                                                     \
			abort();                                                                                                   \
		}                                                                                                              \
	} while (0) /* User adds ";" */

/**
 * Prints error and adds assertion (in the way it was done before).
 * TODO: Avoid using macro ERROR_PRINT here, after we port all debug logging to
 * glog.
 */
#define ASSERT_PRINT(condition, ...)                                                                                   \
	if (!(condition))                                                                                                  \
	{                                                                                                                  \
		ERROR_PRINT(__VA_ARGS__);                                                                                      \
	}                                                                                                                  \
	ASSERT(condition) /* User adds ";" */

#else /* NDEBUG */

/* Implementation for release build. */

/**
 * Dummy assertion, that does not perform any assertion at all, it should be cut
 * away by compiler at compile time for the release build.
 */
#define ASSERT(x)

/**
 * Dummy printing assertion, that does not perform any assertion at all, it
 * should be cut away by compiler at compile time for release build, except that
 * ERROR_PRINT macro should stay.
 * TODO: Avoid using macro ERROR_PRINT here, after we port all debug logging to
 * glog.
 */
#define ASSERT_PRINT(condition, ...)                                                                                   \
	do                                                                                                                 \
	{                                                                                                                  \
		if (!(condition))                                                                                              \
		{                                                                                                              \
			ERROR_PRINT(__VA_ARGS__);                                                                                  \
		}                                                                                                              \
	} while (0) /* User adds ";" */

#endif /* NDEBUG */

#endif /* DEBUGASSERT_H_ */
