#include "hashmap.h"
#include <stdlib.h>

#define HASH_CONST 2654435761u

static inline uint32_t hash_u32(uint32_t key) {
    return key * HASH_CONST;
}

static int is_power_of_two(size_t x) {
    return x && ((x & (x - 1)) == 0);
}

hashmap* hashmap_create(size_t capacity) {
    if (!is_power_of_two(capacity))
        return NULL;

    hashmap* map = malloc(sizeof(hashmap));
    if (!map) return NULL;

    map->buckets = calloc(capacity, sizeof(hashmap_node*));
    if (!map->buckets) {
        free(map);
        return NULL;
    }

    map->capacity = capacity;
    map->size = 0;
    return map;
}

void hashmap_put(hashmap* map, uint32_t key, void* value) {
    uint32_t idx = hash_u32(key) & (map->capacity - 1);
    hashmap_node* node = map->buckets[idx];

    while (node) {
        if (node->key == key) {
            node->value = value;
            return;
        }
        node = node->next;
    }

    hashmap_node* new_node = malloc(sizeof(hashmap_node));
    if (!new_node) return;

    new_node->key = key;
    new_node->value = value;
    new_node->next = map->buckets[idx];
    map->buckets[idx] = new_node;
    map->size++;
}

void* hashmap_get(hashmap* map, uint32_t key) {
    uint32_t idx = hash_u32(key) & (map->capacity - 1);
    hashmap_node* node = map->buckets[idx];

    while (node) {
        if (node->key == key)
            return node->value;
        node = node->next;
    }
    return NULL;
}

void hashmap_remove(hashmap* map, uint32_t key) {
    uint32_t idx = hash_u32(key) & (map->capacity - 1);
    hashmap_node* node = map->buckets[idx];
    hashmap_node* prev = NULL;

    while (node) {
        if (node->key == key) {
            if (prev)
                prev->next = node->next;
            else
                map->buckets[idx] = node->next;

            free(node);
            map->size--;
            return;
        }
        prev = node;
        node = node->next;
    }
}

void hashmap_destroy(hashmap* map) {
    if (!map) return;

    for (size_t i = 0; i < map->capacity; i++) {
        hashmap_node* node = map->buckets[i];
        while (node) {
            hashmap_node* tmp = node;
            node = node->next;
            free(tmp);
        }
    }
    free(map->buckets);
    free(map);
}
