#include "util.h"
#include "http.h"
#include "epoll.h"

extern struct epoll_event *events;

int main(int argc, char* argv[]) {
    int listenfd;
    int rc;
    struct sockaddr_in clientaddr;
    socklen_t inlen;
    
    listenfd = open_listenfd(3000);
    rc = make_socket_non_blocking(listenfd);
    check(rc == 0, "make_socket_non_blocking");

    int epfd = zv_epoll_create(0);
    struct epoll_event event;
    event.data.fd = listenfd;
    event.events = EPOLLIN | EPOLLET;
    zv_epoll_add(epfd, listenfd, &event);

    while(1) {
        log_info("ready to wait");
        int n;
        n = zv_epoll_wait(epfd, events, MAXEVENTS, -1);
        
        int i;
        for (i=0; i<n; i++) {
            if ((events[i].events & EPOLLERR) ||
                (events[i].events & EPOLLHUP) ||
                (!(events[i].events & EPOLLIN))) {
                log_err("epoll error");
                close(events[i].data.fd);
                continue;
            } else if (listenfd == events[i].data.fd) {
                /* we hava one or more incoming connections */

                while(1) {
                    log_info("ready to accept");
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

                    do_request(infd);
                    close(infd);
                }
            
            }   // end of if
        }   //end of for
    }   // end of while(1)
    return 0;
}
