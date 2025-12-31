#include <stdio.h>
#include "print.h"
#include "depth.h"

#define RED   "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define RESET "\033[0m"

static void print_bar(uint32_t qty) {
    for (uint32_t i = 0; i < qty / 10; i++)
        printf("█");
}

static void print_l2_side(orderbook* ob, order_side side) {
    depth_level levels[32];
    size_t n = orderbook_get_l2(ob, side, levels, 32);

    const char* color = (side == SELL) ? RED : GREEN;

    if (side == SELL) {
        /* asks: print high → low */
        for (int i = (int)n - 1; i >= 0; i--) {
            printf("%s%6u | %5u %s",
                   color,
                   levels[i].price,
                   levels[i].total_qty,
                   RESET);
            print_bar(levels[i].total_qty);
            printf("\n");
        }
    } else {
        /* bids: low → high */
        for (size_t i = 0; i < n; i++) {
            printf("%s%6u | %5u %s",
                   color,
                   levels[i].price,
                   levels[i].total_qty,
                   RESET);
            print_bar(levels[i].total_qty);
            printf("\n");
        }
    }
}

static void print_l1(orderbook* ob) {
    uint32_t bid = orderbook_best_bid(ob);
    uint32_t ask = orderbook_best_ask(ob);

    printf(GREEN "Best Bid: %u\n" RESET, bid);
    printf(RED   "Best Ask: %u\n" RESET, ask);

    if (bid && ask) {
        double bps = 10000.0 * (ask - bid) / bid;
        printf(YELLOW "Spread: %.2f bps\n" RESET, bps);
    }
}

void print_orderbook(orderbook* ob, book_view view) {
    printf("\n========== ORDERBOOK ==========\n");

    if (view == VIEW_L1) {
        print_l1(ob);
    } else {
        print_l2_side(ob, SELL);

        uint32_t bid = orderbook_best_bid(ob);
        uint32_t ask = orderbook_best_ask(ob);
        if (bid && ask) {
            double bps = 10000.0 * (ask - bid) / bid;
            printf(YELLOW "\n====== %.2f bps ======\n\n" RESET, bps);
        }

        print_l2_side(ob, BUY);
    }

    printf("================================\n\n");
}
