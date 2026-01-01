#define _POSIX_C_SOURCE 200809L
#include <time.h>
#include "agent_thread.h"
#include <time.h>
#include <stdlib.h>

static void *agent_loop(void *arg)
{
    agent_thread *t = arg;

    while (t->running) {
        t->agent->act(t->agent, t->queue);

        /* pacing: 10–50 ms */
        struct timespec ts;
        ts.tv_sec  = 0;
        ts.tv_nsec = (10 + rand() % 40) * 1000 * 1000;
        nanosleep(&ts, NULL);
    }
    return NULL;
}

void agent_thread_start(agent_thread *t)
{
    t->running = 1;
    pthread_create(&t->thread, NULL, agent_loop, t);
}

void agent_thread_stop(agent_thread *t)
{
    t->running = 0;
    pthread_join(t->thread, NULL);
}

