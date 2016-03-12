/*
 * Copyright (c) 2009-2015 Ilya Palachev <iliyapalachev@gmail.com>
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
 * @file Common.h
 * @brief Common functions used in PCL (implementation)
 */

#include <cstdlib>
#include <sys/time.h>
#include "DebugPrint.h"

bool tryGetenvDouble(const char *envName, double &value)
{
	DEBUG_START;
	char *mistake = NULL;
	char *string = getenv(envName);
	if (string)
	{
		double valueNew = strtod(string, &mistake);
		if (mistake && *mistake)
		{
			DEBUG_END;
			return false;
		}
		else
		{
			value = valueNew;
			DEBUG_END;
			return true;
		}
	}
	DEBUG_END;
	return false;
}

double genRandomDouble(double maxDelta)
{
	DEBUG_START;
	//srand((unsigned) time(0));
	struct timeval t1;
	gettimeofday(&t1, NULL);
	srand(t1.tv_usec * t1.tv_sec);
	
	int randomInteger = rand();
	double randomDouble = randomInteger;
	const double halfRandMax = RAND_MAX * 0.5;
	randomDouble -= halfRandMax;
	randomDouble /= halfRandMax;

	randomDouble *= maxDelta;

	DEBUG_END;
	return randomDouble;
}

