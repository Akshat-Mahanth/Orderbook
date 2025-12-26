#ifndef DEPTH_H
#define DEPTH_H

#include <stdint.h>
#include <stddef.h>
#include "orderbook.h"

typedef struct depth_level {
    uint32_t price;
    uint32_t total_qty;
} depth_level;

/* side = BUY or SELL
 * returns number of levels written */
size_t orderbook_get_l2(
    orderbook* ob,
    order_side side,
    depth_level* out,
    size_t max_levels
);

#endif
