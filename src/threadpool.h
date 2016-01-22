#ifndef _THREADPOOL_H
#define _THREADPOOL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>
#include "dbg.h"

#define THREAD_NUM 8

typedef struct zv_task_s {
    void (*func)(void *);
    void *arg;
    struct zv_task_s *next;
} zv_task_t;

typedef struct {
    pthread_mutex_t lock;
    pthread_cond_t cond;
    pthread_t *threads;
    zv_task_t *head;
    int thread_count;
    int queue_size;
    int shutdown;
    int started;
} zv_threadpool_t;

typedef enum {
    zv_tp_invalid   = -1,
    zv_tp_lock_fail = -2,
    zv_tp_already_shutdown  = -3,
    zv_tp_cond_broadcast    = -4,
    zv_tp_thread_fail       = -5,
    
} zv_threadpool_error_t;

zv_threadpool_t *threadpool_init(int thread_num);

int threadpool_add(zv_threadpool_t *pool, void (*func)(void *), void *arg);

int threadpool_destroy(zv_threadpool_t *pool, int gracegul);

#ifdef __cplusplus
}
#endif

#endif
