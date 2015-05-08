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
#include <stdlib.h>

#include "dbg.h"

// max number of listen queue
#define LISTENQ     1024

#define BUFLEN      8192

#define DELIM       "="

#define ZV_CONF_OK      0
#define ZV_CONF_ERROR   100

struct zv_conf_s {
    void *root;
    int port;
    int thread_num;
};

typedef struct zv_conf_s zv_conf_t;

int open_listenfd(int port);
int make_socket_non_blocking(int fd);

int read_conf(char *filename, zv_conf_t *cf, char *buf, int len);
#endif
