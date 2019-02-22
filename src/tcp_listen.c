#include "tcp_listen.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int tcp_listen(const char *host, const char *serv)
{
    int err, listenfd;
    struct addrinfo hints, *res, *ressave;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if ((err = getaddrinfo(host, serv, &hints, &res) != 0)) {
        fprintf(stderr, "getaddrinfo error for %s, %s: %s\n",
                host, serv, gai_strerror(err));
        exit(EXIT_FAILURE);
    } 
    ressave = res;

    do {
        listenfd =
            socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (listenfd < 0)
            continue;
        int optval = 1;
        if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
                    &optval, sizeof(optval)) == -1) {
            perror("setsockopt");
            close(listenfd);
            exit(1);
        }
        if (bind(listenfd, res->ai_addr, res->ai_addrlen) == 0)
            break;
        close(listenfd); // bind error
    }while((res = res->ai_next) != NULL);

    if (res == NULL) {
        fprintf(stderr, "tcp_listen error for %s, %s\n", host, serv);
        exit(EXIT_FAILURE);
    }
    listen(listenfd, LISTENQ);

    freeaddrinfo(ressave);

    return listenfd;
}
