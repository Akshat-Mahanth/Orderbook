#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdint.h>
#include <stddef.h>

typedef struct hashmap_node {
    uint32_t key;
    void* value;
    struct hashmap_node* next;
} hashmap_node;

typedef struct hashmap {
    hashmap_node** buckets;
    size_t capacity;
    size_t size;
} hashmap;

hashmap* hashmap_create(size_t capacity);
void hashmap_put(hashmap* map, uint32_t key, void* value);
void* hashmap_get(hashmap* map, uint32_t key);
void hashmap_remove(hashmap* map, uint32_t key);
void hashmap_destroy(hashmap* map);

#endif