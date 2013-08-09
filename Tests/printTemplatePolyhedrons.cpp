#include "PolyhedraCorrectionLibrary.h"

void test_figures();

int main(int argc, char** argv)
{
	DEBUG_START;
	test_figures();
	DEBUG_END;
}

void test_figures()
{
	DEBUG_START;
	char *curr_time, *file_name;

	file_name = new char[255];

	time_t seconds = time(NULL);
	tm* timeinfo = localtime(&seconds);
	curr_time = asctime(timeinfo);

	Polyhedron* poly;

/////// Pyramid /////////
	sprintf(file_name, "../poly-data-out/pyramid - %d-%d-%d %d:%d:%d.ply",
			timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
			timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

	poly = new Pyramid(10, 3., 1.);

#ifndef NDEBUG
	poly->fprint_ply_scale(1e3, file_name, "created-by-polyhedron");
#endif

	sprintf(file_name, "../poly-data-out/pyramid - %d-%d-%d %d:%d:%d.txt",
			timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
			timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
	poly->preprocessAdjacency();
	poly->fprint_my_format(file_name);
#ifndef NDEBUG
	poly->my_fprint("poly-data-out/my_fprint.txt");
#endif
	poly->my_fprint(stdout);
	delete poly;

/////// Prism /////////
	sprintf(file_name, "../poly-data-out/prism - %d-%d-%d %d:%d:%d.ply",
			timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
			timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

	poly = new Prism(10, 3., 1.);
#ifndef NDEBUG
	poly->fprint_ply_scale(1e3, file_name, "created-by-polyhedron");
#endif

	sprintf(file_name, "../poly-data-out/prism - %d-%d-%d %d:%d:%d.txt",
			timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
			timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
	poly->preprocessAdjacency();
#ifndef NDEBUG
	poly->fprint_my_format(file_name);
#endif
	delete poly;

	DEBUG_END;
}

