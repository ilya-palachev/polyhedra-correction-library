#ifndef DEBUGPRINT_H
#define DEBUGPRINT_H

#include "Polyhedron.h"

#define DBGPRINT(fmt, ...) \
	printf(fmt, __VA_ARGS__);

#define DBG_START \
	DBGPRINT("Start %s\n", __func__);

#define DBG_END \
	DBGPRINT("End %s \n", __func__);

#endif //DEBUGPRINT_H
