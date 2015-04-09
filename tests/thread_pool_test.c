#include <threadpool.h>

#define THREAD_NUM 8

static void sum_n(void *arg) {
    int n = (int) arg;
    unsigned long sum = 0, i;
    for (i=0; i<n; i++) {
        sum += i;
    }
    
    log_info("thread %d complete sum %d: %lu", pthread_self(), n, sum);
}

int main() {
    int rc;
    zv_threadpool_t *tp = threadpool_init(THREAD_NUM);
    
    log_info("%d", EAGAIN);
    int i;
    for (i=0; i< 1; i++){
        log_info("ready to add num %d", i);
        rc = threadpool_add(tp, sum_n, (void *)i);
        check(rc == 0, "rc == 0");
    }
    sleep(100);
    return 0;
}
