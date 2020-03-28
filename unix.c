#include "common.h"
#include "mux.h"

#include <unistd.h>
#include <sys/un.h>


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


int unixconnection_accept(int listenfd) {
	int err, sockfd;
    struct sockaddr addr;
    
	sockfd = accept(listenfd, NULL, NULL);
	if (sockfd == ERR) {
        if (errno == EBADF) {
            return ERR;
        }
		perrorf("Cannot accept unix domain socket connection");
        // Ignoring error, see accept(2)
        return OK;
	}
    
    struct sockaddr_un *ii = (struct sockaddr_un*)&addr;
    printfln("New connection: %s", ii->sun_path);
    err = connection_add(sockfd, addr, CNTYPE_UNIX);
    if (err == ERR) {
		perrorf("Cannot accept new tcp connection");
        // Just cannot handle the new connection.
    }
    return OK;
}


