
#include "common.h"
#include "logging.h"
#include "settings.h"
#include "tty.h"
#include "cli.h"
#include "networking.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>


int main(int argc, char **argv) {
    cliparse(argc, argv);
	struct sockaddr_in listenaddr;
    
    // Open serial port
    int serialfd = serialopen(settings.device, settings.baudrate);
    if (serialfd == -1) {
        exit(FAILURE_SERIALDEVICE);
    }

	// Listen on tcp port
    int tcplistenfd = tcp_bindandlisten(&listenaddr);
    if (tcplistenfd < 0) {
    }


    char buff[1024];
    int c;
    while (1) {
       c = read(serialfd, buff, sizeof(buff));
       if (c > 0) {
            buff[c] = 0;
            printf("%s", buff);
       }
    }
    printf("Hello: %d\n", serialfd);
    return 0;
}
