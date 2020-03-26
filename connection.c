#include "common.h"
#include "circularbuffer.h"
#include "connection.h"

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>


#define CONNECTIONEVENTS   ( EPOLLIN | EPOLLOUT | EPOLLET | EPOLLRDHUP )


static struct connection* connections[MAXCONNECTIONS];


static int _setnonblocking(int fd) {
	int opts;
	if ((opts = fcntl(fd, F_GETFL)) == ERR) {
		L_ERROR("GETFL %d failed", fd);
        return ERR;
	}
	opts = opts | O_NONBLOCK;
	if (fcntl(fd, F_SETFL, opts) == ERR) {
		L_ERROR("SETFL %d failed", fd);
        return ERR;
	}
	return OK;
}


static int _getfreeslot() {
    int i;
    for (i = 0; i < MAXCONNECTIONS; i++) {
        if (connections[i] == NULL) {
            return i;
        }
    }
    return -1;
}




int connection_registerevents(struct connection *conn) {
    int err;
	struct epoll_event ev;

	ev.events = CONNECTIONEVENTS;
	ev.data.ptr = conn;
	err = epoll_ctl(epollfd, EPOLL_CTL_ADD, conn->sockfd, &ev);
	if (err == ERR) {
		L_ERROR("epoll_ctl: sockfd");
        return ERR;
	}
	return OK;
}


int connection_unregisterevents(struct connection *conn) {
    int err;
	struct epoll_event ev;

	ev.events = CONNECTIONEVENTS;
	ev.data.ptr = conn;
	err = epoll_ctl(epollfd, EPOLL_CTL_DEL, conn->sockfd, &ev);
	if (err == ERR) {
		L_ERROR("epoll_ctl: sockfd");
        return ERR;
	}
	return OK;
}


int tcpconnection_accept(int epollfd, int listenfd) {
	int err, sockfd, slot;
	struct sockaddr addr; 
    struct connection *conn;
	socklen_t addrlen = sizeof(struct sockaddr);
    
    slot = _getfreeslot();
    if (slot == ERR) {
        return ERR;
    }
    
	sockfd = accept(listenfd, &addr, &addrlen);
	if (sockfd == ERR) {
		L_ERROR("tcp accept");
        return ERR;
	}
    
	err = _setnonblocking(sockfd);
    if (err == ERR) {
        L_ERROR("Cannot set nonblocking on tcp socket");
        return err;
    }

    conn = malloc(sizeof(struct connection));
    if (conn == NULL) {
        L_ERROR("Cannot allocate connection memory");
        return ERR;
    }
    
    conn->slot = slot;
    conn->sockfd = sockfd;
    conn->type = CNTYPE_TCP;
    conn->address = (struct sockaddr_in*) &addr;
    conn->outbuffer.size = BUFFERSIZE;
    conn->outbuffer.head = 0;
    conn->outbuffer.tail = 0;
    conn->outbuffer.blob = malloc(BUFFERSIZE);
    if (conn->outbuffer.blob == NULL) {
        L_ERROR("Cannot allocate buffer memory");
        return ERR;
    }

    connections[slot] = conn;
    return connection_registerevents(conn);
}


void connection_broadcast(const char *buff, int len) {
    int i, err;
    for (i = 0; i < MAXCONNECTIONS; i++) {
        if (connections[i] == NULL) {
            break;
        }
        L_INFO("Connection found: %d", i);
        err = bufferput(&(connections[i]->outbuffer), buff, len);
        if (err == ERR) {
            // Buffer full
            if (errno == ENOBUFS) {
                L_ERROR("TCP Buffer full, closing connection");
                connection_close(connections[i]);
            }
        }
    }
}


int connection_close(struct connection *conn) {
    int err;
    
    err = connection_unregisterevents(conn);
    if (err == ERR) {
        L_ERROR("Cannot delete connection epoll events");
        return err;
    }

    err = close(conn->sockfd);
    if (err == ERR) {
        L_ERROR("Cannot close connection");
        return err;
    }
   
    L_INFO("removing connection: %d", conn->slot);
    connections[conn->slot] = NULL;
    free(conn->outbuffer.blob);
    free(conn);
    return OK;
}



