#include "common.h"
#include "mux.h"

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/un.h>
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


int connection_unregisterevents(struct connection *conn) {
    int err;
	struct epoll_event ev;

	ev.events = CONNECTIONEVENTS;
	ev.data.ptr = conn;
	err = epoll_ctl(epollfd, EPOLL_CTL_DEL, conn->sockfd, &ev);
	if (err == ERR) {
		perrorf("epoll_ctl: sockfd");
        return ERR;
	}
	return OK;
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
    
    err = connection_unregisterevents(conn);
    if (err == ERR) {
        perrorf("Cannot delete connection epoll events");
        return err;
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


int tcpconnection_accept(int listenfd) {
	int err, sockfd;
	struct sockaddr addr; 
	socklen_t addrlen = sizeof(struct sockaddr);
    
	sockfd = accept(listenfd, &addr, &addrlen);
	if (sockfd == ERR) {
        if (errno == EBADF) {
            return ERR;
        }
        // TODO: LOG address AND THE REASON
		perrorf("Cannot accept new tcp connection");
        // Ignoring error, see accept(2)
        return OK;
	}
    
    struct sockaddr_in *ii = (struct sockaddr_in*)&addr;
    printfln(
        "New TCP connection: %s:%d", 
        inet_ntoa(ii->sin_addr), 
        ii->sin_port
    );
    
    err = connection_add(sockfd, addr, CNTYPE_TCP);
    if (err == ERR) {
		perrorf("Cannot accept new tcp connection");
        // Just cannot handle the new connection.
    }
    return OK;
}


int unixconnection_accept(int listenfd) {
	int sockfd;
    struct sockaddr addr;
    
	sockfd = accept(listenfd, NULL, NULL);
	if (sockfd == ERR) {
		perrorf("tcp accept");
        return ERR;
	}
    
    struct sockaddr_un *ii = (struct sockaddr_un*)&addr;
    printfln("New connection: %s", ii->sun_path);
    return connection_add(sockfd, addr, CNTYPE_UNIX);
}

int tcpconnection_listen() {
    int listenfd;
	int option = 1;
	int err;
    struct sockaddr_in listenaddr;

	listenfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    
    memset(&listenaddr, 0, sizeof(struct sockaddr_in));
	listenaddr.sin_family = AF_INET;
	
	if (settings.tcpbind == NULL) {
		listenaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	} 
    else if(inet_pton(AF_INET, settings.tcpbind, &listenaddr.sin_addr)<=0) {
        perrorf("Invalid address: %s", settings.tcpbind);
        return ERR;
	}
	listenaddr.sin_port = htons(settings.tcpport); 
	err = bind(listenfd, (struct sockaddr*)&listenaddr, sizeof(listenaddr)); 
	if (err) {
		perrorf("Cannot bind on: %s", inet_ntoa(listenaddr.sin_addr));
        return ERR;
	}
	
	err = listen(listenfd, settings.tcpbacklog); 
	if (err) {
		perrorf("Cannot listen on: %s", inet_ntoa(listenaddr.sin_addr));
        return ERR;
	}
	printfln(
		"Listening on %s:%d", 
		inet_ntoa(listenaddr.sin_addr),
		ntohs(listenaddr.sin_port)
	);
    return listenfd;
}


int unixconnection_listen() {
    struct sockaddr_un name;
    int err, sockfd;
    
    /*
     * In case the program exited inadvertently on the last run,
     * remove the socket.
     */
    unlink(settings.unixfile);
    
    /* Create local socket. */
    //sockfd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd == ERR) {
        perrorf("Cannot create unix domain socket");
        return ERR;
    }
    
    /*
     * For portability clear the whole structure, since some
     * implementations have additional (nonstandard) fields in
     * the structure.
     */
    
    memset(&name, 0, sizeof(struct sockaddr_un));
    
    /* Bind socket to socket name. */
    name.sun_family = AF_UNIX;
    strncpy(name.sun_path, settings.unixfile, sizeof(name.sun_path) - 1);
    
    err = bind(sockfd, (const struct sockaddr *) &name, 
            sizeof(struct sockaddr_un));
    if (err == ERR) {
        perrorf("Cannot bind to unix domain socket");
        return ERR;
    }
    
    /*
     * Prepare for accepting connections. The backlog size is set
     * to 20. So while one request is being processed other requests
     * can be waiting.
     */
    
    err = listen(sockfd, settings.unixbacklog);
    if (err == ERR) {
        perrorf("Cannot listen on unix domain socket");
        return ERR;
    }
    
	printfln("Listening on unix domain socket: %s", settings.unixfile);
    return sockfd;
}

