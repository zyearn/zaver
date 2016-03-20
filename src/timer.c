
/*
 * Copyright (C) Zhu Jiashun
 * Copyright (C) Zaver
 */

#include <sys/time.h>
#include "timer.h"

static int timer_comp(void *ti, void *tj) {
    zv_timer_node *timeri = (zv_timer_node *)ti;
    zv_timer_node *timerj = (zv_timer_node *)tj;

    return (timeri->key < timerj->key)? 1: 0;
}

zv_pq_t zv_timer;
size_t zv_current_msec;

static void zv_time_update() {
    // there is only one thread calling zv_time_update, no need to lock?
    struct timeval tv;
    int rc;

    rc = gettimeofday(&tv, NULL);
    check(rc == 0, "zv_time_update: gettimeofday error");

    zv_current_msec = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    debug("in zv_time_update, time = %zu", zv_current_msec);
}


int zv_timer_init() {
    int rc;
    rc = zv_pq_init(&zv_timer, timer_comp, ZV_PQ_DEFAULT_SIZE);
    check(rc == ZV_OK, "zv_pq_init error");
   
    zv_time_update();
    return ZV_OK;
}

int zv_find_timer() {
    zv_timer_node *timer_node;
    int time = ZV_TIMER_INFINITE;
    int rc;

    while (!zv_pq_is_empty(&zv_timer)) {
        debug("zv_find_timer");
        zv_time_update();
        timer_node = (zv_timer_node *)zv_pq_min(&zv_timer);
        check(timer_node != NULL, "zv_pq_min error");

        if (timer_node->deleted) {
            rc = zv_pq_delmin(&zv_timer); 
            check(rc == 0, "zv_pq_delmin");
            free(timer_node);
            continue;
        }
             
        time = (int) (timer_node->key - zv_current_msec);
        debug("in zv_find_timer, key = %zu, cur = %zu",
                timer_node->key,
                zv_current_msec);
        time = (time > 0? time: 0);
        break;
    }
    
    return time;
}

void zv_handle_expire_timers() {
    debug("in zv_handle_expire_timers");
    zv_timer_node *timer_node;
    int rc;

    while (!zv_pq_is_empty(&zv_timer)) {
        debug("zv_handle_expire_timers, size = %zu", zv_pq_size(&zv_timer));
        zv_time_update();
        timer_node = (zv_timer_node *)zv_pq_min(&zv_timer);
        check(timer_node != NULL, "zv_pq_min error");

        if (timer_node->deleted) {
            rc = zv_pq_delmin(&zv_timer); 
            check(rc == 0, "zv_handle_expire_timers: zv_pq_delmin error");
            free(timer_node);
            continue;
        }
        
        if (timer_node->key > zv_current_msec) {
            return;
        }

        if (timer_node->handler) {
            timer_node->handler(timer_node->rq);
        }
        rc = zv_pq_delmin(&zv_timer); 
        check(rc == 0, "zv_handle_expire_timers: zv_pq_delmin error");
        free(timer_node);
    }
}

void zv_add_timer(zv_http_request_t *rq, size_t timeout, timer_handler_pt handler) {
    int rc;
    zv_timer_node *timer_node = (zv_timer_node *)malloc(sizeof(zv_timer_node));
    check(timer_node != NULL, "zv_add_timer: malloc error");

    zv_time_update();
    rq->timer = timer_node;
    timer_node->key = zv_current_msec + timeout;
    debug("in zv_add_timer, key = %zu", timer_node->key);
    timer_node->deleted = 0;
    timer_node->handler = handler;
    timer_node->rq = rq;

    rc = zv_pq_insert(&zv_timer, timer_node);
    check(rc == 0, "zv_add_timer: zv_pq_insert error");
}

void zv_del_timer(zv_http_request_t *rq) {
    debug("in zv_del_timer");
    zv_time_update();
    zv_timer_node *timer_node = rq->timer;
    check(timer_node != NULL, "zv_del_timer: rq->timer is NULL");

    timer_node->deleted = 1;
}
