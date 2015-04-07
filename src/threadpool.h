#ifndef _THREADPOOL_H
#define _THREADPOOL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "dbg.h"

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
} zv_threadpool_t;

zv_threadpool_t *threadpool_init(int thread_num);

int threadpool_add(zv_threadpool_t *pool, void (*func)(void *), void *arg);

int threadpool_destory(zv_threadpool_t *pool);
int threadpool_free(zv_threadpool_t *pool);

#ifdef __cplusplus
}
#endif
#endif
