#include "tcp_listen.h"
#include "handle_request.h"

#include <stdio.h> // perror
#include <stdlib.h> // exit
#include <sys/types.h>  // accept
#include <sys/socket.h> // accept
#include <unistd.h> // close

int main(int argc, char *argv[])
{
    int listenfd = -1;
    int acceptfd = -1;
    const char *server_port = "9220";
    struct sockaddr_storage client_addr;
    socklen_t addrlen = sizeof(client_addr);

    listenfd = tcp_listen(NULL, server_port);
    printf("httpd listen on port: %s\n", server_port);
    while (1) {
        acceptfd = accept(listenfd, 
                (struct sockaddr *)(&client_addr),
                &addrlen);
        if (acceptfd == -1) {
            perror("accept");
            close(listenfd);
            exit(EXIT_FAILURE);
        }
        accept_request(acceptfd);
        
        printf("1234: %d\n", acceptfd);
        close(acceptfd);
    }

    close(listenfd);
    return 0;
}
