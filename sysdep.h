#pragma once

#include <stdint.h>

/* 32-bit linux */
/* #define SP(x) ((x)[0].__jmpbuf[4]) */

/* 32-bit Windows (mingw32) */
#define SP(x) ((x)[4])

uint64_t system_get_time (void);
void system_sleep (int64_t time_us);

