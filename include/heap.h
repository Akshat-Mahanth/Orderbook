#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>
#include <stddef.h>

typedef enum {
    MIN_HEAP,
    MAX_HEAP
} heap_type;

typedef struct heap {
    uint32_t* data;
    size_t size;
    size_t capacity;
    heap_type type;
} heap;

heap*    heap_create(size_t capacity, heap_type type);
void     heap_push(heap* h, uint32_t price);
uint32_t heap_top(heap* h);
void     heap_pop(heap* h);
int      heap_is_empty(heap* h);
void     heap_destroy(heap* h);

#endif 
