
#include "tty.h"
#include "common.h"
#include "settings.h"

#include <stdio.h>
#include <termios.h>
#include <fcntl.h>


int serialopen() {
    struct termios options;

    L_INFO("DEV: %s, %d", settings.device, settings.baudrate);
    int fd = open(settings.device, O_RDWR | O_NOCTTY); // | O_NONBLOCK);
    if (fd == -1) {
        L_ERROR("Can't open serial device: %s", settings.device);
        return fd;
    }

    tcgetattr(fd, &options);

    cfsetispeed(&options, settings.baudrate);
    cfsetospeed(&options, settings.baudrate);

    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);     /*Input*/
    options.c_oflag &= ~OPOST;                              /*Output*/

    tcsetattr(fd, TCSANOW, &options);

    return fd;
}
