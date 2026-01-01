#include "orderbook.h"
#include <stdlib.h>
#include <stdio.h>

/* ---------- helpers ---------- */

static queue* get_or_create_level(hashmap* map, heap* h, uint32_t price)
{
    queue* q = hashmap_get(map, price);
    if (!q) {
        q = queue_create();
        hashmap_put(map, price, q);
        heap_push(h, price);
    }
    return q;
}

static void remove_level(hashmap* map, uint32_t price)
{
    queue* q = hashmap_get(map, price);
    if (q) {
        queue_destroy(q);
        hashmap_remove(map, price);
    }
}

/* ---------- trade callback ---------- */

void orderbook_set_trade_callback(orderbook* ob,
                                  trade_callback cb,
                                  void* ctx)
{
    ob->on_trade  = cb;
    ob->trade_ctx = ctx;
}

/* ---------- matching ---------- */

static void match_buy(orderbook* ob, order* buy)
{
    while (buy->qty > 0 && !heap_is_empty(ob->ask_prices)) {

        uint32_t best_ask = heap_top(ob->ask_prices);
        if (best_ask > buy->price)
            break;

        queue* q = hashmap_get(ob->asks, best_ask);
        if (!q || queue_is_empty(q)) {
            heap_pop(ob->ask_prices);
            remove_level(ob->asks, best_ask);
            continue;
        }

        order* sell = queue_peek(q);
        uint32_t traded = (buy->qty < sell->qty) ? buy->qty : sell->qty;

        buy->qty  -= traded;
        sell->qty -= traded;

        if (ob->on_trade)
            ob->on_trade(best_ask, traded, 'B', ob->trade_ctx);

        if (sell->qty == 0) {
            queue_pop(q);
            free(sell);
        }

        if (queue_is_empty(q)) {
            heap_pop(ob->ask_prices);
            remove_level(ob->asks, best_ask);
        }
    }
}

static void match_sell(orderbook* ob, order* sell)
{
    while (sell->qty > 0 && !heap_is_empty(ob->bid_prices)) {

        uint32_t best_bid = heap_top(ob->bid_prices);
        if (best_bid < sell->price)
            break;

        queue* q = hashmap_get(ob->bids, best_bid);
        if (!q || queue_is_empty(q)) {
            heap_pop(ob->bid_prices);
            remove_level(ob->bids, best_bid);
            continue;
        }

        order* buy = queue_peek(q);
        uint32_t traded = (sell->qty < buy->qty) ? sell->qty : buy->qty;

        sell->qty -= traded;
        buy->qty  -= traded;

        if (ob->on_trade)
            ob->on_trade(best_bid, traded, 'S', ob->trade_ctx);

        if (buy->qty == 0) {
            queue_pop(q);
            free(buy);
        }

        if (queue_is_empty(q)) {
            heap_pop(ob->bid_prices);
            remove_level(ob->bids, best_bid);
        }
    }
}

/* ---------- public API ---------- */

orderbook* orderbook_create(size_t capacity)
{
    orderbook* ob = calloc(1, sizeof(orderbook));
    if (!ob) return NULL;

    ob->bids        = hashmap_create(capacity);
    ob->asks        = hashmap_create(capacity);
    ob->bid_prices  = heap_create(capacity, MAX_HEAP);
    ob->ask_prices  = heap_create(capacity, MIN_HEAP);
    ob->order_index = hashmap_create(capacity);

    pthread_rwlock_init(&ob->lock, NULL);
    return ob;
}

void orderbook_add_limit(orderbook* ob, order* ord)
{
    pthread_rwlock_wrlock(&ob->lock);

    if (ord->side == BUY)
        match_buy(ob, ord);
    else
        match_sell(ob, ord);

    if (ord->qty > 0) {
        queue* q = get_or_create_level(
            (ord->side == BUY) ? ob->bids : ob->asks,
            (ord->side == BUY) ? ob->bid_prices : ob->ask_prices,
            ord->price
        );
        queue_push(q, ord);
    } else {
        free(ord);
    }

    pthread_rwlock_unlock(&ob->lock);
}

/* 🔴 THIS WAS MISSING OR MISMATCHED BEFORE */
void orderbook_add_market(orderbook* ob, order* ord)
{
    pthread_rwlock_wrlock(&ob->lock);

    if (ord->side == BUY) {
        ord->price = UINT32_MAX;
        match_buy(ob, ord);
    } else {
        ord->price = 0;
        match_sell(ob, ord);
    }

    free(ord);
    pthread_rwlock_unlock(&ob->lock);
}

uint32_t orderbook_best_bid(orderbook* ob)
{
    pthread_rwlock_rdlock(&ob->lock);
    uint32_t p = heap_is_empty(ob->bid_prices) ? 0 : heap_top(ob->bid_prices);
    pthread_rwlock_unlock(&ob->lock);
    return p;
}

uint32_t orderbook_best_ask(orderbook* ob)
{
    pthread_rwlock_rdlock(&ob->lock);
    uint32_t p = heap_is_empty(ob->ask_prices) ? 0 : heap_top(ob->ask_prices);
    pthread_rwlock_unlock(&ob->lock);
    return p;
}

void orderbook_destroy(orderbook* ob)
{
    if (!ob) return;

    pthread_rwlock_destroy(&ob->lock);
    hashmap_destroy(ob->bids);
    hashmap_destroy(ob->asks);
    heap_destroy(ob->bid_prices);
    heap_destroy(ob->ask_prices);
    hashmap_destroy(ob->order_index);
    free(ob);
}

