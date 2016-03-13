#include <threadpool.h>

#define THREAD_NUM 8

static void sum_n(void *arg) {
    size_t n = (size_t) arg;
    size_t sum = 0, i;
    for (i=0; i<n; i++) {
        sum += i;
    }
    
    log_info("thread %08x complete sum %zu: %zu", (unsigned int)pthread_self(), n, sum);
}

int main() {
    int rc;
    zv_threadpool_t *tp = threadpool_init(THREAD_NUM);
    
    size_t i;
    for (i=0; i< 100; i++){
        log_info("ready to add num %zu", i);
        rc = threadpool_add(tp, sum_n, (void *)i);
        check(rc == 0, "rc == 0");
    }

    if (threadpool_destroy(tp, 1) < 0) {
        log_err("destroy threadpool failed");
    }
    return 0;
}
