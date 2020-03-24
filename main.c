
#include "common.h"
#include "settings.h"
#include "tty.h"
#include "cli.h"
#include "networking.h"
#include "connection.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>


#define MAXEVENTS  10


int main(int argc, char **argv) {
    int serialfd, tcplistenfd, epollfd, fdcount, i;
    struct epoll_event tcplistenevent, events[MAXEVENTS], *e;
	struct sockaddr_in listenaddr;

    // Parse command line arguments
    cliparse(argc, argv);
    
    // Open serial port
    serialfd = serialopen(settings.device, settings.baudrate);
    if (serialfd == -1) {
        exit(FAILURE_SERIALDEVICE);
    }

	// Listen on tcp port
    tcplistenfd = tcp_bindandlisten(&listenaddr);
    if (tcplistenfd < 0) {
    }
    
    // Create epoll instance
    epollfd = epoll_create1(0);
    if (epollfd < 0) {
        L_ERROR("Cannot create epoll file descriptor");
        exit(FAILURE_EPOLLFD);
    }
    
    // Register epoll events
    tcplistenevent.events = EPOLLIN;
    tcplistenevent.data.fd = tcplistenfd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, tcplistenfd, &tcplistenevent) == -1) {
        L_ERROR("epoll_ctl: tcplisten");
        exit(FAILURE_EPOLLCTL);
    }

    char buff[1024];
    while (1) {
        fdcount = epoll_wait(epollfd, events, MAXEVENTS, -1);
        if (fdcount == -1) {
            L_ERROR("epoll_wait:");
            exit(FAILURE_EPOLLWAIT);
        }
        
        for (i = 0; i < fdcount; i++) {
            e = &events[i];
            if (e->data.fd == tcplistenfd) {
                // Ignoring error.
				tcpconnection_accept(epollfd, tcplistenfd);
            }
        }
    }
    printf("Hello: %d\n", serialfd);
    return 0;
}
