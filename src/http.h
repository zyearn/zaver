#ifndef _HTTP_H
#define _HTTP_H

#include <strings.h>
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "rio.h"

typedef struct mime_type_s
{
	const char *type;
	const char *value;
}mime_type_t;

const char* get_file_type(const char *type);
void *do_request(void *infd);
int read_request_body(rio_t *rio);
void parse_uri(char *uri, char *filename, char *querystring);
void do_error(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg);
void serve_static(int fd, char *filename, int filesize);

#endif
