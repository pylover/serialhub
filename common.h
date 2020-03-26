#ifndef H_COMMON
#define H_COMMON

#include <stdio.h>
#include <string.h>
#include <errno.h>


#define OK  0
#define ERR -1

/* It must be power-of-2, 2 ** n
 */
#define BUFFERSIZE  16
#define MAXCONNECTIONS  1
#define CHUNKSIZE   BUFFERSIZE / 2



extern int errno;



#define _STR(X) #X
#define STR(X) _STR(X)


#define LOG(file, ...) do {	\
	fprintf(file, __VA_ARGS__); \
	fprintf(file, "\n" ); \
    if ((file == stderr) && errno) { \
        fprintf(file, "Additional info: %s\n", strerror(errno)); \
    } \
	fflush(file); \
} while(0)
 

#define L_INFO( ... ) LOG(stdout, __VA_ARGS__)
#define L_ERROR( ... ) LOG(stderr, __VA_ARGS__)

volatile int epollfd;

#endif
