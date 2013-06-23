#pragma once

#include <setjmp.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_TASKS 1024

struct priority {
    int         priority;
    uint64_t    deadline;
};

struct task {
    struct priority pri;
    bool            zombie;
    int             id;
    jmp_buf         ctx;
    int             retval;
    size_t          stack_size;
    uint8_t         stack[];
};

