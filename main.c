
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


static char serialblob[BUFFERSIZE];
static struct ringbuffer serialbuffer = {
    serialblob,
    0,
    0,
    BUFFERSIZE
};


static int _process_serial_interface_io(struct epoll_event e) {
	int err = OK;
	if (e.events & EPOLLIN) {
		err = buffer_readinto(&serialbuffer, e.data.fd);
        // buffer full
        // EAGAIN, read will be blocked
        // TODO: continue here
	}
	return err;
}


int main(int argc, char **argv) {
    int serialfd, tcplistenfd, fdcount, i;
    struct epoll_event tcplistenevent, events[MAXEVENTS], *e;
	struct sockaddr_in listenaddr;
    
    // Parse command line arguments
    cliparse(argc, argv);
    
    // Open serial port
    serialfd = serialopen();
    if (serialfd == -1) {
        L_ERROR("Cannot open serial device: %s", settings.device);
        exit(EXIT_FAILURE);
    }

	// Listen on tcp port
    tcplistenfd = tcp_bindandlisten(&listenaddr);
    if (tcplistenfd < 0) {
    }
    
    // Create epoll instance
    epollfd = epoll_create1(0);
    if (epollfd < 0) {
        L_ERROR("Cannot create epoll file descriptor");
        exit(EXIT_FAILURE);
    }
    
    // Register epoll events
    tcplistenevent.events = EPOLLIN | EPOLLOUT;
    tcplistenevent.data.fd = tcplistenfd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, tcplistenfd, &tcplistenevent) == -1) {
        L_ERROR("epoll_ctl: EPOLL_CTL_ADD, tcplisten socket");
        exit(EXIT_FAILURE);
    }

    char buff[1024];
    while (1) {
        fdcount = epoll_wait(epollfd, events, MAXEVENTS, -1);
        if (fdcount == -1) {
            L_ERROR("epoll_wait returned: %d", fdcount);
            exit(EXIT_FAILURE);
        }
        
        for (i = 0; i < fdcount; i++) {
            e = &events[i];
            if (e->data.fd == tcplistenfd) {
                // Ignoring error.
				tcpconnection_accept(epollfd, tcplistenfd);
            }
        }
    }
    return 0;
}
