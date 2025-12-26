#ifndef ORDER_LOCATION_H
#define ORDER_LOCATION_H

#include <stdint.h>
#include "order.h"
#include "queue.h"

typedef struct order_location {
    uint32_t     price;
    order_side   side;
    queue_node*  node;
} order_location;

#endif /* ORDER_LOCATION_H */
