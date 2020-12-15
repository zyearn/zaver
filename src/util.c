
/*
 * Copyright (C) Zhu Jiashun
 * Copyright (C) Zaver
 */

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include <stdlib.h>
#include "util.h"
#include "dbg.h"

int open_listenfd(int port) 
{
    if (port <= 0) {
        port = 3000;
    }

    int listenfd, optval=1;
    struct sockaddr_in serveraddr;
  
    /* Create a socket descriptor */
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	    return -1;
 
    /* Eliminates "Address already in use" error from bind. */
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, 
		   (const void *)&optval , sizeof(int)) < 0)
	    return -1;

    /* Listenfd will be an endpoint for all requests to port
       on any IP address for this host */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET; 
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    serveraddr.sin_port = htons((unsigned short)port); 
    if (bind(listenfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
	    return -1;

    /* Make it a listening socket ready to accept connection requests */
    if (listen(listenfd, LISTENQ) < 0)
	    return -1;

    return listenfd;
}

/*
    make a socket non blocking. If a listen socket is a blocking socket, after it comes out from epoll and accepts the last connection, the next accept will block, which is not what we want
*/
int make_socket_non_blocking(int fd) {
    int flags, s;
    flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        log_err("fcntl");
        return -1;
    }

    flags |= O_NONBLOCK;
    s = fcntl(fd, F_SETFL, flags);
    if (s == -1) {
        log_err("fcntl");
        return -1;
    }

    return 0;
}

/*
* Read configuration file
* TODO: trim input line
*/
int read_conf(char *filename, zv_conf_t *cf, char *buf, int len) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        log_err("cannot open config file: %s", filename);
        return ZV_CONF_ERROR;
    }

    int pos = 0;
    char *delim_pos;
    int line_len;
    char *cur_pos = buf+pos;

    while (fgets(cur_pos, len-pos, fp)) {
        delim_pos = strstr(cur_pos, DELIM);
        line_len = strlen(cur_pos);
        
        /*
        debug("read one line from conf: %s, len = %d", cur_pos, line_len);
        */
        if (!delim_pos)
            return ZV_CONF_ERROR;
        
        if (cur_pos[strlen(cur_pos) - 1] == '\n') {
            cur_pos[strlen(cur_pos) - 1] = '\0';
        }

        if (strncmp("root", cur_pos, 4) == 0) {
            cf->root = delim_pos + 1;
        }

        if (strncmp("port", cur_pos, 4) == 0) {
            cf->port = atoi(delim_pos + 1);     
        }

        if (strncmp("threadnum", cur_pos, 9) == 0) {
            cf->thread_num = atoi(delim_pos + 1);
        }

        cur_pos += line_len;
    }

    fclose(fp);
    return ZV_CONF_OK;
}
