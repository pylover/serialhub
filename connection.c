#include "common.h"
#include "connection.h"

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>


int connection_register(struct connection *conn) {
    return OK;
}


int tcpconnection_accept(int epollfd, int listenfd) {
	int err, sockfd;
	struct sockaddr addr; 
    struct connection *conn;
	socklen_t addrlen = sizeof(struct sockaddr);

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
    
    conn->sockfd = sockfd;
    conn->type = CNTYPE_TCP;
    conn->address = (struct sockaddr_in*) &addr;
    conn->buffer.size = BUFFERSIZE;
    conn->buffer.blob = malloc(BUFFERSIZE);

    if (conn->buffer.blob == NULL) {
        L_ERROR("Cannot allocate buffer memory");
        return FAILURE_MEMALLOCATE;
    }
    return connection_register(conn);
}

