#ifndef _HTTP_H
#define _HTTP_H

#include <strings.h>
#include <stdio.h>

void do_request(int fd);
void do_error(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg);

#endif
