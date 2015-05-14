
/*
 * Copyright (C) Zhu Jiashun
 * Copyright (C) Zaver
 */

#include <math.h>
#include "http.h"
#include "http_request.h"

static int zv_http_process_ignore(zv_http_request_t *r, zv_http_out_t *out, char *data, int len);
static int zv_http_process_connection(zv_http_request_t *r, zv_http_out_t *out, char *data, int len);
static int zv_http_process_if_modified_since(zv_http_request_t *r, zv_http_out_t *out, char *data, int len);

zv_http_header_handle_t zv_http_headers_in[] = {
    {"Host", zv_http_process_ignore},
    {"Connection", zv_http_process_connection},
    {"If-Modified-Since", zv_http_process_if_modified_since},
    {"", zv_http_process_ignore}
};

int zv_init_request_t(zv_http_request_t *r, int fd, zv_conf_t *cf) {
    r->fd = fd;
    r->pos = r->last = r->buf;
    r->state = 0;
    r->root = cf->root;
    INIT_LIST_HEAD(&(r->list));

    return ZV_OK;
}

int zv_free_request_t(zv_http_request_t *r) {
    // TODO
    return ZV_OK;
}

int zv_init_out_t(zv_http_out_t *o, int fd) {
    o->fd = fd;
    o->keep_alive = 0;
    o->modified = 1;
    o->status = 0;

    return ZV_OK;
}

int zv_free_out_t(zv_http_out_t *o) {
    // TODO
    return ZV_OK;
}

void zx_http_handle_header(zv_http_request_t *r, zv_http_out_t *o) {
    list_head *pos;
    zv_http_header_t *hd;
    zv_http_header_handle_t *header_in;

    list_for_each(pos, &(r->list)) {
        hd = list_entry(pos, zv_http_header_t, list);
        /* handle */

        for (header_in = zv_http_headers_in; 
            strlen(header_in->name) > 0;
            header_in++) {
            if (strncmp(hd->key_start, header_in->name, hd->key_end - hd->key_start) == 0) {
            
                debug("key = %.*s, value = %.*s", hd->key_end-hd->key_start, hd->key_start, hd->value_end-hd->value_start, hd->value_start);
                int len = hd->value_end-hd->value_start;
                (*(header_in->handler))(r, o, hd->value_start, len);
                break;
            }    
        }

        /* delete it from the original list */
        list_del(pos);
        free(hd);
    }
}

static int zv_http_process_ignore(zv_http_request_t *r, zv_http_out_t *out, char *data, int len) {
    
    return ZV_OK;
}

static int zv_http_process_connection(zv_http_request_t *r, zv_http_out_t *out, char *data, int len) {
    if (strncasecmp("keep-alive", data, len) == 0) {
        out->keep_alive = 1;
    }

    return ZV_OK;
}

static int zv_http_process_if_modified_since(zv_http_request_t *r, zv_http_out_t *out, char *data, int len) {
    struct tm tm;
    strptime(data, "%a, %d %b %Y %H:%M:%S GMT", &tm);
    time_t client_time = mktime(&tm);

    double time_diff = difftime(out->mtime, client_time);
    if (fabs(time_diff) < 1e6) {
        debug("not modified!!");
        /* Not modified */
        out->modified = 0;
        out->status = ZV_HTTP_NOT_MODIFIED;
    }
    
    return ZV_OK;
}

const char *get_shortmsg_from_status_code(int status_code) {
    /*  for code to msg mapping, please check: 
    * http://users.polytech.unice.fr/~buffa/cours/internet/POLYS/servlets/Servlet-Tutorial-Response-Status-Line.html
    */
    if (status_code = ZV_HTTP_OK) {
        return "OK";
    }

    if (status_code = ZV_HTTP_NOT_MODIFIED) {
        return "Not Modified";
    }

    if (status_code = ZV_HTTP_NOT_FOUND) {
        return "Not Found";
    }
    

    return "Unknown";
}
