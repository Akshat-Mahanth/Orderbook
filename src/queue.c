#include "queue.h"
#include <stdlib.h>

queue* queue_create(void) {
    queue* q = malloc(sizeof(queue));
    if (!q) return NULL;

    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    return q;
}

queue_node* queue_push(queue* q, order* ord) {
    if (!q || !ord) return NULL;

    queue_node* node = malloc(sizeof(queue_node));
    if (!node) return NULL;

    node->ord  = ord;
    node->next = NULL;
    node->prev = q->tail;

    if (q->tail)
        q->tail->next = node;
    else
        q->head = node;

    q->tail = node;
    q->size++;
    return node;
}

order* queue_peek(queue* q) {
    return (q && q->head) ? q->head->ord : NULL;
}

order* queue_pop(queue* q) {
    if (!q || !q->head) return NULL;

    queue_node* node = q->head;
    order* ord = node->ord;

    q->head = node->next;
    if (q->head)
        q->head->prev = NULL;
    else
        q->tail = NULL;

    free(node);
    q->size--;
    return ord;
}

/* NEW: O(1) arbitrary removal */
void queue_remove_node(queue* q, queue_node* node) {
    if (!q || !node) return;

    if (node->prev)
        node->prev->next = node->next;
    else
        q->head = node->next;

    if (node->next)
        node->next->prev = node->prev;
    else
        q->tail = node->prev;

    free(node);
    q->size--;
}

int queue_is_empty(queue* q) {
    return !q || q->size == 0;
}

void queue_destroy(queue* q) {
    if (!q) return;

    queue_node* cur = q->head;
    while (cur) {
        queue_node* tmp = cur;
        cur = cur->next;
        free(tmp);
    }
    free(q);
}
