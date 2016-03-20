
/*
 * Copyright (C) Zhu Jiashun
 * Copyright (C) Zaver
 */

#ifndef UTIL_H
#define UTIL_H

// max number of listen queue
#define LISTENQ     1024

#define BUFLEN      8192

#define DELIM       "="

#define ZV_CONF_OK      0
#define ZV_CONF_ERROR   100

#define MIN(a,b) ((a) < (b) ? (a) : (b))

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
