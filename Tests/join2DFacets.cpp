#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <cstdio>

#include <CGAL/Cartesian.h>
typedef CGAL::Cartesian<double> K;
typedef CGAL::Point_2<K> Point_2;
typedef CGAL::Line_2<K> Line_2;
typedef CGAL::Vector_2<K> Vector_2;
#include <CGAL/convex_hull_2.h>
#include <CGAL/intersections.h>

#include "Constants.h"
#include "DebugPrint.h"
#include "DebugAssert.h"
#include "PCLDumper.h"

/**
 * Generates random number d such that |d| <= maxDelta
 *
 * @param maxDelta	maximum absolute limit of generated number
 */
static double genRandomDouble(double maxDelta)
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

#define MAX_DELTA 10.

std::vector<Point_2> generateExtremePoints(int numPoints)
{
	std::vector<Point_2> points;
#if 0
	for (int i = 0; i < numPoints; ++i)
	{
		do
		{
			Point_2 point(genRandomDouble(MAX_DELTA),
					genRandomDouble(MAX_DELTA));
			points.push_back(point);
			std::vector<Point_2> hull;
			convex_hull_2(points.begin(), points.end(),
					std::back_inserter(hull));
			if (hull.size() == points.size())
				break;
			points.pop_back();
		}
		while (1);
	}
#endif
	double maxDelta = 2. / ((double) numPoints);
	for (int i = 0; i < numPoints; ++i)
	{
		double angle = 2. * M_PI * ((double) i) / ((double) numPoints);
		double xShift = genRandomDouble(maxDelta);
		double yShift = genRandomDouble(maxDelta);
		Point_2 point(cos(angle) + xShift, sin(angle) + yShift);
		points.push_back(point);
	}
	std::vector<Point_2> hull;
	convex_hull_2(points.begin(), points.end(), std::back_inserter(hull));
	assert(hull.size() == numPoints);
	std::cout << "Number of points in the hull: " << hull.size() << std::endl;
	return points;
}

void centerizeExtremePoints(std::vector<Point_2> &points)
{
	double centerX = 0., centerY = 0.;
	for (auto point: points)
	{
		centerX += point.x();
		centerY += point.y();
	}
	centerX /= points.size();
	centerY /= points.size();
	Vector_2 center(centerX, centerY);
	for (auto &point: points)
	{
		point = point - center;
	}
}

std::pair<std::vector<Vector_2>, std::vector<double>>
prepareSupportData(std::vector<Point_2> points)
{
	std::vector<Vector_2> directions;
	std::vector<double> values;
	int numPoints = points.size();
	for (int i = 0; i < numPoints; ++i)
	{
		int iNext = (i + 1) % numPoints;
		Line_2 line(points[i], points[iNext]);
		double a = line.a();
		double b = line.b();
		double c = line.c();
		if (a * points[i].x() + b * points[i].y() + c < 0)
		{
			a = -a;
			b = -b;
			c = -c;
		}
		Vector_2 normal(line.a(), line.b());
		double length = sqrt(normal.squared_length());
		Vector_2 direction(normal.x() / length, normal.y() / length);
		directions.push_back(direction);
		double value = -line.c() / length;
		values.push_back(value);
	}
	return std::make_pair(directions, values);
}

int calculateJoinedNumber(std::vector<Point_2> points)
{
	int numJoined = 0;
	do
	{
		Point_2 a = points[0];
		Point_2 b = points[numJoined + 1];
		double area = a.x() * b.y() - a.y() * b.x();
		if (area > 0.)
			break;
		++numJoined;
	} while (true);
	--numJoined;
	numJoined = numJoined * 50 / 100;
	return numJoined;
}

Point_2 calculateTop(Point_2 left, Point_2 first, Point_2 last, Point_2 right)
{
	Line_2 lineLeft(left, first);
	Line_2 lineRight(right, last);

	auto top = CGAL::intersection(lineLeft, lineRight);
	if (top)
		return boost::get<Point_2>(*top);
	else
		exit(EXIT_FAILURE);
}

double calculateFunctional(int numJoined, std::vector<Vector_2> directions,
		std::vector<double> values, Point_2 gridLeft, Point_2 gridRight)
{
	Vector_2 left = gridLeft - CGAL::Origin();
	Vector_2 right = gridRight - CGAL::Origin();
	double functional = 0.;
	for (int i = 1; i <= numJoined; ++i)
	{
		Vector_2 direction = directions[i];
		double value = values[i];
		double difference = std::max(direction * left,
				direction * right) - value;
		functional += difference * difference;
	}
	return functional > 100. ? 100. : functional;
}

int main(int argc, char **argv)
{
	if (argc != 3)
		return EXIT_FAILURE;
	int numPoints = atoi(argv[1]);
	int numGridPoints = atoi(argv[2]);
	std::cout << "Convex hull of " << numPoints << " will be generated"
		<< std::endl;
	if (numPoints <= 0)
		return EXIT_FAILURE;

	auto points = generateExtremePoints(numPoints);
	centerizeExtremePoints(points);

	std::sort(points.begin(), points.end(),
		[](const Point_2 &a, const Point_2 &b)
		{
			return atan2(a.y(), a.x()) > atan2(b.y(), b.x());
		});
	std::vector<Vector_2> directions;
	std::vector<double> values;
	std::tie(directions, values) = prepareSupportData(points);

	int numJoined = calculateJoinedNumber(points);
	std::cout << "Joined facets number: " << numJoined << std::endl;

	Point_2 left = points[0];
	Point_2 first = points[1];
	Point_2 last = points[numJoined];
	Point_2 right = points[numJoined + 1];
	
	Point_2 top = calculateTop(left, first, last, right);
	Vector_2 sideLeft = top - first;
	double stepLeft = sqrt(sideLeft.squared_length()) / numGridPoints;
	Vector_2 sideRight = top - last;
	double stepRight = sqrt(sideRight.squared_length()) / numGridPoints;

	std::ofstream result;
	result.open("result.csv");
	result << "x coord,y coord,z coord,scalar" << std::endl;
	Point_2 gridLeft = first;
	double x = 0.;
	for (int i = 0; i < numGridPoints; ++i)
	{
		Point_2 gridRight = last;
		double y = 0.;
		for (int j = 0; j < numGridPoints; ++j)
		{
			double functional = calculateFunctional(numJoined,
					directions, values,
					gridLeft, gridRight);
			if (functional <= 100.)			
				result << x << "," << y << ","
					<< functional << ","
					<< 0 << std::endl;
			gridRight = gridRight + sideRight * stepRight;
			y += stepRight;
		}
		gridLeft = gridLeft + sideLeft * stepLeft;
		x += stepLeft;
	}
	result.close();

	std::ofstream outputFile;
	outputFile.open("join2Dfacets-initial.txt");
	for (int i = 0; i < numJoined + 1; ++i)
		outputFile << points[i] << std::endl;
	outputFile << top << std::endl;
	outputFile.close();


	return 0;
}
