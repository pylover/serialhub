#ifndef LOGGING_H 
#define LOGGING_H 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


extern int errno;

enum {
	LOG_ERROR,		// 0
	LOG_WARNING,	// 1
	LOG_INFO,		// 2
	LOG_DEBUG,		// 3
};
 
extern char loglevel;

extern const char * loglevel_strings[];

#define LOG_FP stdout
 
#define LOG_SHOULD_I( level ) ( level <= loglevel )

#define LOG(level, ...) do {	\
	if ( LOG_SHOULD_I(level) ) { \
		fprintf(LOG_FP, "[%s] ", \
			loglevel_strings[level]); \
		if ( level == LOG_ERROR ) \
			fprintf(LOG_FP, "[%s:%d: %d:%s] ", __FUNCTION__, __LINE__, \
					errno, strerror(errno)); \
		fprintf( LOG_FP, __VA_ARGS__ ); \
		fprintf( LOG_FP, "\n" ); \
		fflush( LOG_FP ); \
	} \
} while(0)
 

#define L_ERROR( ... ) LOG( LOG_ERROR, __VA_ARGS__ )
#define L_WARNING( ... ) LOG( LOG_WARNING, __VA_ARGS__ )
#define L_INFO( ... ) LOG( LOG_INFO, __VA_ARGS__ )
#define L_DEBUG( ... ) LOG( LOG_DEBUG,  __VA_ARGS__)


void logginginit(char level);
#endif
