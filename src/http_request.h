
/*
 * Copyright (C) Zhu Jiashun
 * Copyright (C) Zaver
 */

#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#define ZV_AGAIN    EAGAIN
#define ZV_OK       0

#define ZV_HTTP_PARSE_INVALID_METHOD        10
#define ZV_HTTP_PARSE_INVALID_REQUEST       11
#define ZV_HTTP_PARSE_INVALID_HEADER        12

#define ZV_HTTP_UNKNOWN                     0x0001
#define ZV_HTTP_GET                         0x0002
#define ZV_HTTP_HEAD                        0x0004
#define ZV_HTTP_POST                        0x0008

#define MAX_BUF 8124

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

extern void zx_http_handle_header(zv_http_request_t *r);
extern int zv_init_request_t(zv_http_request_t *r, int fd);
extern int zv_free_request_t(zv_http_request_t *r);

#endif
