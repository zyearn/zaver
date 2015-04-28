
/*
 * Copyright (C) Zhu Jiashun
 * Copyright (C) Zaver
 */

#ifndef HTTP_PARSE_H
#define HTTP_PARSE_H

#define CR '\r'
#define LF '\n'
#define CRLFCRLF "\r\n\r\n"

extern int zv_http_parse_request_line(zv_http_request_t *r);
extern int zv_http_parse_request_body(zv_http_request_t *r);

#endif
