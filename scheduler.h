#pragma once

#include "task.h"

struct scheduler;

struct scheduler *sched_new (void);
void sched_free (struct scheduler *sched);

void sched_push_task (struct scheduler *sched, struct task *old);
struct task *sched_pop_task (struct scheduler *sched);
