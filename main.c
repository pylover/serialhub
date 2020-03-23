
#include "common.h"
#include "logging.h"
#include "tty.h"
#include "cli.h"
#include <stdio.h>


int main(int argc, char **argv) {
    settingsparse(argc, argv);
    
    serialopen(settings.device, settings.baudrate);
    printf("Hello: %s\n", settings.device);
    return 0;
}
