/*
 * Copyright (c) 2009-2018 Ilya Palachev <iliyapalachev@gmail.com>
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
 * @file Recoverer_test.cpp
 * @brief Unit tests for Recoverer of the polyhedron.
 */

#include <cstdlib>
#include "DebugPrint.h"
#include "PolyhedraCorrectionLibrary.h"

int main(int argc, char **argv)
{
    DEBUG_START;
    if (argc != 1)
    {
        fprintf(stderr, "Usage: %s countours_file\n", argv[0]);
        return EXIT_FAILURE;
    }
    char *path = argv[1];
    /* FIXME: Check the existance of the file. */

    /* Create fake empty polyhedron. */
    PolyhedronPtr p(new Polyhedron());

    /* Create shadow contour data and associate p with it. */
    ShadowContourDataPtr SCData(new ShadowContourData(p));

    /* Read shadow contours data from file. */
    SCData->fscanDefault(path);

    DEBUG_END;
    return EXIT_SUCCESS;
}
