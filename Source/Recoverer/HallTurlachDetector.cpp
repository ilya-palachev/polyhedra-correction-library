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

#include "Recoverer/HallTurlachDetector.h"

typedef std::vector<std::pair<double, double>> ItemsVector;
typedef std::vector<std::set<unsigned>> ClustersVector;

static int getModulo(int value, int mod) { return ((value % mod) + mod) % mod; }

static double getTheta(int i, const ItemsVector &items) {
  return items[getModulo(i, items.size())].first;
}

static double getY(int i, const ItemsVector &items) {
  return items[getModulo(i, items.size())].second;
}

static double getSine(int i, int j, const ItemsVector &items) {
  return sin(getTheta(i, items) - getTheta(j, items));
}

static double getC(int i, int j, int m, const ItemsVector &items,
                   bool positive) {
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

  fprintf(stderr, "i = %d, j = %d, m = %d, positive = %d\n", i, j, m, positive);
  ASSERT(0 && "Impossible happened");
  return 0.;
}

static double getDelta1(int i, int m, const ItemsVector &items, bool positive) {
  double sum = 0.;
  int lower = positive ? i : i - 3 * m + 1;
  int upper = positive ? i + 3 * m - 1 : i;
  for (int j = lower; j <= upper; ++j)
    sum += getY(j, items) * getC(i, j, m, items, positive);
  return sum;
}

static double getTau(int i, int m, const ItemsVector &items, bool positive) {
  double sum = 0.;
  int lower = positive ? i : i - 3 * m + 1;
  int upper = positive ? i + 3 * m - 1 : i;
  for (int j = lower; j <= upper; ++j) {
    double c = getC(i, j, m, items, positive);
    sum += c * c;
  }
  return sqrt(sum);
}

static double getDelta(int i, int m, const ItemsVector &items, bool positive,
                       double sigma) {
  double denominator = sigma * getTau(i, m, items, positive);
  return getDelta1(i, m, items, positive) / denominator;
}

static double getDerivativeEstimate(int i, int l, const ItemsVector &items) {
  double cosJI = cos(getTheta(i - l, items) - getTheta(i, items));
  double cosKI = cos(getTheta(i + l, items) - getTheta(i, items));
  double enumerator = getY(i + l, items) * cosJI - getY(i - l, items) * cosKI;
  double denominator = getSine(i + l, i - l, items);
  return enumerator / denominator;
}

static double getXEstimate(int i, int l, const ItemsVector &items) {
  double estimate = getY(i, items) * cos(getTheta(i, items));
  estimate -= getDerivativeEstimate(i, l, items) * sin(getTheta(i, items));
  return estimate;
}

static double getYEstimate(int i, int l, const ItemsVector &items) {
  double estimate = getY(i, items) * sin(getTheta(i, items));
  estimate += getDerivativeEstimate(i, l, items) * cos(getTheta(i, items));
  return estimate;
}

static double getXEstimateAverage(int l, double lower, double upper,
                                  const ItemsVector &items, bool piCase) {
  int N = 0;
  double average = 0.;
  for (unsigned i = 0; i < items.size(); ++i) {
    double theta = getTheta(i, items);
    if ((!piCase && theta > lower && theta < upper) ||
        (piCase && (theta > lower || theta < upper))) {
      ++N;
      average += getXEstimate(i, l, items);
    }
  }
  average /= N;
  return average;
}

static double getYEstimateAverage(int l, double lower, double upper,
                                  const ItemsVector &items, bool piCase) {
  int N = 0;
  double average = 0.;
  for (unsigned i = 0; i < items.size(); ++i) {
    double theta = getTheta(i, items);
    if ((!piCase && theta > lower && theta < upper) ||
        (piCase && (theta > lower || theta < upper))) {
      ++N;
      average += getYEstimate(i, l, items);
    }
  }
  average /= N;
  return average;
}

static std::set<double> indicesToThetas(const std::set<unsigned> &cluster,
                                        const ItemsVector &items) {
  std::set<double> thetas;
  for (unsigned i : cluster)
    thetas.insert(getTheta(i, items));
  return thetas;
}

static double getSigma(const ItemsVector &items) {
  fprintf(stdout, "Support items:\n");
  double sum = 0.;
  for (unsigned i = 0; i < items.size(); ++i) {
    unsigned iPrev = getModulo(i - 1, items.size());
    double diff = items[i].second - items[iPrev].second;
    sum += diff * diff;
  }
  double variance = sum / (2. * items.size());
  fprintf(stdout, "Variance estimate: %.16lf\n", variance);
  double sigma = sqrt(variance);
  fprintf(stdout, "Sigma: %.16lf\n", sigma);

  return sigma;
}

static ClustersVector getClusters(int m, double t, const ItemsVector &items,
                                  bool reverse) {
  double sigma = getSigma(items);
  ClustersVector clusters(items.size());
  unsigned iCluster = 0;
  for (unsigned i = 0; i < items.size(); ++i) {
    double deltaPlus = getDelta(i, m, items, true, sigma);
    double deltaMinus = getDelta(i, m, items, false, sigma);
    fprintf(stdout, "Delta #%d: (+): %lf (-): %lf", i, deltaPlus, deltaMinus);
    if (!reverse && (fabs(deltaPlus) < t || fabs(deltaMinus) < t)) {
      fprintf(stdout, "  LOW !");
      clusters[iCluster].insert(i);
    } else if (reverse && (fabs(deltaPlus) >= t || fabs(deltaMinus) >= t)) {
      fprintf(stdout, "  HIGH !");
      clusters[iCluster].insert(i);
    } else if (clusters[iCluster].size() > 0)
      ++iCluster;
    fprintf(stdout, "\n");
  }

  if (iCluster > 0 && clusters[iCluster].size() == 0)
    --iCluster;

  if (iCluster >= 1) {
    fprintf(stdout, "First and last clusters - checking...\n");
    unsigned iFirst = *(clusters[0].begin());
    unsigned iLast = *(--clusters[iCluster].end());
    if (iFirst == 0 && iLast == items.size() - 1) {
      clusters[0].insert(clusters[iCluster].begin(), clusters[iCluster].end());
      clusters[iCluster].clear();
    }
    fprintf(stdout, "First and last clusters has been united.\n");
  }

  while (clusters.back().empty())
    clusters.pop_back();
  fprintf(stdout, "Number of clusters: %lu\n", clusters.size());

  return clusters;
}

static std::pair<double, double>
getMinMaxTheta(const std::set<unsigned> &cluster, const ItemsVector &items,
               bool piCase) {
  double thetaMin = 0.;
  double thetaMax = 0.;
  std::set<double> thetas = indicesToThetas(cluster, items);
  if (piCase) {
    // Corner interval contains PI
    fprintf(stdout, "Handling PI cluster\n");
    std::set<double> positive;
    for (double theta : thetas) {
      if (theta < 0.)
        theta += 2. * M_PI;
      positive.insert(theta);
    }
    thetaMin = *(positive.begin());
    thetaMax = *(--positive.end()) - 2. * M_PI;
  } else {
    thetaMin = *(thetas.begin());
    thetaMax = *(--thetas.end());
  }

  return std::make_pair(thetaMin, thetaMax);
}

std::vector<std::pair<double, double>>
estimateCorners(ItemsVector items, unsigned mParameter, double tParameter,
                int lParameter, double qParameter, double sParameter,
                bool reverse) {
  std::sort(items.begin(), items.end());
  auto clusters = getClusters(mParameter, tParameter, items, reverse);

  std::vector<std::pair<double, double>> corners;
  
  if (clusters.empty())
    return corners;

  fprintf(stdout, "Clusters:\n");
  unsigned iCluster = 0;
  for (const auto &cluster : clusters) {
    fprintf(stdout, "  cluster #%d (size %lu): ", iCluster, cluster.size());
    for (unsigned i : cluster)
      fprintf(stdout, " %d", i);
    fprintf(stdout, "\n");
    if (cluster.size() < sParameter) {
      fprintf(stdout, "  skipped\n");
      continue;
    }
    bool piCase = cluster.find(0) != cluster.end() &&
                  cluster.find(items.size() - 1) != cluster.end();
    auto pair = getMinMaxTheta(cluster, items, piCase);
    double thetaMin = pair.first;
    double thetaMax = pair.second;
    fprintf(stdout, "    min = %lf, max = %lf\n", thetaMin, thetaMax);
    double omega = (thetaMin + thetaMax) / 2.;
    double delta = (thetaMax - thetaMin) / 2.;
    double lower = omega - qParameter * delta;
    double upper = omega + qParameter * delta;
    double x = getXEstimateAverage(lParameter, lower, upper, items, piCase);
    double y = getYEstimateAverage(lParameter, lower, upper, items, piCase);
    fprintf(stdout, "    x = %lf, y = %lf\n", x, y);
    double theta = atan2(y, x);
    fprintf(stdout, "    theta = %lf\n", theta);
    corners.push_back(std::make_pair(x, y));
    ++iCluster;
  }
  return corners;
}
