#include "orderbook.h"
#include <stdlib.h>
#include <stdio.h>

/* ---------- helpers ---------- */

static queue* get_or_create_level(hashmap* map, heap* h, uint32_t price) {
    queue* q = hashmap_get(map, price);
    if (!q) {
        q = queue_create();
        hashmap_put(map, price, q);
        heap_push(h, price);
    }
    return q;
}

static void remove_level(hashmap* map, uint32_t price) {
    queue* q = hashmap_get(map, price);
    if (q) {
        queue_destroy(q);
        hashmap_remove(map, price);
    }
}

static int can_fully_fill(orderbook* ob, order* ord) {
    uint32_t remaining = ord->qty;

    if (ord->side == BUY) {
        heap* prices = ob->ask_prices;
        hashmap* book = ob->asks;

        for (size_t i = 0; i < prices->size && remaining > 0; i++) {
            uint32_t price = prices->data[i];
            if (price > ord->price && ord->type != MARKET)
                continue;

            queue* q = hashmap_get(book, price);
            for (queue_node* n = q->head; n && remaining > 0; n = n->next) {
                if (n->ord->qty >= remaining)
                    return 1;
                remaining -= n->ord->qty;
            }
        }
    } else { /* SELL */
        heap* prices = ob->bid_prices;
        hashmap* book = ob->bids;

        for (size_t i = 0; i < prices->size && remaining > 0; i++) {
            uint32_t price = prices->data[i];
            if (price < ord->price && ord->type != MARKET)
                continue;

            queue* q = hashmap_get(book, price);
            for (queue_node* n = q->head; n && remaining > 0; n = n->next) {
                if (n->ord->qty >= remaining)
                    return 1;
                remaining -= n->ord->qty;
            }
        }
    }
    return remaining == 0;
}

void orderbook_set_trade_callback(
    orderbook* ob,
    trade_callback cb,
    void* ctx
) {
    ob->on_trade = cb;
    ob->trade_ctx = ctx;
}


/* ---------- matching ---------- */

static void match_buy(orderbook* ob, order* buy) {
    while (buy->qty > 0 && !heap_is_empty(ob->ask_prices)) {

        uint32_t best_ask = heap_top(ob->ask_prices);
        if (best_ask > buy->price)
            break;

        queue* q = hashmap_get(ob->asks, best_ask);
        if (!q || queue_is_empty(q)) {
            /* stale price level */
            heap_pop(ob->ask_prices);
            remove_level(ob->asks, best_ask);
            continue;
        }

        order* sell = queue_peek(q);
        if (!sell)
            break;

        uint32_t traded = (buy->qty < sell->qty) ? buy->qty : sell->qty;

        buy->qty  -= traded;
        sell->qty -= traded;

        if (ob->on_trade)
            ob->on_trade(best_ask, traded, 'B', ob->trade_ctx);

        printf("TRADE: %u @ %u\n", traded, best_ask);

        /* fully filled resting order */
        if (sell->qty == 0) {

            /* remove from order index */
            order_location* loc =
                hashmap_get(ob->order_index, sell->id);
            if (loc) {
                hashmap_remove(ob->order_index, sell->id);
                free(loc);
            }

            /* REMOVE HEAD ONLY */
            queue_pop(q);
            free(sell);
        }

        /* clean empty level */
        if (queue_is_empty(q)) {
            heap_pop(ob->ask_prices);
            remove_level(ob->asks, best_ask);
        }
    }
}

static void match_sell(orderbook* ob, order* sell) {
    while (sell->qty > 0 && !heap_is_empty(ob->bid_prices)) {

        uint32_t best_bid = heap_top(ob->bid_prices);
        if (best_bid < sell->price)
            break;

        queue* q = hashmap_get(ob->bids, best_bid);
        if (!q || queue_is_empty(q)) {
            /* stale price level */
            heap_pop(ob->bid_prices);
            remove_level(ob->bids, best_bid);
            continue;
        }

        order* buy = queue_peek(q);
        if (!buy)
            break;

        uint32_t traded = (sell->qty < buy->qty) ? sell->qty : buy->qty;

        sell->qty -= traded;
        buy->qty  -= traded;

        if (ob->on_trade)
            ob->on_trade(best_bid, traded, 'S', ob->trade_ctx);

        printf("TRADE: %u @ %u\n", traded, best_bid);

        /* fully filled resting order */
        if (buy->qty == 0) {

            /* remove from order index */
            order_location* loc =
                hashmap_get(ob->order_index, buy->id);
            if (loc) {
                hashmap_remove(ob->order_index, buy->id);
                free(loc);
            }

            /* REMOVE HEAD ONLY */
            queue_pop(q);
            free(buy);
        }

        /* clean empty level */
        if (queue_is_empty(q)) {
            heap_pop(ob->bid_prices);
            remove_level(ob->bids, best_bid);
        }
    }
}


/* ---------- public API ---------- */

orderbook* orderbook_create(size_t capacity) {
    orderbook* ob = malloc(sizeof(orderbook));
    if (!ob) return NULL;

    ob->bids = hashmap_create(capacity);
    ob->asks = hashmap_create(capacity);
    ob->bid_prices = heap_create(capacity, MAX_HEAP);
    ob->ask_prices = heap_create(capacity, MIN_HEAP);
    ob->order_index = hashmap_create(capacity);
    return ob;
}
void orderbook_add_limit(orderbook* ob, order* ord) {

    /* FOK pre-check */
    if (ord->type == FOK) {
        if (!can_fully_fill(ob, ord)) {
            free(ord);
            return;
        }
    }

    if (ord->side == BUY) {
        match_buy(ob, ord);
    } else {
        match_sell(ob, ord);
    }

    /* IOC & FOK never rest */
    if (ord->type == IOC || ord->type == FOK) {
        free(ord);
        return;
    }

    /* LIMIT: rest remaining */
    if (ord->qty > 0) {
        queue* q = get_or_create_level(
            (ord->side == BUY) ? ob->bids : ob->asks,
            (ord->side == BUY) ? ob->bid_prices : ob->ask_prices,
            ord->price
        );

        queue_node* node = queue_push(q, ord);

        order_location* loc = malloc(sizeof(order_location));
        loc->price = ord->price;
        loc->side  = ord->side;
        loc->node  = node;

        hashmap_put(ob->order_index, ord->id, loc);
    } else {
        free(ord);
    }
}


void orderbook_add_market(orderbook* ob, order* ord) {
    if (ord->side == BUY) {
        ord->price = UINT32_MAX;
        match_buy(ob, ord);
    } else {
        ord->price = 0;
        match_sell(ob, ord);
    }
    free(ord);
}

uint32_t orderbook_best_bid(orderbook* ob) {
    return heap_is_empty(ob->bid_prices) ? 0 : heap_top(ob->bid_prices);
}

uint32_t orderbook_best_ask(orderbook* ob) {
    return heap_is_empty(ob->ask_prices) ? 0 : heap_top(ob->ask_prices);
}

int orderbook_cancel(orderbook* ob, uint32_t order_id) {
    order_location* loc = hashmap_get(ob->order_index, order_id);
    if (!loc)
        return 0;   /* already gone */

    hashmap* book = (loc->side == BUY) ? ob->bids : ob->asks;
    queue* q = hashmap_get(book, loc->price);
    if (!q) {
        hashmap_remove(ob->order_index, order_id);
        free(loc);
        return 0;
    }

    if (loc->node)
        queue_remove_node(q, loc->node);

    hashmap_remove(ob->order_index, order_id);
    free(loc);
    return 1;
}




int orderbook_modify(
    orderbook* ob,
    uint32_t order_id,
    uint32_t new_price,
    uint32_t new_qty
) {
    order_location* loc = hashmap_get(ob->order_index, order_id);
    if (!loc)
        return 0;

    /* copy old order */
    order* old = loc->node->ord;

    order* new_ord = malloc(sizeof(order));
    if (!new_ord)
        return 0;

    *new_ord = *old;
    new_ord->price = new_price;
    new_ord->qty   = new_qty;

    /* cancel old */
    orderbook_cancel(ob, order_id);

    /* reinsert as limit order */
    orderbook_add_limit(ob, new_ord);

    return 1;
}



void orderbook_destroy(orderbook* ob) {
    if (!ob) return;

    hashmap_destroy(ob->bids);
    hashmap_destroy(ob->asks);
    heap_destroy(ob->bid_prices);
    heap_destroy(ob->ask_prices);
    hashmap_destroy(ob->order_index);
    free(ob);
}
