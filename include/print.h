#ifndef PRINT_H
#define PRINT_H

#include "orderbook.h"

typedef enum {
    VIEW_L1,
    VIEW_L2
} book_view;

void print_orderbook(orderbook* ob, book_view view);

#endif
