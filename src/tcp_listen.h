#ifndef INCLUDED_TCP_LISTEN_H
#define INCLUDED_TCP_LISTEN_H

#define LISTENQ 1024 // maximun number listened

int tcp_listen(const char *host, const char *server);

#endif
