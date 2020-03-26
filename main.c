
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


static int serialfd;


static int _process_serialio(struct epoll_event *e) {
    char buff[CHUNKSIZE];
	int err, bytes, fd;
    fd = e->data.fd;
	if (e->events & EPOLLIN) {
        bytes = read(fd, buff, CHUNKSIZE);
        if (bytes == ERR) {
            L_ERROR("Cannot read from serial interface");
            return bytes;
        }
        else if (bytes == 0) {
            L_ERROR("Serial file closed");
            return ERR;
        }
        L_RAW("%.*s", bytes, buff);
        connection_broadcast(buff, bytes);
	}
	return OK;
}

// TODO: delete buffers and allocations
//
static int _process_connectionio(struct epoll_event *e) {
    char buff[CHUNKSIZE];
	int err, bytes;
    struct connection *conn = (struct connection *)e->data.ptr;
    L_INFO("Event TCP");
	if (e->events & EPOLLIN) {
        bytes = read(conn->sockfd, buff, CHUNKSIZE);
        if (bytes <= 0) {
            L_ERROR("Cannot read from tcp socket");
            connection_close(conn);
            return OK;
        }
       
        L_INFO("RCV TCP: %.*s", bytes, buff);
        err = write(serialfd, buff, bytes);
        if (err == ERR) {
            L_ERROR("Cannot write to serial device");
            return err;
        }
	}
    else if (e->events & EPOLLOUT) { 
        L_INFO("Connection write");
    }
    // TODO: EPOLLRDHUP

    return OK; 
}


int main(int argc, char **argv) {
    int tcplistenfd, fdcount, err, i;
    struct epoll_event ev, events[MAXEVENTS], *e;
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
    if (tcplistenfd == ERR) {
        L_ERROR("Cannot bind tcp socket");
        exit(EXIT_FAILURE);
    }
    
    // Create epoll instance
    epollfd = epoll_create1(0);
    if (epollfd < 0) {
        L_ERROR("Cannot create epoll file descriptor");
        exit(EXIT_FAILURE);
    }
    
    // Register epoll events
    // tcplisten
    ev.events = EPOLLIN | EPOLLOUT;
    ev.data.fd = tcplistenfd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, tcplistenfd, &ev) == ERR) {
        L_ERROR("epoll_ctl: EPOLL_CTL_ADD, tcplisten socket");
        exit(EXIT_FAILURE);
    }

    // serialport
    ev.events = EPOLLIN | EPOLLOUT;
    ev.data.fd = serialfd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, serialfd, &ev) == ERR) {
        L_ERROR("epoll_ctl: EPOLL_CTL_ADD, serial interface");
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
            else if (e->data.fd == serialfd) {
                err = _process_serialio(e);
                // EAGAIN, read will be blocked
                if (err == ERR) {
                    exit(EXIT_FAILURE);
                }
            } 
            else {
                err = _process_connectionio(e);
                if (err == ERR) {
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
    return 0;
}
