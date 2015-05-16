#ifndef _ERROR_H
#define _ERROR_H

void unix_error(char *msg) /* unix-style error */
{
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    /* exit(0); */
}

void posix_error(int code, char *msg) /* posix-style error */
{
    fprintf(stderr, "%s: %s\n", msg, strerror(code));
    /* exit(0); */
}

#endif
