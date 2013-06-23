#pragma once

#include <stddef.h>

/* returns pid */
int co_run (int (*func) (void *param), void *param, size_t stack_size);

/* returns retval of the last task */
int co_schedule (void);

void co_sleep (unsigned long time_us);

int co_getpid (void);

/* returns retval of the task */
int co_wait (int pid);

void co_exit (int ret);

void co_set_priority (int pri);
