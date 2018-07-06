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
 * @file HallTurlachDetector.cpp
 * @brief Detection of corners based on Hall-Turlach algorithm.
 * Article URL: https://ieeexplore.ieee.org/abstract/document/754588/
 */

#include <cstdlib>
#include "PolyhedraCorrectionLibrary.h"

typedef std::vector<std::pair<double, double>> ItemsVector;

static int getModulo(int value, int mod)
{
    return ((value % mod) + mod) % mod;
}

static double getTheta(int i, const ItemsVector &items)
{
    return items[getModulo(i, items.size())].first;
}

static double getY(int i, const ItemsVector &items)
{
    return items[getModulo(i, items.size())].second;
}

static double getSine(int i, int j, const ItemsVector &items)
{
    return sin(getTheta(i, items) - getTheta(j, items));
}

static double getC(int i, int j, int m, const ItemsVector &items,
                   bool positive)
{
    ASSERT(m >= 0);
    ASSERT(unsigned(m) < items.size());

    if (positive && i <= j && j <= i + m - 1)
        return getSine(j + m, j + 2 * m, items);
    else if (positive && i + m <= j && j <= i + 2 * m - 1)
        return getSine(j + m, j - m, items);
    else if (positive && i + 2 * m <= j && j <= i + 3 * m)
        return getSine(j - 2 * m, j - m, items);
    else if (!positive && i - 3 * m + 1 <= j && j <= i - 2 * m)
        return getSine(j + m, j + 2 * m, items);
    else if (!positive && i - 2 * m + 1 <= j && j <= i - m)
        return getSine(j + m, j - m, items);
    else if (!positive && i - m + 1 <= j && j <= i)
        return getSine(j - 2 * m, j - m, items);

    fprintf(stderr, "i = %d, j = %d, m = %d, positive = %d\n", i, j, m,
            positive);
    ASSERT(0 && "Impossible happened");
    return 0.;
}

static double getDelta1(int i, int m, const ItemsVector &items, bool positive)
{
    double sum = 0.;
    int lower = positive ? i : i - 3 * m + 1;
    int upper = positive ? i + 3 * m - 1 : i;
    for (int j = lower; j <= upper; ++j)
        sum += getY(j, items) * getC(i, j, m, items, positive);
    return sum;
}

static double getTau(int i, int m, const ItemsVector &items, bool positive)
{
    double sum = 0.;
    int lower = positive ? i : i - 3 * m + 1;
    int upper = positive ? i + 3 * m - 1 : i;
    for (int j = lower; j <= upper; ++j)
    {
        double c = getC(i, j, m, items, positive);
        sum += c * c;
    }
    return sqrt(sum);
}

static double getDelta(int i, int m, const ItemsVector &items, bool positive,
                       double sigma)
{
    double denominator = sigma * getTau(i, m, items, positive);
    return getDelta1(i, m, items, positive) / denominator;
}

static double getDerivativeEstimate(int i, int l, const ItemsVector &items)
{
    double cosJI = cos(getTheta(i - l, items) - getTheta(i, items));
    double cosKI = cos(getTheta(i + l, items) - getTheta(i, items));
    double enumerator = getY(i + l, items) * cosJI - getY(i - l, items) * cosKI;
    double denominator = getSine(i + l, i - l, items);
    return enumerator / denominator;
}

static double getXEstimate(int i, int l, const ItemsVector &items)
{
    double estimate = getY(i, items) * cos(getTheta(i, items));
    estimate -= getDerivativeEstimate(i, l, items) * sin(getTheta(i, items));
    return estimate;
}

static double getYEstimate(int i, int l, const ItemsVector &items)
{
    double estimate = getY(i, items) * sin(getTheta(i, items));
    estimate += getDerivativeEstimate(i, l, items) * cos(getTheta(i, items));
    return estimate;
}

static double getXEstimateAverage(int l, double lower, double upper,
                                  const ItemsVector &items)
{
    int N = 0;
    double average = 0.;
    for (unsigned i = 0; i < items.size(); ++i)
    {
        double theta = getTheta(i, items);
        if (theta > lower && theta < upper)
	{
            ++N;
	    average += getXEstimate(i, l, items);
	}
    }
    average /= N;
    return average;
}

static double getYEstimateAverage(int l, double lower, double upper,
                                  const ItemsVector &items)
{
    int N = 0;
    double average = 0.;
    for (unsigned i = 0; i < items.size(); ++i)
    {
        double theta = getTheta(i, items);
        if (theta > lower && theta < upper)
	{
            ++N;
	    average += getYEstimate(i, l, items);
	}
    }
    average /= N;
    return average;
}

static std::set<double> indicesToThetas(const std::set<unsigned> &cluster,
                                        const ItemsVector &items)
{
    std::set<double> thetas;
    for (unsigned i : cluster)
        thetas.insert(getTheta(i, items));
    return thetas;
}

int main(int argc, char **argv)
{
    DEBUG_START;
    /* Parse command line. */
    if (argc != 7)
    {
        fprintf(stderr, "Usage: %s countours_file z m t l q\n",
                argv[0]);
        return EXIT_FAILURE;
    }

    /* Read the file path to contours. */
    /* FIXME: Check the existance of the file. */
    char *path = argv[1];

    /* Read the "z" value, i.e. the coordinate of plane on Oz axis. */
    char *mistake = NULL;
    double zValue = strtod(argv[2], &mistake);
    if (mistake && *mistake)
    {
        fprintf(stderr, "Error while reading z = %s\n", argv[2]);
        return EXIT_FAILURE;
    }

    /* Read the "m" value, i.e. p'' + p averaging parameter. */
    int mValue = atoi(argv[3]);

    /* Read the "t" value, i.e. the upper bound for small numbers. */
    double tValue = strtod(argv[4], &mistake);
    if (mistake && *mistake)
    {
        fprintf(stderr, "Error while reading t = %s\n", argv[4]);
        return EXIT_FAILURE;
    }

    /* Read the "l" value, i.e. step for p' estimate calculation. */
    int lValue = atoi(argv[5]);

    /* Read the "q" value, i.e. the final estimate averaging parameter. */
    double qValue = strtod(argv[6], &mistake);
    if (mistake && *mistake)
    {
        fprintf(stderr, "Error while reading q = %s\n", argv[6]);
        return EXIT_FAILURE;
    }

    /* Create fake empty polyhedron. */
    PolyhedronPtr p(new Polyhedron());

    /* Create shadow contour data and associate p with it. */
    ShadowContourDataPtr SCData(new ShadowContourData(p));

    /* Read shadow contours data from file. */
    SCData->fscanDefault(path);

    int numContoursIntersecting = 0;
    ItemsVector items;
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
            items.push_back(std::make_pair(theta, h));

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

    std::sort(items.begin(), items.end());
    fprintf(stdout, "Support items:\n");
    double sum = 0.;
    for (unsigned i = 0; i < items.size(); ++i)
    {
        unsigned iPrev = getModulo(i - 1, items.size());
        double diff = items[i].second - items[iPrev].second;
        sum += diff * diff;
    }
    double variance = sum / (2. * items.size());
    fprintf(stdout, "Variance estimate: %.16lf\n", variance);
    double sigma = sqrt(variance);
    fprintf(stdout, "Sigma: %.16lf\n", sigma);

    std::vector<std::set<unsigned>> clusters(items.size());
    unsigned iCluster = 0;
    for (unsigned i = 0; i < items.size(); ++i)
    {
        double deltaPlus = getDelta(i, mValue, items, true, sigma);
        double deltaMinus = getDelta(i, mValue, items, false, sigma);
        fprintf(stdout, "Delta #%d: (+): %lf (-): %lf", i, deltaPlus,
                deltaMinus);
        if (fabs(deltaPlus) < tValue || fabs(deltaMinus) < tValue)
        {
            fprintf(stdout, "  LOW !");
            clusters[iCluster].insert(i);
        }
        else if (clusters[iCluster].size() > 0)
            ++iCluster;
        fprintf(stdout, "\n");
    }

    if (iCluster > 0 && clusters[iCluster].size() == 0)
        --iCluster;

    if (iCluster >= 1)
    {
        fprintf(stdout, "First and last clusters - checking...\n");
        unsigned iFirst = *(clusters[0].begin());
        unsigned iLast = *(--clusters[iCluster].end());
        if (iFirst == 0 && iLast == items.size() - 1)
        {
            clusters[0].insert(clusters[iCluster].begin(),
                               clusters[iCluster].end());
            clusters[iCluster].clear();
        }
        fprintf(stdout, "First and last clusters has been united.\n");
    }

    unsigned numClusters = iCluster;
    if (clusters[iCluster].size() > 0)
        ++numClusters;

    fprintf(stdout, "Number of clusters: %d\n", numClusters);

    if (numClusters > 0)
    {
        fprintf(stdout, "Clusters:\n");
        for (iCluster = 0; iCluster < items.size(); ++iCluster)
        {
            const std::set<unsigned> &cluster = clusters[iCluster];
            if (cluster.size() == 0)
                continue;

            fprintf(stdout, "  cluster #%d (size %lu): ", iCluster,
                    cluster.size());
            for (unsigned i : cluster)
                fprintf(stdout, " %d", i);
            fprintf(stdout, "\n");
	    if (cluster.find(0) != cluster.end())
	    {
                fprintf(stdout, "    PI clusters not implemented.\n");
		continue;
	    }
	    auto thetas = indicesToThetas(cluster, items);
	    double thetaMin = *(thetas.begin());
	    double thetaMax = *(--thetas.end());
	    fprintf(stdout, "    min = %lf, max = %lf\n", thetaMin, thetaMax);
	    double omega = (thetaMin + thetaMax) / 2.;
	    double delta = (thetaMax - thetaMin) / 2.;
	    double lower = omega - qValue * delta;
	    double upper = omega + qValue * delta;
	    double x = getXEstimateAverage(lValue, lower, upper, items);
	    double y = getYEstimateAverage(lValue, lower, upper, items);
	    fprintf(stdout, "    x = %lf, y = %lf\n", x, y);
        }
    }
    
    DEBUG_END;
    return EXIT_SUCCESS;
}
