#ifndef H_COMMON
#define H_COMMON

#include <stdio.h>
#include <string.h>
#include <errno.h>


#define OK  0
#define ERR -1

/* It must be power-of-2, 2 ** n
 */
#define MAXEVENTS  10
#define MAXCONNECTIONS  2
#define CHUNKSIZE   1024


extern int errno;


/* Used to create string literal from integer during preprocess. */
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
 

#define printfln( ... ) LOG(stdout, __VA_ARGS__)
#define perrorf( ... ) LOG(stderr, __VA_ARGS__)

#define DEFAULT_TCPPORT     5600
#define DEFAULT_BAUDRATE    115200
#define DEFAULT_TCPBIND     "0.0.0.0"
#define DEFAULT_TCPBACKLOG  1
#define DEFAULT_UNIXFILE    "serialhub.socket"
#define DEFAULT_UNIXBACKLOG 1


struct Settings {
	char *device;
    unsigned int baudrate;
	char *tcpbind;
	unsigned int tcpport; 
	unsigned int tcpbacklog;
    char *unixfile;
    unsigned int unixbacklog;
};


volatile int epollfd;
volatile struct Settings settings;

#endif
