#ifndef _HTTP_H
#define _HTTP_H

#include <strings.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "rio.h"
#include "list.h"

#define ZV_AGAIN    EAGAIN
#define ZV_OK       0

#define ZV_HTTP_PARSE_INVALID_METHOD        10
#define ZV_HTTP_PARSE_INVALID_REQUEST       11
#define ZV_HTTP_PARSE_INVALID_HEADER        12


#define ZV_HTTP_UNKNOWN                     0x0001
#define ZV_HTTP_GET                         0x0002
#define ZV_HTTP_HEAD                        0x0004
#define ZV_HTTP_POST                        0x0008
   
#define MAX_BUF                             8124

typedef struct list_head list_head;

typedef struct mime_type_s {
	const char *type;
	const char *value;
} mime_type_t;

typedef struct zv_http_request_s {
    int fd;
    char buf[MAX_BUF];
    void *pos, *last;
    int state;
    void *request_start;
    void *method_end;   /* not include method_end*/
    int method;
    void *uri_start;
    void *uri_end;      /* not include uri_end*/ 
    int http_major;
    int http_minor;
    void *request_end;

    struct list_head list;  /* store http header */
    void *cur_header_key_start;
    void *cur_header_key_end;
    void *cur_header_value_start;
    void *cur_header_value_end;

} zv_http_request_t;

typedef struct zv_http_header_s {
    void *key_start, *key_end;
    void *value_start, *value_end;
    list_head list;
} zv_http_header_t;

int zv_http_parse_request_line(zv_http_request_t *r);
int zv_http_parse_request_body(zv_http_request_t *r);
void zx_http_handle_header(zv_http_request_t *r);

int zv_init_request_t(zv_http_request_t *r, int fd);
int zv_free_request_t(zv_http_request_t *r);

const char* get_file_type(const char *type);
void do_request(void *infd);
void parse_uri(char *uri, int length, char *filename, char *querystring);
void do_error(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg);
void serve_static(int fd, char *filename, int filesize);

#endif
