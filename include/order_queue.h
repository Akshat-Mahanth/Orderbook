#ifndef ORDER_QUEUE_H
#define ORDER_QUEUE_H

#include <pthread.h>
#include <stddef.h>
#include "order_intent.h"

typedef struct order_node {
    OrderIntent intent;
    struct order_node *next;
} order_node;

typedef struct order_queue {
    order_node *head;
    order_node *tail;
    size_t size;
    size_t max_size;

    int running;                 /* <-- REQUIRED */

    pthread_mutex_t lock;
    pthread_cond_t  not_empty;
    pthread_cond_t  not_full;
} order_queue;

/* lifecycle */
void order_queue_init(order_queue *q, size_t max_size);
void order_queue_destroy(order_queue *q);

/* operations */
void order_queue_push(order_queue *q, const OrderIntent *intent);
int  order_queue_pop(order_queue *q, OrderIntent *out);

/* shutdown support */
void order_queue_wake_all(order_queue *q);

#endif

