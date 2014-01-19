#include "planner.h"

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

static int
task (void *ptr)
{
    int i;

    printf ("i'm task %li with pid %i\n", (long)ptr, co_getpid ());

    for (i = 0; i < 10; ++i) {
        co_sleep (10000);
        printf("iter %i: task %li pid %i\n", i, (long)ptr, co_getpid ());
    }

    printf ("exiting %li %i\n", (long)ptr, co_getpid ());

    return 7 + (long)ptr;
}

static int
task2 (void *ptr)
{
    (void) ptr;
    printf ("i'm forking task\n");
    co_run (task, (void *) 99, 1<<20);
    co_sleep (100000);
    printf ("ok\n");
    return 0;
}

static int
waiter (void *ptr)
{
    int ret = co_wait ((int)ptr);
    printf ("task %i returned %i\n", (int)ptr, ret);
    return ret;
}

/******************/
static int thread1()
{
    bool direction = true;
    int x = 60;
    int y = 100;
    while (1) {
        printf("%d %d %d\n", 0, x, y);
        fflush(stdout);
        if (x == 60)
            direction = true;
        if (x == 500)
            direction = false;
        x = (direction == true) ? (x + 1) : (x - 1);
        co_sleep (300000);
    }
    return 0;
}

static int thread2()
{
    bool direction = true;
    int x = 60;
    int y = 200;
    while (1) {
        printf("%d %d %d\n", 1, x, y);
        fflush(stdout);
        if (x == 60)
            direction = true;
        if (x == 498/*500*/)
            direction = false;
        x = (direction == true) ? (x + 3) : (x - 3);
        co_sleep (300000);
    }
    return 0;
}

static int thread3()
{
    bool direction = true;
    int x = 60;
    int y = 300;
    while (1) {
        printf("%d %d %d\n", 2, x, y);
        fflush(stdout);
        if (x == 60)
            direction = true;
        if (x == 500)
            direction = false;
        x = (direction == true) ? (x + 5) : (x - 5);
        co_sleep (300000);
    }
    return 0;
}

static int thread4()
{
    bool direction = true;
    int x = 60;
    int y = 400;
    while (1) {
        printf("%d %d %d\n", 3, x, y);
        fflush(stdout);
        if (x == 60)
            direction = true;
        if (x == 500)
            direction = false;
        x = (direction == true) ? (x + 1) : (x - 1);
        co_sleep (300000);
    }
    return 0;
}

/******************/

#ifdef ANOTHER_EXAMPLE
static int thread1()
{
    while (1) {
        printf("%d %c \n", 1, 'a');
        fflush(stdout);
        co_sleep (100000);
    }
    return 0;
}

static int thread2()
{
    while (1) {
        printf("%d %c \n", 2, 'b');
        fflush(stdout);
        co_sleep (100000);
    }
    return 0;
}

static int thread3()
{
    while (1) {
        printf("%d %c \n", 3, 'c');
        fflush(stdout);
        co_sleep (100000);
    }
    return 0;
}

static int thread4()
{
    while (1) {
        printf("%d %c \n", 4, 'd');
        fflush(stdout);
        co_sleep (100000);
    }
    return 0;
}
#endif

int
main (int argc, char **argv)
{
	#ifdef ANOTHER_EXAMPLE
    int pid;
    co_run (task, (void*) 1, 1<<20);
    co_run (task, (void*) 2, 1<<20);
    pid = co_run (task, (void*) 3, 1<<20);
    co_run (task2, NULL, 1<<20);
    co_run (waiter, (void*)pid, 1<<20);
    return co_schedule ();
    #endif
	
    co_run (thread1, NULL, 1<<20);
    co_run (thread2, NULL, 1<<20);
    co_run (thread3, NULL, 1<<20);
    co_run (thread4, NULL, 1<<20);
    return co_schedule ();
}
