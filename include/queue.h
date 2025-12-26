#ifndef QUEUE_H
#define QUEUE_H

#include <stddef.h>

/* forward declaration */
typedef struct order order;

typedef struct queue_node {
    order* ord;
    struct queue_node* prev;
    struct queue_node* next;
} queue_node;

typedef struct queue {
    queue_node* head;
    queue_node* tail;
    size_t size;
} queue;

queue*      queue_create(void);
queue_node* queue_push(queue* q, order* ord);
order*      queue_peek(queue* q);
order*      queue_pop(queue* q);

void        queue_remove_node(queue* q, queue_node* node);

int         queue_is_empty(queue* q);
void        queue_destroy(queue* q);

#endif
