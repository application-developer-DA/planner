#include "planner.h"

#include "scheduler.h"
#include "sysdep.h"
#include "task.h"

#include <setjmp.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

struct planner {
    bool                stack_grows_down;
    jmp_buf             main_ctx;
    struct task         *curtask;
    struct scheduler    *sched;
    int                 retval;
    struct task         *all_tasks[MAX_TASKS];
};

struct planner *planner = NULL;

static void
plan (void)
{
    while (1) {
#ifdef _DEBUG
        printf("i am the planner\n");
#endif
        if (planner->curtask) {
            if (! planner->curtask->zombie)
                sched_push_task (planner->sched, planner->curtask);
        }
        planner->curtask = sched_pop_task (planner->sched);
        if (! planner->curtask)
            return;
#ifdef _DEBUG
        printf("sleeping for task %i\n", planner->curtask->id);
#endif
        system_sleep (planner->curtask->pri.deadline - system_get_time ());
#ifdef _DEBUG
        printf("switching to task %i\n", planner->curtask->id);
        printf("stack %p - %p, SP %p\n", planner->curtask->stack,
                planner->curtask->stack + planner->curtask->stack_size,
                SP(planner->curtask->ctx));
#endif
        if (setjmp (planner->main_ctx) == 0)
            longjmp (planner->curtask->ctx, 1);
    }
}

static void
ensure_planner (void)
{
    if (planner)
        return;
    planner = (struct planner *) calloc (1, sizeof (struct planner));
    planner->sched = sched_new ();
}

static struct task *
create_task (size_t stack_size)
{
    int id;
    struct task *task; 

    ensure_planner ();

    /* find unused task id */
    for (id = 0; id < MAX_TASKS; ++id) {
        if (planner->all_tasks[id] == NULL)
            break;
    }
    if (id >= MAX_TASKS)
        return NULL;

    task = calloc (1, sizeof (struct task) + stack_size);

    task->id = id + 1;

    task->stack_size = stack_size;

    planner->all_tasks[id] = task;

    return task;
}


void
co_sleep (unsigned long time_us)
{
    if (! planner || ! planner->curtask)
        return;
    planner->curtask->pri.deadline = system_get_time () + time_us;
    if (setjmp (planner->curtask->ctx) == 0)
        longjmp (planner->main_ctx, 1);
}

void
co_exit (int ret)
{
    if (! planner || ! planner->curtask)
        return;
#ifdef _DEBUG
    printf("end task %i (%i)\n", planner->curtask->id, ret);
#endif
    planner->curtask->zombie = true;
    planner->retval = planner->curtask->retval = ret;
    if (setjmp (planner->curtask->ctx) == 0)
        longjmp (planner->main_ctx, 1);
    fprintf (stderr, "ERROR: trying to switch to dead task!\n");
    longjmp (planner->main_ctx, 1);
}

int
co_getpid (void)
{
    if (! planner || ! planner->curtask)
        return 0;
    return planner->curtask->id;
}

static void
trampoline (struct task *task, jmp_buf retbuf,
            int (*func) (void *parm), void *parm)
{
    /* update context and return immediately */
    if (setjmp (task->ctx) == 0)
        longjmp (retbuf, 1);
    task->retval = func (parm);
    co_exit (task->retval);
    /* this function should not attempt to return */
}

int
co_run (int (*func) (void *parm), void *parm, size_t stack_size)
{
    uint8_t *mainstack; /* must be first */

    struct task *newtask;
    bool stack_grows_down;
    jmp_buf tmp_ctx;

    mainstack = (uint8_t *) &mainstack;

    ensure_planner ();
    newtask = create_task (stack_size);
    if (! newtask)
        return 0;

    if (setjmp (newtask->ctx) != 0) {
        trampoline (newtask, tmp_ctx, func, parm); /* never returns */
    }

    stack_grows_down = ((intptr_t) SP(newtask->ctx) < (intptr_t) mainstack);

    if (stack_grows_down) {
        size_t copysize = (intptr_t) mainstack
                        - (intptr_t) SP(newtask->ctx);
        memcpy (newtask->stack + newtask->stack_size - copysize,
                (uint8_t *) SP(newtask->ctx),
                copysize);
        SP(newtask->ctx) = (intptr_t) newtask->stack
                         + newtask->stack_size - copysize;
    } else {
        size_t copysize =
                ((intptr_t) SP(newtask->ctx) - (intptr_t) mainstack);
        memcpy (newtask->stack, mainstack, copysize);
        SP(newtask->ctx) = (intptr_t) newtask->stack + copysize;
    }

    /* fire the trampoline to save func and parm on task's stack now */
    if (setjmp (tmp_ctx) == 0)
        longjmp (newtask->ctx, 1);

    sched_push_task (planner->sched, newtask);

    return newtask->id;
}

int
co_schedule (void)
{
    int i;
    int retval;
    if (! planner)
        return -1;
    plan ();
#ifdef DEBUG
    printf ("all tasks ended\n");
#endif
    retval = planner->retval;
    sched_free (planner->sched);
    /* kill zombies, if any */
    for (i = 0; i < MAX_TASKS; ++i) {
        if (planner->all_tasks[i]) {
#ifdef DEBUG
            printf ("killing zombie %i\n", i+1);
#endif
            free (planner->all_tasks[i]);
        }
    }
    free (planner);
    return (retval);
}

int
co_wait (int pid)
{
    int retval;
    struct task *task;
    if (! planner)
        return -1;
    if (pid <= 0 || pid >= MAX_TASKS)
        return -1;
    task = planner->all_tasks[pid - 1];
    if (! task)
        return -1;
    while (! task->zombie)
        co_sleep (0);
    retval = task->retval;
    free (task);
    planner->all_tasks[pid - 1] = NULL;
    return retval;
}

void
co_set_priority (int pri)
{
    if (! planner || ! planner->curtask)
        return;
    planner->curtask->pri.priority = pri;
}
