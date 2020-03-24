#ifndef H_COMMON
#define H_COMMON

#include <stdio.h>
#include <string.h>
#include <errno.h>


#define OK  0

enum {
    FAILURE_CLIPARSE = -1,
    FAILURE_SERIALDEVICE = -2,
    FAILURE_MEMALLOCATE = -3,
    FAILURE_TCPLISTEN = -4,
    FAILURE_TCPBIND = -5,
    FAILURE_INVALIDTCPADDRESS = -6,
    FAILURE_EPOLLFD = -7,
    FAILURE_EPOLLCTL = -8,
    FAILURE_EPOLLWAIT = -9,
    FAILURE_TCPACCEPT = -10,
};

#define _STR(X) #X
#define STR(X) _STR(X)


#define LOG(file, ...) do {	\
	fprintf(file, __VA_ARGS__); \
	fprintf(file, "\n" ); \
    if (file == stderr) { \
        fprintf(file, "%s", strerror(errno)); \
    } \
	fflush(file); \
} while(0)
 

#define L_INFO( ... ) LOG(stdout, __VA_ARGS__)
#define L_ERROR( ... ) LOG(stderr, __VA_ARGS__)


#endif
