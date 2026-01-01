#ifndef ORDERBOOK_H
#define ORDERBOOK_H
#define _POSIX_C_SOURCE 200809L

#include <pthread.h> 
#include <stdint.h>
#include "order.h"
#include "hashmap.h"
#include "queue.h"
#include "heap.h"
#include "order_location.h"
#include "trade_event.h"   

#pragma once
#include <pthread.h>

typedef struct orderbook orderbook;
typedef void (*trade_callback)(uint32_t, uint32_t, char, void *);

struct orderbook {
    hashmap *bids;
    hashmap *asks;
    heap    *bid_prices;
    heap    *ask_prices;
    hashmap *order_index;

    trade_callback on_trade;
    void *trade_ctx;

    pthread_rwlock_t lock;
};
/* lifecycle */
orderbook* orderbook_create(size_t capacity);
void       orderbook_destroy(orderbook* ob);

/* order entry */
void orderbook_add_limit(orderbook* ob, order* ord);
void orderbook_add_market(orderbook* ob, order* ord);

/* cancel / modify */
int orderbook_cancel(orderbook* ob, uint32_t order_id);
int orderbook_modify(orderbook* ob, uint32_t order_id,
                     uint32_t new_price, uint32_t new_qty);

/* best prices */
uint32_t orderbook_best_bid(orderbook* ob);
uint32_t orderbook_best_ask(orderbook* ob);

/* trade callback registration */
void orderbook_set_trade_callback(
    orderbook* ob,
    trade_callback cb,
    void* ctx
);

#endif
