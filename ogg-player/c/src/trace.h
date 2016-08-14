#ifndef __TRACE_H
#define __TRACE_H

#include <stdio.h>
#include <unistd.h>

#define TRACE_ERROR(format, ...) \
    printf("[pid: %d] error: " format "\n", getpid(), ## __VA_ARGS__)

#define TRACE_WARNING(format, ...) \
    printf("[pid: %d] warning: " format "\n", getpid(), ## __VA_ARGS__)

#define TRACE_INFO(format, ...) \
    printf("[pid: %d] info: " format "\n", getpid(), ## __VA_ARGS__)

#ifdef DEBUG
#define TRACE_DEBUG(format, ...) \
    printf("[pid: %d][%s:%d] " format "\n", \
           getpid(), \
           __FUNCTION__, \
           __LINE__, \
           ## __VA_ARGS__)
#else
#define TRACE_DEBUG(format, ...)
#endif // DEBUG

#endif // __TRACE_H
