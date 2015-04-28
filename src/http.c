#include "http.h"

mime_type_t zaver_mime[] = 
{
    {".html", "text/html"},
    {".xml", "text/xml"},
    {".xhtml", "application/xhtml+xml"},
    {".txt", "text/plain"},
    {".rtf", "application/rtf"},
    {".pdf", "application/pdf"},
    {".word", "application/msword"},
    {".png", "image/png"},
    {".gif", "image/gif"},
    {".jpg", "image/jpeg"},
    {".jpeg", "image/jpeg"},
    {".au", "audio/basic"},
    {".mpeg", "video/mpeg"},
    {".mpg", "video/mpeg"},
    {".avi", "video/x-msvideo"},
    {".gz", "application/x-gzip"},
    {".tar", "application/x-tar"},
    {".css", "text/css"},
    {NULL ,"text/plain"}
};

void do_request(void *ptr) {
    zv_http_request_t *r = (zv_http_request_t *)ptr;
    int fd = r->fd;
    int rc;
    char filename[SHORTLINE];
    struct stat sbuf;
    int n;

    for(;;) {
        n = read(fd, r->last, (uint64_t)r->buf + MAX_BUF - (uint64_t)r->last);
        //log_info("has read %d, buffer remaining: %d, buffer rece:%s", n, (uint64_t)r->buf + MAX_BUF - (uint64_t)r->last, r->buf);

        if (n == 0) {   // EOF
            log_info("read return 0, ready to close fd %d", fd);
            goto err;
        }

        if (n < 0) {
            if (errno != EAGAIN) {
                log_err("read err");
                goto err;
            }
            break;
        }

        r->last += n;
        check(r->last <= r->buf + MAX_BUF, "r->last <= MAX_BUF");
        
        log_info("ready to parse request line"); 
        rc = zv_http_parse_request_line(r);
        if (rc == ZV_AGAIN) {
            continue;
        } else if (rc != ZV_OK) {
            log_err("rc != ZV_OK");
            goto err;
        }

        log_info("method == %.*s",r->method_end - r->request_start, r->request_start);
        log_info("uri == %.*s", r->uri_end - r->uri_start, r->uri_start);

        log_info("ready to parse request body");
        rc  = zv_http_parse_request_body(r);
        if (rc == ZV_AGAIN) {
            continue;
        } else if (rc != ZV_OK) {
            log_err("rc != ZV_OK");
            goto err;
        }
        
        /*
        *   handle http header
        */
        zv_http_out_t *out = (zv_http_out_t *)malloc(sizeof(zv_http_out_t));
        zx_http_handle_header(r, out);
        check(list_empty(&(r->list)) == 1, "header list should be empty");


        parse_uri(r->uri_start, r->uri_end - r->uri_start, filename, NULL);

        if(stat(filename, &sbuf) < 0) {
            do_error(fd, filename, "404", "Not Found", "zaver can't find the file");
            continue;
        }

        if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode))
        {
            do_error(fd, filename, "403", "Forbidden",
                    "zaver can't read the file");
            continue;
        }
        
        serve_static(fd, filename, sbuf.st_size, out);
        log_info("serve_static suc");

        fflush(stdout);

        free(out);

        if (out->keep_alive) {
            log_info("no keep_alive! ready to close");
            goto close;
        }
    }
    
    return;

err:
    log_info("err when serve fd %d, ready to close", fd);
close:
    close(fd);
}

void parse_uri(char *uri, int uri_length, char *filename, char *querystring) {
    strcpy(filename, root);
    strncat(filename, uri, uri_length);

    char *last_comp = rindex(filename, '/');
    char *last_dot = rindex(last_comp, '.');
    if (last_dot == NULL && filename[strlen(filename)-1] != '/') {
        strcat(filename, "/");
    }
    
    if(uri[strlen(uri)-1] == '/' || filename[strlen(filename)-1] == '/') {
        strcat(filename, "index.html");
    }

    log_info("filename = %s", filename);
    return;
}

void do_error(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg)
{
    char header[MAXLINE], body[MAXLINE];

    sprintf(body, "<html><title>Tiny Error</title>");
    sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
    sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
    sprintf(body, "%s<p>%s: %s\r\n</p>", body, longmsg, cause);
    sprintf(body, "%s<hr><em>The Tiny web server</em>\r\n", body);

    sprintf(header, "HTTP/1.1 %s %s\r\n", errnum, shortmsg);
    sprintf(header, "%sServer: Zaver\r\n", header);
    sprintf(header, "%sContent-type: text/html\r\n", header);
    sprintf(header, "%sConnection: close\r\n", header);
    sprintf(header, "%sContent-length: %d\r\n\r\n", header, (int)strlen(body));
    //log_info("header  = \n %s\n", header);
    rio_writen(fd, header, strlen(header));
    rio_writen(fd, body, strlen(body));
    log_info("leave clienterror\n");
    return;
}

void serve_static(int fd, char *filename, int filesize, zv_http_out_t *out) {
    log_info("filename = %s", filename);
    char header[MAXLINE];
    int n;
    
    const char *file_type;
    const char *dot_pos = rindex(filename, '.');
    file_type = get_file_type(dot_pos);

    sprintf(header, "HTTP/1.1 200 OK\r\n");
    sprintf(header, "%sServer: Zaver\r\n", header);
    sprintf(header, "%sContent-length: %d\r\n", header, filesize);
    if (out->keep_alive) {
        sprintf(header, "%sConnection: keep-alive\r\n", header);
    }
    sprintf(header, "%sContent-type: %s\r\n\r\n", header, file_type);
//    sprintf(header, "%sConnection: close\r\n\r\n", header);

    n = rio_writen(fd, header, strlen(header));
    check(n == strlen(header), "rio_writen error");

    int srcfd = open(filename, O_RDONLY, 0);
    check(srcfd > 2, "open error");
    // can use sendfile
    char *srcaddr = mmap(NULL, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);
    check(srcaddr > 0, "mmap error");
    close(srcfd);

    n = rio_writen(fd, srcaddr, filesize);
    check(n == filesize, "rio_writen error");

    munmap(srcaddr, filesize);
    return;
}

const char* get_file_type(const char *type)
{
    if (type == NULL) {
        return "text/plain";
    }

    int i;
    for (i = 0; zaver_mime[i].type != NULL; ++i) {
        if (strcmp(type, zaver_mime[i].type) == 0)
            return zaver_mime[i].value;
    }
    return zaver_mime[i].value;
}
