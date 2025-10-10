/**
 * @file err.c
 * @brief Definition of error handling functions for the synchronization protocol.
 */

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "err.h"

noreturn void syserr(const char* fmt, ...) {
    va_list fmt_args;
    int org_errno = errno;

    fprintf(stderr, "\tERROR: ");

    va_start(fmt_args, fmt);
    vfprintf(stderr, fmt, fmt_args);
    va_end(fmt_args);

    fprintf(stderr, " (%d; %s)\n", org_errno, strerror(org_errno));
    exit(1);
}

noreturn void fatal(const char* fmt, ...) {
    va_list fmt_args;

    fprintf(stderr, "\tERROR: ");

    va_start(fmt_args, fmt);
    vfprintf(stderr, fmt, fmt_args);
    va_end(fmt_args);

    fprintf(stderr, "\n");
    exit(1);
}

void error(const char* fmt, ...) {
    va_list fmt_args;
    int org_errno = errno;

    fprintf(stderr, "\tERROR: ");

    va_start(fmt_args, fmt);
    vfprintf(stderr, fmt, fmt_args);
    va_end(fmt_args);

    if (org_errno != 0) {
        fprintf(stderr, " (%d; %s)", org_errno, strerror(org_errno));
    }
    fprintf(stderr, "\n");
}

void error_msg(const uint8_t *msg, size_t len) {
    fprintf(stderr, "ERROR MSG ");
    size_t max = len < 10 ? len : 10;

    for (size_t i = 0; i < max; ++i) {
        fprintf(stderr, "%02x", msg[i]);
    }

    fprintf(stderr, "\n");
}
