#include "logging.h"

char loglevel = LOG_INFO;


const char * loglevel_strings [] = {
	"ERROR",	// 0
	"WARNING",	// 1
	"INFO",		// 2
	"DEBUG",	// 3
};


void logginginit(char level) {
	loglevel = level;
	L_INFO(
		"Logging Initialized, level: %s", 
		loglevel_strings[(int)level]
	);
}
