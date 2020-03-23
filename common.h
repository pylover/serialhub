#ifndef H_COMMON
#define H_COMMON

#define OK  0
#define NULL 0

#define FAILURE_SETTINGS    11


struct Settings {
	char *bind;
	char verbosity;
	char *device;
	unsigned int tcpport; 
	unsigned int tcpbacklog;
	unsigned int epollmaxevents;
};


#endif
