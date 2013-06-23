#include "scheduler.h"

#include "task.h"

#include <stdlib.h>


struct scheduler {
    unsigned ntask;
    struct task *tasks[MAX_TASKS];
};


struct scheduler *
sched_new (void)
{
    struct scheduler *sched = calloc (1, sizeof (struct scheduler));
    sched->ntask = 0;
    return sched;
}

void
sched_free (struct scheduler *sched)
{
    free (sched);
}


static bool
less (const struct priority *a, const struct priority *b)
{
    if (a->deadline < b->deadline)
        return true;
    if (a->deadline > b->deadline)
        return false;
    if (a->priority > b->priority)
        return true;
    return false;
}


void
sched_push_task (struct scheduler *sched, struct task *task)
{
    unsigned pos = sched->ntask;

    ++sched->ntask;

    while (pos != 0) {
        unsigned parent = (pos - 1) >> 1;
        if (less(&sched->tasks[parent]->pri, &task->pri))
            break;
        sched->tasks[pos] = sched->tasks[parent];
        pos = parent;
    }

    sched->tasks[pos] = task;
}

struct task *
sched_pop_task (struct scheduler *sched)
{
    unsigned pos = 0;
    struct task *ret;
    if (! sched->ntask)
        return NULL;

    ret = sched->tasks[0];

    --sched->ntask;

    while (true) {
        unsigned child = (pos << 1) + 1;
        if (child >= sched->ntask)
            break;
        if (child < sched->ntask - 1) {
            if (less(&sched->tasks[child + 1]->pri, &sched->tasks[child]->pri))
                ++child;
        }
        if (less(&sched->tasks[sched->ntask]->pri, &sched->tasks[child]->pri))
            break;
        sched->tasks[pos] = sched->tasks[child];
        pos = child;
    }

    sched->tasks[pos] = sched->tasks[sched->ntask];

    return ret;
}
