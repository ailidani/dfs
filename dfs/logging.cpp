

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <unistd.h>
#include <fcntl.h>

#include "logging.h"

FILE *logfd = NULL;

void ori_fuse_log_enable()
{
    if (logfd == NULL) {
        logfd = fopen("fuse.log", "wb+");
        if (logfd == NULL) {
            perror("fopen");
            exit(1);
        } 
    }
}

void ori_fuse_log(const char *what, ...)
{
    va_list vl;
    va_start(vl, what);

    if (logfd == NULL) {
        return;
    }

    vfprintf(logfd, what, vl);

#ifdef DEBUG
    fflush(logfd);
#endif
}

