
/*
 * Copyright (C) Zhu Jiashun
 * Copyright (C) Zaver
 */

#include "priority_queue.h"

int zv_pq_init(zv_pq_t *zv_pq, zv_pq_comparator_pt comp, size_t size) {
    zv_pq->pq = (void **)malloc(sizeof(void *) * (size+1));
    if (!zv_pq->pq) {
        log_err("zv_pq_init: malloc failed");
        return -1;
    }
    
    zv_pq->nalloc = 0;
    zv_pq->size = size + 1;
    zv_pq->comp = comp;
    
    return ZV_OK;
}

int zv_pq_is_empty(zv_pq_t *zv_pq) {
    return (zv_pq->nalloc == 0)? 1: 0;
}

size_t zv_pq_size(zv_pq_t *zv_pq) {
    return zv_pq->nalloc;
}

void *zv_pq_min(zv_pq_t *zv_pq) {
    if (zv_pq_is_empty(zv_pq)) {
        return NULL;
    }

    return zv_pq->pq[1];
}

static int resize(zv_pq_t *zv_pq, size_t new_size) {
    if (new_size <= zv_pq->nalloc) {
        log_err("resize: new_size to small");
        return -1;
    }

    void **new_ptr = (void **)malloc(sizeof(void *) * new_size);
    if (!new_ptr) {
        log_err("resize: malloc failed");
        return -1;
    }

    memcpy(new_ptr, zv_pq->pq, sizeof(void *) * (zv_pq->nalloc + 1));
    free(zv_pq->pq);
    zv_pq->pq = new_ptr;
    zv_pq->size = new_size;
    return ZV_OK;
}

static void exch(zv_pq_t *zv_pq, size_t i, size_t j) {
    void *tmp = zv_pq->pq[i];
    zv_pq->pq[i] = zv_pq->pq[j];
    zv_pq->pq[j] = tmp;
}

static void swim(zv_pq_t *zv_pq, size_t k) {
    while (k > 1 && zv_pq->comp(zv_pq->pq[k], zv_pq->pq[k/2])) {
        exch(zv_pq, k, k/2);
        k /= 2;
    }
}

static size_t sink(zv_pq_t *zv_pq, size_t k) {
    size_t j;
    size_t nalloc = zv_pq->nalloc;

    while (2*k <= nalloc) {
        j = 2*k;
        if (j < nalloc && zv_pq->comp(zv_pq->pq[j+1], zv_pq->pq[j])) j++;
        if (!zv_pq->comp(zv_pq->pq[j], zv_pq->pq[k])) break;
        exch(zv_pq, j, k);
        k = j;
    }
    
    return k;
}

int zv_pq_delmin(zv_pq_t *zv_pq) {
    if (zv_pq_is_empty(zv_pq)) {
        return ZV_OK;
    }

    exch(zv_pq, 1, zv_pq->nalloc);
    zv_pq->nalloc--;
    sink(zv_pq, 1);
    if (zv_pq->nalloc > 0 && zv_pq->nalloc <= (zv_pq->size - 1)/4) {
        if (resize(zv_pq, zv_pq->size / 2) < 0) {
            return -1;
        }
    }

    return ZV_OK;
}

int zv_pq_insert(zv_pq_t *zv_pq, void *item) {
    if (zv_pq->nalloc + 1 == zv_pq->size) {
        if (resize(zv_pq, zv_pq->size * 2) < 0) {
            return -1;
        }
    }

    zv_pq->pq[++zv_pq->nalloc] = item;
    swim(zv_pq, zv_pq->nalloc);

    return ZV_OK;
}

int zv_pq_sink(zv_pq_t *zv_pq, size_t i) {
    return sink(zv_pq, i);
}
