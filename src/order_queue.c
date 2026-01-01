#include "order_queue.h"
#include <stdlib.h>

void order_queue_init(order_queue *q, size_t max_size)
{
    q->head = q->tail = NULL;
    q->size = 0;
    q->max_size = max_size;
    q->running = 1;

    pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->not_empty, NULL);
    pthread_cond_init(&q->not_full, NULL);
}

void order_queue_push(order_queue *q, const OrderIntent *intent)
{
    pthread_mutex_lock(&q->lock);

    while (q->size >= q->max_size && q->running)
        pthread_cond_wait(&q->not_full, &q->lock);

    if (!q->running) {
        pthread_mutex_unlock(&q->lock);
        return;
    }

    order_node *n = malloc(sizeof(order_node));
    n->intent = *intent;
    n->next = NULL;

    if (q->tail)
        q->tail->next = n;
    else
        q->head = n;

    q->tail = n;
    q->size++;

    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->lock);
}

int order_queue_pop(order_queue *q, OrderIntent *out)
{
    pthread_mutex_lock(&q->lock);

    while (q->size == 0 && q->running)
        pthread_cond_wait(&q->not_empty, &q->lock);

    if (q->size == 0 && !q->running) {
        pthread_mutex_unlock(&q->lock);
        return 0;
    }

    order_node *n = q->head;
    q->head = n->next;
    if (!q->head)
        q->tail = NULL;

    *out = n->intent;
    free(n);

    q->size--;
    pthread_cond_signal(&q->not_full);
    pthread_mutex_unlock(&q->lock);
    return 1;
}

void order_queue_wake_all(order_queue *q)
{
    pthread_mutex_lock(&q->lock);
    q->running = 0;
    pthread_cond_broadcast(&q->not_empty);
    pthread_cond_broadcast(&q->not_full);
    pthread_mutex_unlock(&q->lock);
}

void order_queue_destroy(order_queue *q)
{
    order_queue_wake_all(q);
    pthread_mutex_destroy(&q->lock);
    pthread_cond_destroy(&q->not_empty);
    pthread_cond_destroy(&q->not_full);
}

