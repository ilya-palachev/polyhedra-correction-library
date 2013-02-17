#ifndef DEBUGPRINT_H
#define DEBUGPRINT_H

#include "Polyhedron.h"

#define WHERESTR  "[file %s, function %s, line %d]: "
#define WHEREARG  __FILE__, __func__, __LINE__
#define DBGPRINT2(...)       fprintf(stderr, __VA_ARGS__)
#define DBGPRINT(_fmt, ...)  DBGPRINT2(WHERESTR _fmt "\n", WHEREARG, ##__VA_ARGS__)

#define DBG_START \
	DBGPRINT("Start\n");

#define DBG_END \
	DBGPRINT("End\n");

#endif //DEBUGPRINT_H
