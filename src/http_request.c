
/*
 * Copyright (C) Zhu Jiashun
 * Copyright (C) Zaver
 */

#include "http.h"


int zv_init_request_t(zv_http_request_t *r, int fd) {
    r->fd = fd;
    r->pos = r->last = r->buf;
    r->state = 0;
    INIT_LIST_HEAD(&(r->list));

    return ZV_OK;
}

int zv_free_request_t(zv_http_request_t *r) {
    
    return ZV_OK;
}

void zx_http_handle_header(zv_http_request_t *r) {
    list_head *pos;
    zv_http_header_t *hd;

    list_for_each(pos, &(r->list)) {
        hd = list_entry(pos, zv_http_header_t, list);
        /* handle */
        log_info("###header, %.*s:%.*s", 
            hd->key_end - hd->key_start,
            hd->key_start,
            hd->value_end - hd->value_start,
            hd->value_start);

        /* delete it from the original list */
        list_del(pos);
        free(hd);
    }
}
