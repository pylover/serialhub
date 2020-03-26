
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
static struct ringbuffer serialoutbuffer = {
    serialblob,
    0,
    0,
    BUFFERSIZE
};


static int _process_serialio(struct epoll_event *e) {
    char buff[CHUNKSIZE];
	int err, bytes, fd;
    fd = e->data.fd;
	if (e->events & EPOLLIN) {
        while (1) {
            bytes = read(fd, buff, CHUNKSIZE);
            if (bytes == ERR) {
                if (errno == EAGAIN) {
                    //L_ERROR("Eagain");
                    break;
                }
                L_ERROR("Cannot read from serial interface");
                return bytes;
            }
            else if (bytes == 0) {
                L_ERROR("Serial file closed");
                return ERR;
            }
            //printf("%d, %.*s\n", bytes, bytes, buff);
            printf("%.*s", bytes, buff);
            fflush(stdout);
            //connection_broadcast(buff, bytes);
        }
	}
	return OK;
}


static int _process_connectionio(struct epoll_event *e) {
    char buff[CHUNKSIZE];
	int err, bytes, fd;
    fd = e->data.fd;
	if (e->events & EPOLLIN) {
        while (1) {
            bytes = read(fd, buff, CHUNKSIZE);
            if (bytes == ERR) {
                if (errno == EAGAIN) {
                    L_ERROR("EAGAIN TCP READ");
                    break;
                }
                L_ERROR("Cannot read from tcp socket");
                return bytes;
            }
           
            printf("RCV TCP: %.*s", bytes, buff);
            err = bufferput(&serialoutbuffer, buff, bytes);
            if (err == ERR) {
                return err;
            }
        }
	}

    return OK; 
}

int main(int argc, char **argv) {
    int serialfd, tcplistenfd, fdcount, err, i;
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
    ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
    ev.data.fd = tcplistenfd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, tcplistenfd, &ev) == ERR) {
        L_ERROR("epoll_ctl: EPOLL_CTL_ADD, tcplisten socket");
        exit(EXIT_FAILURE);
    }

    // serialport
    ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
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
            } else if (e->data.fd == tcplistenfd) {
                err = _process_connectionio(e);
                if (err == ERR) {
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
    return 0;
}
