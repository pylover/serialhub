
#include "common.h"
#include "logging.h"
#include "settings.h"
#include "tty.h"
#include "cli.h"

#include <stdio.h>
#include <stdlib.h>


int main(int argc, char **argv) {
    cliparse(argc, argv);
    
    int serialfd = serialopen(settings.device, settings.baudrate);
    if (serialfd == -1) {
        exit(FAILURE_SERIALDEVICE);
    }
    printf("Hello: %d\n", serialfd);
    return 0;
}
