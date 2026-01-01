#ifndef ORDER_INTENT_H
#define ORDER_INTENT_H

#include <stdint.h>
#include "order.h"

typedef struct {
    int          agent_id;
    int          asset_id;   /* NEW */
    order_side  side;
    order_type  type;
    uint32_t    price;
    uint32_t    qty;
} OrderIntent;


#endif

