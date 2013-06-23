#include "sysdep.h"

#include <stddef.h>
#include <stdint.h>

#include <sys/time.h>
#include <windows.h>

#include <stdio.h>

uint64_t
system_get_time (void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64_t usec = tv.tv_usec + 1000000L * (uint64_t)tv.tv_sec;
    return usec;
}


void
system_sleep (int64_t time_us)
{
    time_us /= 1000;
    if (time_us > 0)
        Sleep (time_us);
}
