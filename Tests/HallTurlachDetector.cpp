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
#include "PolyhedraCorrectionLibrary.h"

int main(int argc, char **argv)
{
    DEBUG_START;
    /* Parse command line. */
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s countours_file z_value\n", argv[0]);
        return EXIT_FAILURE;
    }
    char *path = argv[1];
    char *mistake = NULL;
    double zValue = strtod(argv[2], &mistake);
    if (mistake && *mistake)
    {
        fprintf(stderr, "Error while reading z_value = %s\n", argv[2]);
        return EXIT_FAILURE;
    }
    /* FIXME: Check the existance of the file. */

    /* Create fake empty polyhedron. */
    PolyhedronPtr p(new Polyhedron());

    /* Create shadow contour data and associate p with it. */
    ShadowContourDataPtr SCData(new ShadowContourData(p));

    /* Read shadow contours data from file. */
    SCData->fscanDefault(path);

    int numContoursIntersecting = 0;
    std::vector<std::pair<double, double>> supportItems;
    for (int iContour = 0; iContour < SCData->numContours; ++iContour)
    {
        SContour &contour = SCData->contours[iContour];
        int numIntersecting = 0;
        for (int iSide = 0; iSide < contour.ns; ++iSide)
        {
            SideOfContour &side = contour.sides[iSide];
            double diff1 = side.A1.z - zValue;
            double diff2 = side.A2.z - zValue;
            if (diff1 * diff2 >= 0.)
                continue;

            ++numIntersecting;

            /* Find t, for which A^intersect = t * A1 + (1-t) * A2. */
            /* FIXME: Prevent zero division. */
            double t = (zValue - side.A2.z) / (side.A1.z - side.A2.z);

            /* Find z and y coordinates of A^intersect. */
            double x = t * side.A1.x + (1 - t) * side.A2.x;
            double y = t * side.A1.y + (1 - t) * side.A2.y;

            /* Support angle and support value. */
            double theta = atan2(y, x);
            double h = sqrt(x * x + y * y);
            supportItems.push_back(std::make_pair(theta, h));

        }
        if (numIntersecting > 0)
            ++numContoursIntersecting;
    }
    fprintf(stdout, "Number of contours intersecting the z value "
            "barrier: %d\n", numContoursIntersecting);

    if (numContoursIntersecting < SCData->numContours)
    {
        fprintf(stderr, "Error: Not all contours intersect plane {z = %lf}\n",
                zValue);
        return EXIT_FAILURE;
    }

    std::sort(supportItems.begin(), supportItems.end());
    fprintf(stdout, "Support items:\n");
    for (unsigned i = 0; i < supportItems.size(); ++i)
    {
        fprintf(stdout, "  angle %.16lf : value %lf\n", supportItems[i].first,
                supportItems[i].second);
    }

    DEBUG_END;
    return EXIT_SUCCESS;
}
