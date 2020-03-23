#ifndef H_CLI
#define H_CLI


struct Settings {
	char *bind;
	char *device;
    unsigned int baudrate;
	unsigned int tcpport; 
	unsigned int tcpbacklog;
};

volatile struct Settings settings;

void settingsparse(int argc, char **argv);

#endif

