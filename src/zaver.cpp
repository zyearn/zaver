#include "util.h"

int main(int argc, char* argv[]) {
    int listenfd;
    int rc;
    struct sockaddr_in clientaddr;
    socklen_t inlen;
    
    listenfd = open_listenfd(3000);
    //rc = make_socket_non_blocking(listenfd);
    //check(rc == 0, "make_socket_non_blocking");

    while(1) {
        log_info("ready to accpet");
        int infd = accept(listenfd, (struct sockaddr *)&clientaddr, &inlen);
        if (infd == -1) {
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                /* we have processed all incoming connections */
                continue;
            } else {
                log_err("accept");
                break;
            }
        }

        close(infd);
        
    }
    return 0;
}
