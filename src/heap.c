#include "heap.h"
#include <stdlib.h>

static inline int cmp(heap* h, uint32_t a, uint32_t b) {
    if (h->type == MIN_HEAP)
        return a < b;
    else
        return a > b;
}

static void swap(uint32_t* a, uint32_t* b) {
    uint32_t tmp = *a;
    *a = *b;
    *b = tmp;
}

heap* heap_create(size_t capacity, heap_type type) {
    heap* h = malloc(sizeof(heap));
    if (!h) return NULL;

    h->data = malloc(capacity * sizeof(uint32_t));
    if (!h->data) {
        free(h);
        return NULL;
    }

    h->size = 0;
    h->capacity = capacity;
    h->type = type;
    return h;
}

void heap_push(heap* h, uint32_t price) {
    if (!h || h->size >= h->capacity)
        return;

    size_t i = h->size++;
    h->data[i] = price;

    /* Bubble up */
    while (i > 0) {
        size_t parent = (i - 1) / 2;
        if (cmp(h, h->data[i], h->data[parent])) {
            swap(&h->data[i], &h->data[parent]);
            i = parent;
        } else {
            break;
        }
    }
}

uint32_t heap_top(heap* h) {
    if (!h || h->size == 0)
        return 0;
    return h->data[0];
}

void heap_pop(heap* h) {
    if (!h || h->size == 0)
        return;

    h->data[0] = h->data[--h->size];

    /* Heapify down */
    size_t i = 0;
    while (1) {
        size_t left = 2 * i + 1;
        size_t right = 2 * i + 2;
        size_t best = i;

        if (left < h->size && cmp(h, h->data[left], h->data[best]))
            best = left;
        if (right < h->size && cmp(h, h->data[right], h->data[best]))
            best = right;

        if (best != i) {
            swap(&h->data[i], &h->data[best]);
            i = best;
        } else {
            break;
        }
    }
}

int heap_is_empty(heap* h) {
    return !h || h->size == 0;
}

void heap_destroy(heap* h) {
    if (!h) return;
    free(h->data);
    free(h);
}
