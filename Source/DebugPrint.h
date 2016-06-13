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

#ifndef DEBUGPRINT_H
#define DEBUGPRINT_H

#include <stdio.h>

#define OUTPUT
#define GLOBAL_CORRECTION_DERIVATIVE_TESTING

#define COLOUR_NORM "\x1B[0m"
#define COLOUR_RED "\x1B[31m"
#define COLOUR_GREEN "\x1B[32m"
#define COLOUR_YELLOW "\x1B[33m"
#define COLOUR_BLUE "\x1B[34m"
#define COLOUR_MAGENTA "\x1B[35m"
#define COLOUR_CYAN "\x1B[36m"
#define COLOUR_WHITE "\x1B[37m"

#define WHERESTR  "[%s: %d]: "
#define WHEREARG  __PRETTY_FUNCTION__, __LINE__
#define STDERR_PRINT(...) \
	do \
	{ \
		fprintf(stderr, __VA_ARGS__); \
	} \
	while (0)
#define FILE_PRINT(_file, ...) \
	do \
	{ \
		fprintf(_file, ##__VA_ARGS__); \
	} \
	while (0)

#if (!defined NDEBUG && !defined GTEST_BUILD)
#define DEBUG_PRINT(...)  \
	do \
	{ \
		STDERR_PRINT(COLOUR_NORM WHERESTR, WHEREARG); \
		STDERR_PRINT(__VA_ARGS__); \
		STDERR_PRINT("\n"); \
	} \
	while (0)
#define REGULAR_PRINT(_file, ...)  \
	do \
	{ \
		FILE_PRINT(_file, COLOUR_NORM); \
		FILE_PRINT(_file, ##__VA_ARGS__); \
	} \
	while (0)
#define DEBUG_START \
	do \
	{ \
		DEBUG_PRINT("Start"); \
	} \
	while (0)
#define DEBUG_END \
	do \
	{ \
		DEBUG_PRINT("End"); \
	} \
	while (0)

#else
#define DEBUG_PRINT(...)
#define REGULAR_PRINT(...)
#define DEBUG_START
#define DEBUG_END
#endif

#define MAIN_PRINT(...)  \
	do \
	{ \
		STDERR_PRINT(COLOUR_NORM WHERESTR, WHEREARG); \
		STDERR_PRINT(__VA_ARGS__); \
		STDERR_PRINT("\n" COLOUR_NORM); \
	} \
	while (0)

#define ALWAYS_PRINT(_file, ...)  \
	do \
	{ \
		FILE_PRINT(_file, __VA_ARGS__); \
	} \
	while (0)

#define ERROR_PRINT(...)  \
	do \
	{ \
		STDERR_PRINT(COLOUR_RED WHERESTR, WHEREARG); \
		STDERR_PRINT(__VA_ARGS__); \
		STDERR_PRINT("\n" COLOUR_NORM); \
	} \
	while (0)

#define PRINT(...)  \
	do \
	{ \
		STDERR_PRINT(COLOUR_NORM); \
		STDERR_PRINT(__VA_ARGS__); \
		STDERR_PRINT("\n" COLOUR_NORM); \
	} \
	while (0)

#define DEBUG_VARIABLE __attribute__ ((unused))

#endif //DEBUGPRINT_H
