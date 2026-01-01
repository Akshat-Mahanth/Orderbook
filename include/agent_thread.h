#ifndef AGENT_THREAD_H
#define AGENT_THREAD_H

#include <pthread.h>
#include "agent.h"
#include "order_queue.h"

typedef struct {
    agent        *agent;
    order_queue  *queue;
    int           running;
    pthread_t     thread;
} agent_thread;

void agent_thread_start(agent_thread *t);
void agent_thread_stop(agent_thread *t);

#endif

