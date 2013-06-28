#ifndef DEBUGPRINT_H
#define DEBUGPRINT_H

//#define DEBUG
//#define DEBUG1
#define OUTPUT
//#define TCPRINT //Печатать вывод из Polyhedron::test_consections()
#define GLOBAL_CORRECTION_DERIVATIVE_TESTING

#define COLOUR_NORM  		"\x1B[0m"
#define COLOUR_RED  		"\x1B[31m"
#define COLOUR_GREEN  	"\x1B[32m"
#define COLOUR_YELLOW  	"\x1B[33m"
#define COLOUR_BLUE 		"\x1B[34m"
#define COLOUR_MAGENTA  "\x1B[35m"
#define COLOUR_CYAN  		"\x1B[36m"
#define COLOUR_WHITE  	"\x1B[37m"

#define WHERESTR  "[%s: %d]: "
#define WHEREARG  __PRETTY_FUNCTION__, __LINE__
#define STDERR_PRINT(...)       fprintf(stderr, __VA_ARGS__)

#ifndef NDEBUG
#		define DEBUG_PRINT(_fmt, ...)  \
	STDERR_PRINT(COLOUR_NORM WHERESTR _fmt "\n", \
			WHEREARG, ##__VA_ARGS__)
#		define DEBUG_START DEBUG_PRINT("Start")
#		define DEBUG_END DEBUG_PRINT("End")
#else
#		define DEBUG_PRINT(_fmt, ...)
#		define DEBUG_START
#		define DEBUG_END
#endif

#define MAIN_PRINT(_fmt, ...)  \
	STDERR_PRINT(COLOUR_NORM WHERESTR _fmt "\n", \
			WHEREARG, ##__VA_ARGS__)

#define ERROR_PRINT(_fmt, ...)  \
	STDERR_PRINT(COLOUR_RED WHERESTR _fmt "\n" COLOUR_NORM, \
			WHEREARG, ##__VA_ARGS__)

#define PRINT(_fmt, ...)  \
	STDERR_PRINT(COLOUR_NORM _fmt COLOUR_NORM, ##__VA_ARGS__)

#endif //DEBUGPRINT_H
