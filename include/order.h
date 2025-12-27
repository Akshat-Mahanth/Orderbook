#ifndef ORDER_H
#define ORDER_H

#include <stdint.h>

typedef enum {
    BUY,
    SELL
} order_side;

typedef enum {
    LIMIT,
    MARKET,
    IOC,   // immediate_or_cancel 
    FOK    // fill_or_kill
} order_type;

typedef struct order {
    uint32_t   id;
    uint32_t   price;
    uint32_t   qty;
    order_side side;
    order_type  type; 
} order;

#endif
