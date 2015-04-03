#ifndef _UTIL_H
#define _UTIL_H

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>

#include "dbg.h"

// max number of listen queue
#define LISTENQ 100 

int open_listenfd(int port);
int make_socket_non_blocking(int fd);

#endif
