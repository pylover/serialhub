
#include "common.h"
#include "logging.h"
#include "tty.h"
#include "cli.h"
#include <stdio.h>


int main(int argc, char **argv) {
    settingsparse(argc, argv);

    printf("Hello\r\n");
    return 0;
}
