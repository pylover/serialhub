#include "common.h"


volatile struct Settings settings = {
    NULL,
    DEFAULT_BAUDRATE,
	DEFAULT_TCPBIND, 
	DEFAULT_TCPPORT, 
	DEFAULT_TCPBACKLOG,
    DEFAULT_UNIXFILE,
    DEFAULT_UNIXBACKLOG
};


