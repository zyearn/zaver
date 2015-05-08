
/*
 * Copyright (C) Zhu Jiashun
 * Copyright (C) Zaver
 */

#include <stdint.h>
#include <getopt.h>
#include "util.h"
#include "http.h"
#include "epoll.h"
#include "threadpool.h"

#define CONF "zaver.conf"
#define PROGRAM_VERSION "0.0"

extern struct epoll_event *events;

static const struct option long_options[]=
{
    {"help",no_argument,NULL,'?'},
    {"version",no_argument,NULL,'V'},
    {"conf",required_argument,NULL,'c'},
    {NULL,0,NULL,0}
};

static void usage() {
   fprintf(stderr,
	"zaver [option]... \n"
	"  -c|--conf <config file>  Specify config file. Default ./zaver.conf.\n"
	"  -?|-h|--help             This information.\n"
	"  -V|--version             Display program version.\n"
	);
}

int main(int argc, char* argv[]) {
    int rc;
    int opt = 0;
    int options_index = 0;
    char *conf_file = CONF;

    /*
    * parse argv 
    * more detail visit: http://www.gnu.org/software/libc/manual/html_node/Getopt.html
    */

    if (argc == 1) {
        usage();
        return 0;
    }

    while ((opt=getopt_long(argc, argv,"Vc:?h",long_options,&options_index)) != EOF) {
        switch (opt) {
            case  0 : break;
            case 'c':
                conf_file = optarg;
                break;
            case 'V':
                printf(PROGRAM_VERSION"\n");
                return 0;
            case ':':
            case 'h':
            case '?':
                usage();
                return 0;
        }
    }

    debug("conffile = %s", conf_file);

    if (optind < argc) {
        log_err("non-option ARGV-elements: ");
        while (optind < argc)
            log_err("%s ", argv[optind++]);
        return 0;
    }

    /*
    * read confile file
    */
    char conf_buf[BUFLEN];
    zv_conf_t cf;
    rc = read_conf(conf_file, &cf, conf_buf, BUFLEN);
    check(rc == ZV_CONF_OK, "read conf err");

    /*
    * initialize listening socket
    */
    int listenfd;
    struct sockaddr_in clientaddr;
    // initialize clientaddr and inlen to solve "accept Invalid argument" bug
    socklen_t inlen = 1;
    memset(&clientaddr, 0, sizeof(struct sockaddr_in));  
    
    listenfd = open_listenfd(cf.port);
    rc = make_socket_non_blocking(listenfd);
    check(rc == 0, "make_socket_non_blocking");

    /*
    * create epoll and add listenfd to ep
    */
    int epfd = zv_epoll_create(0);
    struct epoll_event event;
    
    zv_http_request_t *request = (zv_http_request_t *)malloc(sizeof(zv_http_request_t));
    zv_init_request_t(request, listenfd, &cf);

    event.data.ptr = (void *)request;
    event.events = EPOLLIN | EPOLLET;
    zv_epoll_add(epfd, listenfd, &event);

    /*
    create thread pool
    */
    zv_threadpool_t *tp = threadpool_init(cf.thread_num);
    
    /* epoll_wait loop */
    while (1) {
        log_info("ready to wait");
        fflush(stdout);
        int n;
        n = zv_epoll_wait(epfd, events, MAXEVENTS, -1);
        
        int i, fd;
        for (i=0; i<n; i++) {
            zv_http_request_t *r = (zv_http_request_t *)events[i].data.ptr;
            fd = r->fd;
            
            if ((events[i].events & EPOLLERR) ||
                (events[i].events & EPOLLHUP) ||
                (!(events[i].events & EPOLLIN))) {
                log_err("epoll error %d", r->fd);
                close(fd);
                continue;
            }
            
            if (listenfd == fd) {
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
                    log_info("new connection fd %d", infd);
                    
                    zv_http_request_t *request = (zv_http_request_t *)malloc(sizeof(zv_http_request_t));
                    zv_init_request_t(request, infd, &cf);
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
                log_info("new data from fd %d", fd);
                rc = threadpool_add(tp, do_request, events[i].data.ptr);
                 
            }
        }   //end of for
    }   // end of while(1)
    return 0;
}
