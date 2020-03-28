#include "common.h"
#include "mux.h"

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
//#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/epoll.h>
//#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>   


#define CONNECTIONEVENTS   ( EPOLLIN | EPOLLRDHUP )


static struct connection* connections[MAXCONNECTIONS];


static int _getfreeslot() {
    int i;
    for (i = 0; i < MAXCONNECTIONS; i++) {
        if (connections[i] == NULL) {
            return i;
        }
    }
    return -1;
}


void connection_broadcast(const char *buff, int len) {
    int i, err;
    for (i = 0; i < MAXCONNECTIONS; i++) {
        if (connections[i] == NULL) {
            break;
        }
        err = write(connections[i]->sockfd, buff, len);
        // TODO: if err < len: close socket
        if (err <= 0) {
            connection_close(connections[i]);
        }
    }
}



int connection_close(struct connection *conn) {
    int err;
	struct epoll_event ev;

	ev.events = CONNECTIONEVENTS;
	ev.data.ptr = conn;
	err = epoll_ctl(epollfd, EPOLL_CTL_DEL, conn->sockfd, &ev);
	if (err == ERR) {
        perrorf("Cannot delete connection epoll events");
        return ERR;
	}

    err = close(conn->sockfd);
    if (err == ERR) {
        perrorf("Cannot close connection");
        return err;
    }
   
    printfln("removing connection: %d", conn->slot);
    connections[conn->slot] = NULL;
    free(conn);
    return OK;
}


int connection_add(int fd, struct sockaddr addr, enum connectiontype type) {
    int slot, err;
	struct epoll_event ev;
    struct connection *conn;

    slot = _getfreeslot();
    if (slot == ERR) {
        // No free spot, rejecting
        printfln("Rejecting connection");
        err = close(fd);
        if (err == ERR) {
            perrorf("Cannot close rejected connection");
        }
        return ERR; 
    }
    
    conn = malloc(sizeof(struct connection));
    if (conn == NULL) {
        perrorf("Cannot allocate connection memory");
        return ERR;
    }

    conn->slot = slot;
    conn->sockfd = fd;
    conn->type = type;
    conn->address = addr;

    // Register epoll events
	ev.events = CONNECTIONEVENTS;
	ev.data.ptr = conn;
	err = epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
	if (err == ERR) {
		perrorf("epoll_ctl: sockfd");
        free(conn);    
        return ERR;
	}

    connections[slot] = conn;
	return OK;
}

