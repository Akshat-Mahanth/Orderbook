#ifndef L3_SNAPSHOT_H
#define L3_SNAPSHOT_H

#include <stdint.h>

#define L3_MAX_ORDERS 64

typedef struct {
    uint32_t order_id;
    uint32_t price;
    uint32_t qty;
    uint8_t  side;   /* 'B' or 'S' */
    uint8_t  _pad[3];
} L3Order;

typedef struct {
    uint32_t bid_count;
    uint32_t ask_count;

    L3Order bids[L3_MAX_ORDERS];
    L3Order asks[L3_MAX_ORDERS];
} L3Snapshot;

#endif

