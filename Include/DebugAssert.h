/*
 * DebugAssert.h
 *
 *  Created on: 30.03.2013
 *      Author: iliya
 */

#ifndef DEBUGASSERT_H_
#define DEBUGASSERT_H_

#include "PolyhedraCorrectionLibrary.h"

#ifndef NDEBUG
	#define ASSERT(x) assert(x)
#else
	#define ASSERT(x)
#endif

#endif /* DEBUGASSERT_H_ */
