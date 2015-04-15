#include "util.h"
#include "http.h"
#include "epoll.h"
#include "threadpool.h"

extern struct epoll_event *events;

int main(int argc, char* argv[]) {

    /*
    initialize listening socket
    */
    int listenfd;
    int rc;
    struct sockaddr_in clientaddr;
    socklen_t inlen;
    
    listenfd = open_listenfd(3000);
    rc = make_socket_non_blocking(listenfd);
    check(rc == 0, "make_socket_non_blocking");

    /*
    create epoll and add listenfd to ep
    */
    int epfd = zv_epoll_create(0);
    struct epoll_event event;
    event.data.fd = listenfd;
    event.events = EPOLLIN | EPOLLET;
    zv_epoll_add(epfd, listenfd, &event);

    /*
    create thread pool
    */
    zv_threadpool_t *tp = threadpool_init(THREAD_NUM);
    
    /* epoll_wait loop */
    while(1) {
        log_info("ready to wait");
        fflush(stdout);
        int n;
        n = zv_epoll_wait(epfd, events, MAXEVENTS, -1);
        
        int i;
        for (i=0; i<n; i++) {
            if ((events[i].events & EPOLLERR) ||
                (events[i].events & EPOLLHUP) ||
                (!(events[i].events & EPOLLIN))) {
                zv_http_request_t *r = (zv_http_request_t *)events[i].data.ptr;
                log_err("epoll error %d", r->fd);
                close(events[i].data.fd);
                continue;
            }
            
            if (listenfd == events[i].data.fd) {
                /* we hava one or more incoming connections */

                while(1) {
                    log_info("## ready to accept");
                    fflush(stdout);
                    int infd = accept(listenfd, (struct sockaddr *)&clientaddr, &inlen);
                    if (infd == -1) {
                        if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                            /* we have processed all incoming connections */
                            break;
                        } else {
                            log_err("accept");
                            break;
                        }
                    }

                    rc = make_socket_non_blocking(infd);
                    check(rc == 0, "make_socket_non_blocking");
                    
                    zv_http_request_t *request = (zv_http_request_t *)malloc(sizeof(zv_http_request_t));
                    zv_init_request_t(request, infd);
                    event.data.ptr = (void *)request;
                    event.events = EPOLLIN | EPOLLET;

                    zv_epoll_add(epfd, infd, &event);
                }   // end of while of accept

                log_info("## end accept");
                fflush(stdout);

            } else {
                /*
                do_request(infd);
                close(infd);
                */

                rc = threadpool_add(tp, do_request, events[i].data.ptr);
                 
            }
        }   //end of for
    }   // end of while(1)
    return 0;
}
