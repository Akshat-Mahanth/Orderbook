#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include <stdint.h>
#include "orderbook.h"

#define L2_DEPTH 10

#define MAX_TRADES 64
typedef struct {
    uint32_t price;
    uint32_t qty;
    uint64_t timestamp;
} TradeEvent;

typedef struct {
    uint32_t price;
    uint64_t qty;
} PriceLevel;

typedef struct {
    uint64_t timestamp;
    uint32_t best_bid;
    uint32_t best_ask;
    uint64_t best_bid_qty;
    uint64_t best_ask_qty;

    PriceLevel bids[L2_DEPTH];
    PriceLevel asks[L2_DEPTH];

    uint32_t bid_levels;
    uint32_t ask_levels;
    TradeEvent trades[MAX_TRADES];
    uint32_t   trade_count;
} MarketSnapshot;

/* build snapshot from existing orderbook state */
void build_snapshot(orderbook *ob, MarketSnapshot *snap);
void snapshot_trade_cb(
    uint32_t price,
    uint32_t qty,
    char side,
    void *ctx
);

#endif
