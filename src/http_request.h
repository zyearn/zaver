
/*
 * Copyright (C) Zhu Jiashun
 * Copyright (C) Zaver
 */

#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <time.h>
#include "http.h"

#define ZV_AGAIN    EAGAIN

#define ZV_HTTP_PARSE_INVALID_METHOD        10
#define ZV_HTTP_PARSE_INVALID_REQUEST       11
#define ZV_HTTP_PARSE_INVALID_HEADER        12

#define ZV_HTTP_UNKNOWN                     0x0001
#define ZV_HTTP_GET                         0x0002
#define ZV_HTTP_HEAD                        0x0004
#define ZV_HTTP_POST                        0x0008

#define ZV_HTTP_OK                          200

#define ZV_HTTP_NOT_MODIFIED                304

#define ZV_HTTP_NOT_FOUND                   404

#define MAX_BUF 8124

typedef struct zv_http_request_s {
    void *root;
    int fd;
    int epfd;
    char buf[MAX_BUF];  /* ring buffer */
    size_t pos, last;
    int state;
    void *request_start;
    void *method_end;   /* not include method_end*/
    int method;
    void *uri_start;
    void *uri_end;      /* not include uri_end*/ 
    void *path_start;
    void *path_end;
    void *query_start;
    void *query_end;
    int http_major;
    int http_minor;
    void *request_end;

    struct list_head list;  /* store http header */
    void *cur_header_key_start;
    void *cur_header_key_end;
    void *cur_header_value_start;
    void *cur_header_value_end;

    void *timer;
} zv_http_request_t;

typedef struct {
    int fd;
    int keep_alive;
    time_t mtime;       /* the modified time of the file*/
    int modified;       /* compare If-modified-since field with mtime to decide whether the file is modified since last time*/

    int status;
} zv_http_out_t;

typedef struct zv_http_header_s {
    void *key_start, *key_end;          /* not include end */
    void *value_start, *value_end;
    list_head list;
} zv_http_header_t;

typedef int (*zv_http_header_handler_pt)(zv_http_request_t *r, zv_http_out_t *o, char *data, int len);

typedef struct {
    char *name;
    zv_http_header_handler_pt handler;
} zv_http_header_handle_t;

void zv_http_handle_header(zv_http_request_t *r, zv_http_out_t *o);
int zv_http_close_conn(zv_http_request_t *r);

int zv_init_request_t(zv_http_request_t *r, int fd, int epfd, zv_conf_t *cf);
int zv_free_request_t(zv_http_request_t *r);

int zv_init_out_t(zv_http_out_t *o, int fd);
int zv_free_out_t(zv_http_out_t *o);

const char *get_shortmsg_from_status_code(int status_code);

extern zv_http_header_handle_t     zv_http_headers_in[];

#endif

