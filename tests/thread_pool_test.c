#include <threadpool.h>

#define THREAD_NUM 8

pthread_mutex_t lock;
size_t sum = 0;

static void sum_n(void *arg) {
    size_t n = (size_t) arg;
    int rc;

    rc = pthread_mutex_lock(&lock);
    check_exit(rc == 0, "pthread_mutex_lock error");

    sum += n;

    rc = pthread_mutex_unlock(&lock);
    check_exit(rc == 0, "pthread_mutex_unlock error");
}

int main() {
    int rc;
    check_exit(pthread_mutex_init(&lock, NULL) == 0, "lock init error");

    zv_threadpool_t *tp = threadpool_init(THREAD_NUM);
    check_exit(tp != NULL, "threadpool_init error");
    
    size_t i;
    for (i=1; i< 1000; i++){
        rc = threadpool_add(tp, sum_n, (void *)i);
        check_exit(rc == 0, "threadpool_add error");
    }

    check_exit(threadpool_destroy(tp, 1) == 0, "threadpool_destroy error");

    check_exit(sum == 499500, "sum error");
    printf("pass thread_pool_test\n");
    return 0;
}
