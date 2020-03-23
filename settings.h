#ifndef H_SETTINGS
#define H_SETTINGS


#define DEFAULT_TCPPORT     5600
#define DEFAULT_BAUDRATE    115200
#define DEFAULT_BIND        "0.0.0.0"
#define DEFAULT_BACKLOG     1


struct Settings {
	char *bind;
	char *device;
    unsigned int baudrate;
	unsigned int tcpport; 
	unsigned int tcpbacklog;
};

volatile struct Settings settings;

#endif
