#include <CGAL/Origin.h>
#include <CGAL/convex_hull_2.h>
#include <CGAL/basic.h>
#include <CGAL/Filtered_kernel.h>
#include "PCLKernel/PCLKernel.h"
typedef PCLKernel<double> PCL_K;
typedef CGAL::Filtered_kernel_adaptor<PCL_K> LocalSContourK;
typedef LocalSContourK::Point_2 Point_2;

#define NUM_ARGUMENTS_DEFAULT 2

/**
 * Prints the usage information for the program.
 *
 * @param argv	Program arguments.
 */
void printUsage(const char *path)
{
	std::cerr << "Usage: " << path << " <number of support planes>"
		<< std::endl;
}

void errorExit(const char *path, const char *message)
{
	std::cerr << "Error: " << message << std::endl;
	printUsage(path);
	exit(EXIT_FAILURE);
}

long scanInteger(const char *path, const char *string)
{
	char *end;
	errno = 0;
	long value = strtol(string, &end, 0);
	if ((errno == ERANGE && (value == LONG_MIN || value == LONG_MAX))
			|| (errno != 0 && value == 0))
	{
		perror("strtol");
		errorExit(path, "Invalid number.");
	}

	if (end == string)
	{
		errorExit(path, "Invalid number: no digits were found.");
	}

	return value;
}

int main(int argc, char **argv)
{
	if (argc != NUM_ARGUMENTS_DEFAULT)
	{
		errorExit(argv[0], "Wrong number of arguments");
	}
	long numPlanes = scanInteger(argv[0], argv[1]);
	std::cout << "Number of support planes: " << numPlanes << std::endl;

	/*
	 * TODO:
	 *
	 * 1. Generate numPlanes support planes so that they are:
	 *  a.
	 *  b.
	 * (depending on an option)
	 *
	 * 2. Sort them by polar angle.
	 *
	 * 3. Select begin and end planes counts (depending on two more options)
	 *
	 * 4. Try to join select group of planes.
	 */
	return EXIT_SUCCESS;
}
