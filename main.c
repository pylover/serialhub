
#include "common.h"
#include "tty.h"
#include "cli.h"
#include "mux.h"
#include "tcp.h"
#include "unix.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>


static int serialfd;
int epollfd;


static int _process_serialio(struct epoll_event *e) {
    char buff[CHUNKSIZE];
	int bytes, fd;
    fd = e->data.fd;
	if (e->events & EPOLLIN) {
        bytes = read(fd, buff, CHUNKSIZE);
        if (bytes == ERR) {
            perrorf("Cannot read from serial interface");
            return bytes;
        }
        else if (bytes == 0) {
            perrorf("Serial file closed");
            return ERR;
        }
        connection_broadcast(buff, bytes);
	}
	return OK;
}


static int _process_connectionio(struct epoll_event *e) {
    char buff[CHUNKSIZE];
	int err, bytes;
    struct connection *conn = (struct connection *)e->data.ptr;
	if (e->events & EPOLLIN) {
        bytes = read(conn->sockfd, buff, CHUNKSIZE);
        if (bytes <= 0) {
            perrorf("Cannot read from tcp socket");
            connection_close(conn);
            return OK;
        }
       
        err = write(serialfd, buff, bytes);
        // Simplified version of (err == ERR) | (err < bytes)
        if (err < bytes) {             
            perrorf("Cannot write to serial device");
            return err;
        }
	}

    return OK; 
}


int main(int argc, char **argv) {
    int tcplistenfd, unixlistenfd, fdcount, err, i;
    struct epoll_event events[MAXEVENTS], *e;
    
    // Parse command line arguments
    cliparse(argc, argv);

    // Create epoll instance
    epollfd = epoll_create1(0);
    if (epollfd < 0) {
        perrorf("Cannot create epoll file descriptor");
        exit(EXIT_FAILURE);
    }
    
    // Open and register serial port
    serialfd = serialopen();
    if (serialfd == -1) {
        perrorf("Cannot open serial device: %s", settings.device);
        exit(EXIT_FAILURE);
    }

	// Listen on tcp port
    tcplistenfd = tcpconnection_listen();
    if (tcplistenfd == ERR) {
        perrorf("Cannot bind tcp socket");
        exit(EXIT_FAILURE);
    }
    
    // Listen on unix domain socket
    unixlistenfd = unixconnection_listen();
    if (unixlistenfd == ERR) {
        perrorf("Cannot bind unix domain socket");
        exit(EXIT_FAILURE);
    }
   
    /* Main Loop */
    while (1) {
        fdcount = epoll_wait(epollfd, events, MAXEVENTS, -1);
        if (fdcount == -1) {
            perrorf("epoll_wait returned: %d", fdcount);
            exit(EXIT_FAILURE);
        }
        
        for (i = 0; i < fdcount; i++) {
            e = &events[i];
            if (e->data.fd == tcplistenfd) {
				err = tcpconnection_accept(tcplistenfd);
                if (err == ERR) {
                    perrorf("TCP listen socket broken");
                    exit(EXIT_FAILURE);
                }
            }
            else if (e->data.fd == unixlistenfd) {
				unixconnection_accept(unixlistenfd);
                if (err == ERR) {
                    perrorf("UNIX listen socket broken");
                    exit(EXIT_FAILURE);
                }
            }
            else if (e->data.fd == serialfd) {
                err = _process_serialio(e);
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
