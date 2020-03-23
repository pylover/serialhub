#ifndef LOGGING_H 
#define LOGGING_H 

#include <stdio.h>

#define LOG(file, ...) do {	\
	fprintf(file, __VA_ARGS__); \
	fprintf(file, "\n" ); \
	fflush(file); \
} while(0)
 

#define L_INFO( ... ) LOG(stdout, __VA_ARGS__)
#define L_ERROR( ... ) LOG(stderr, __VA_ARGS__)


#endif
