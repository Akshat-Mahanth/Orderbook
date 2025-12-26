#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include <stdint.h>
#include "order.h"
#include "hashmap.h"
#include "queue.h"
#include "heap.h"
#include "order_location.h"

typedef struct orderbook {
    hashmap* bids;
    hashmap* asks;
    heap*    bid_prices;
    heap*    ask_prices;

    hashmap* order_index;  
} orderbook;

int orderbook_modify(
    orderbook* ob,
    uint64_t order_id,
    uint32_t new_price,
    uint32_t new_qty
);

orderbook* orderbook_create(size_t capacity);
void       orderbook_destroy(orderbook* ob);

void orderbook_add_limit(orderbook* ob, order* ord);
void orderbook_add_market(orderbook* ob, order* ord);

uint32_t orderbook_best_bid(orderbook* ob);
uint32_t orderbook_best_ask(orderbook* ob);

int orderbook_cancel(orderbook* ob, uint64_t order_id);

#endif /* ORDERBOOK_H */
