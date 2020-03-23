
#include "tty.h"
#include "settings.h"
#include "logging.h"

#include <stdio.h>
#include <termios.h>
#include <fcntl.h>


int serialopen(char *dev, unsigned int baudrate) {
    struct termios options;

    L_INFO("DEV: %s, %d", dev, baudrate);
    int fd = open(dev, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1) {
        L_ERROR("Can't open serial device: %s", dev);
        return fd;
    }

    tcgetattr(fd, &options);

    cfsetispeed(&options, baudrate);
    cfsetospeed(&options, baudrate);

    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);     /*Input*/
    options.c_oflag &= ~OPOST;                              /*Output*/

    tcsetattr(fd, TCSANOW, &options);

    return fd;
}
