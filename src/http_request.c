
/*
 * Copyright (C) Zhu Jiashun
 * Copyright (C) Zaver
 */

#include "http.h"

static int zv_http_process_ignore(zv_http_out_t *out, char *data, size_t len);
static int zv_http_process_connection(zv_http_out_t *out, char *data, size_t len);
static int zv_http_process_if_modified_since(zv_http_out_t *out, char *data, size_t len);

zv_http_header_handle_t zv_http_headers_in[] = {
    {"Host", zv_http_process_ignore},
    {"Connection", zv_http_process_connection},
    {"If-Modified-Since", zv_http_process_if_modified_since},
    {"", zv_http_process_ignore}
};

int zv_init_request_t(zv_http_request_t *r, int fd) {
    r->fd = fd;
    r->pos = r->last = r->buf;
    r->state = 0;
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
        /*
        log_info("###header, %.*s:%.*s", 
            hd->key_end - hd->key_start,
            hd->key_start,
            hd->value_end - hd->value_start,
            hd->value_start);
            */


        for (header_in = zv_http_headers_in; 
            strlen(header_in->name) > 0;
            header_in++) {
            if (strncmp(hd->key_start, header_in->name, hd->key_end - hd->key_start) == 0) {
            
                log_info("find match: %s", header_in->name);
                (*(header_in->handler))(o, hd->value_start, hd->value_end - hd->value_start);
            }    
        }

        /* delete it from the original list */
        list_del(pos);
        free(hd);
    }
}

static int zv_http_process_ignore(zv_http_out_t *out, char *data, size_t len) {
    
    return ZV_OK;
}

static int zv_http_process_connection(zv_http_out_t *out, char *data, size_t len) {
    if (strncasecmp("keep-alive", data, len) == 0) {
        log_info("find Keep-alive!");
        out->keep_alive = 1;
    }

    return ZV_OK;
}

static int zv_http_process_if_modified_since(zv_http_out_t *out, char *data, size_t len) {
    
    return ZV_OK;
}
