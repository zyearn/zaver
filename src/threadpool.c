#include "threadpool.h"

static void *threadpool_worker(void *arg);

zv_threadpool_t *threadpool_init(int thread_num) {
    if (thread_num <= 0) {
        log_err("the arg of threadpool_init must greater than 0");
        return NULL;
    }

    zv_threadpool_t *pool;
    if ((pool = (zv_threadpool_t *)malloc(sizeof(zv_threadpool_t))) == NULL) {
        goto err;
    }

    pool->thread_count = 0;
    pool->queue_size = 0;
    pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * thread_num);
    /* dummy head */
    pool->head = (zv_task_t *)malloc(sizeof(zv_task_t));
    pool->head->func = NULL;
    pool->head->arg = NULL;
    pool->head->next = NULL;

    if ((pool->threads == NULL) || (pool->head == NULL)) {
        goto err;
    }

    if (pthread_mutex_init(&(pool->lock), NULL) != 0) {
        goto err;
    }

    if (pthread_cond_init(&(pool->cond), NULL) != 0) {
        pthread_mutex_lock(&(pool->lock));
        pthread_mutex_destroy(&(pool->lock));
        goto err;
    }
    
    int i, rc;
    for (i=0; i<thread_num; ++i) {
        rc = pthread_create(&(pool->threads[i]), NULL, threadpool_worker, (void *)pool);

        if (rc != 0) {
            threadpool_destory(pool);
            return NULL;
        }

        pool->thread_count++;
    }

    return pool;

err:
    if (pool) {
        threadpool_free(pool);
    }

    return NULL;
}


int threadpool_add(zv_threadpool_t *pool, void (*func)(void *), void *arg) {
    int rc;
    if (pool == NULL || func == NULL) {
        log_err("pool == NULL or func == NULL");
        return -1;
    }
    
    if (pthread_mutex_lock(&(pool->lock)) != 0) {
        log_err("pthread_mutex_lock");
        return -1;
    }
    
    // TODO: use a memory pool
    zv_task_t *task = (zv_task_t *)malloc(sizeof(zv_task_t));
    if (task == NULL) {
        log_err("add task fail");
        return -1;
    }
    
    // TODO: use a memory pool
    task->func = func;
    task->arg = arg;
    task->next = pool->head->next;
    pool->head->next = task;

    pool->queue_size++;
    
    rc = pthread_cond_signal(&(pool->cond));
    check(rc == 0, "pthread_cond_signal");

    if(pthread_mutex_unlock(&pool->lock) != 0) {
        log_err("pthread_mutex_unlock");
        return -1;
    }
    
    return 0;
}

int threadpool_free(zv_threadpool_t *pool) {
    if (pool == NULL) {
        log_err("pool == NULL");
        return -1;
    }

    if (pool->threads) {
        free(pool->threads);
    }

    zv_task_t *old;
    while (pool->head) {
        old = pool->head;
        pool->head = pool->head->next;
        free(old);
    }

    //TODO: how to free mutex and cond properly

    return 0;
}

int threadpool_destory(zv_threadpool_t *pool) {
    if (pool == NULL) {
        log_err("pool == NULL");
        return -1;
    }
    
    /*
    if (pthread_mutex_lock(&(pool->lock)) != 0) {
        return -1;
    }
    */

    do {
        // set the showdown flag of pool and wake up all thread    
    } while(0);

    return 0;
}

static void *threadpool_worker(void *arg) {
    if (arg == NULL) {
        log_err("arg should be type zv_threadpool_t*");
        return NULL;
    }

    zv_threadpool_t *pool = (zv_threadpool_t *)arg;
    zv_task_t *task;

    while (1) {
        pthread_mutex_lock(&(pool->lock));
        
        /*  Wait on condition variable, check for spurious wakeups. */
        while (pool->queue_size == 0) {
            log_info("%d goto sleep", pthread_self());
            pthread_cond_wait(&(pool->cond), &(pool->lock));
        }

        log_info("%d wake up", pthread_self());
        task = pool->head->next;
        if (task == NULL) {
            log_info("empyt task list");
            continue;
        }

        pool->head->next = task->next;
        pool->queue_size--;

        pthread_mutex_unlock(&(pool->lock));

        (*(task->func))(task->arg);
        log_info("%d complete its job", pthread_self());
        /* TODO: memory pool */
        free(task);
    }

    pthread_mutex_unlock(&(pool->lock));
    pthread_exit(NULL);

    return NULL;
}
