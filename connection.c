#include "common.h"
#include "connection.h"

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>


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


int connection_registerevents(struct connection *conn) {
    // TODO: epoll events
    return OK;
}


int tcpconnection_accept(int epollfd, int listenfd) {
	int err, sockfd, slot;
	struct sockaddr addr; 
    struct connection *conn;
	socklen_t addrlen = sizeof(struct sockaddr);
    
    slot = _getfreeslot();
    if (slot == -1) {
        return FAILURE_MAXCONNECTIONS;
    }

	sockfd = accept(listenfd, &addr, &addrlen);
	if (sockfd == -1) {
		L_ERROR("accept()");
		return FAILURE_TCPACCEPT;
	}
    
    conn = malloc(sizeof(struct connection));
    if (conn == NULL) {
        L_ERROR("Cannot allocate connection memory");
        return FAILURE_MEMALLOCATE;
    }
    
    conn->slot = slot;
    conn->sockfd = sockfd;
    conn->type = CNTYPE_TCP;
    conn->address = (struct sockaddr_in*) &addr;
    conn->buffer.size = BUFFERSIZE;
    conn->buffer.blob = malloc(BUFFERSIZE);
    if (conn->buffer.blob == NULL) {
        L_ERROR("Cannot allocate buffer memory");
        return FAILURE_MEMALLOCATE;
    }

    connections[slot] = conn;
    return connection_registerevents(conn);
}


void tcpconnection_dispose(struct connection *conn) {
    free(conn->buffer.blob);
    free(conn);
}

