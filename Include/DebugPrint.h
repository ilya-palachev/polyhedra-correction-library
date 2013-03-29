#ifndef DEBUGPRINT_H
#define DEBUGPRINT_H

#include "PolyhedraCorrectionLibrary.h"

//#define DEBUG
//#define DEBUG1
#define OUTPUT
//#define TCPRINT //Печатать вывод из Polyhedron::test_consections()

#define WHERESTR  "[%s: %d]: "
#define WHEREARG  __func__, __LINE__
#define DBGPRINT2(...)       fprintf(stderr, __VA_ARGS__)

#ifndef NDEBUG
#define DBGPRINT(_fmt, ...)  DBGPRINT2(WHERESTR _fmt "\n", WHEREARG, ##__VA_ARGS__)
#else
#define DBGPRINT(_fmt, ...)
#endif

#define DBG_START \
	DBGPRINT("Start\n");

#define DBG_END \
	DBGPRINT("End\n");

#endif //DEBUGPRINT_H

