
#include "tty.h"
#include "common.h"
#include "settings.h"

#include <stdio.h>
#include <termios.h>
#include <fcntl.h>

int termiosbaudrate(int b) {
    switch (b) {
        case 57600:
            return B57600;

        case 115200:
            return B115200;

        case 230400:
            return B230400;

        case 460800:
            return B460800;

        case 500000:
            return B500000;

        case 576000:
            return B576000;

        case 921600:
            return B921600;

        case 1000000:
            return B1000000;

        case 1152000:
            return B1152000;

        case 1500000:
            return B1500000;

        case 2000000:
            return B2000000;

        case 2500000:
            return B2500000;

        case 3000000:
            return B3000000;

        case 3500000:
            return B3500000;

        case 4000000:
            return B4000000;
         
    }
}


int serialopen() {
    struct termios options;

    L_INFO("DEV: %s, %d", settings.device, settings.baudrate);
    int fd = open(settings.device, O_RDWR | O_NDELAY); // | O_NOCTTY | O_NONBLOCK);
    if (fd == -1) {
        L_ERROR("Can't open serial device: %s", settings.device);
        return fd;
    }

    speed_t b = B115200;
    tcgetattr(fd, &options);
    // TODO: baudrate validation
    cfsetispeed(&options, termiosbaudrate(settings.baudrate));
    cfsetospeed(&options, termiosbaudrate(settings.baudrate));

    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);     /*Input*/
    options.c_oflag &= ~OPOST;                              /*Output*/
    tcsetattr(fd, TCSANOW, &options);
    tcflush(fd, TCOFLUSH);
    return fd;
}
