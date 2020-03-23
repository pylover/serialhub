#include "common.h"
#include "settings.h"

#include <stdlib.h>

volatile struct Settings settings = {
	DEFAULT_BIND, 
    NULL,
    DEFAULT_BAUDRATE,
	DEFAULT_TCPPORT, 
	DEFAULT_BACKLOG,
};
