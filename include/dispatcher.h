#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <pthread.h>
#include "orderbook.h"
#include "order_queue.h"

typedef struct dispatcher {
    orderbook   **books;     /* array of pointers */
    order_queue *queue;      /* single global queue */
    int           num_assets;

    pthread_t     thread;
    int           running;
} dispatcher;

void dispatcher_start(dispatcher *d);
void dispatcher_stop(dispatcher *d);

#endif

