
/*
 * Copyright (C) Zhu Jiashun
 * Copyright (C) Zaver
 */

#ifndef _HTTP_H
#define _HTTP_H

#include <strings.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "rio.h"
#include "list.h"
#include "dbg.h"
#include "util.h"
#include "http_request.h"
   
#define MAXLINE     8192
#define SHORTLINE   512

#define zv_str3_cmp(m, c0, c1, c2, c3)                                       \
    *(uint32_t *) m == ((c3 << 24) | (c2 << 16) | (c1 << 8) | c0)
#define zv_str3Ocmp(m, c0, c1, c2, c3)                                       \
    *(uint32_t *) m == ((c3 << 24) | (c2 << 16) | (c1 << 8) | c0)

#define zv_str4cmp(m, c0, c1, c2, c3)                                        \
    *(uint32_t *) m == ((c3 << 24) | (c2 << 16) | (c1 << 8) | c0)


typedef struct mime_type_s {
	const char *type;
	const char *value;
} mime_type_t;

extern void do_request(void *infd);

#endif
