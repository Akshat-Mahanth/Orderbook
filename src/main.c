#include <stdio.h>
#include <stdlib.h>

#include "orderbook.h"
#include "print.h"
#include "trade_log.h"

/* from candle_builder.c */
void build_candles(const char*, const char*);

/* ---------- helpers ---------- */

static uint64_t next_id = 1;

static order* make_order(
    uint32_t price,
    uint32_t qty,
    order_side side,
    order_type type
) {
    order* o = malloc(sizeof(order));
    o->id    = next_id++;
    o->price = price;
    o->qty   = qty;
    o->side  = side;
    o->type  = type;
    return o;
}

/* ---------- main ---------- */

int main(void) {
    orderbook* ob = orderbook_create(1024);

    /* attach trade CSV logger */
    FILE* trade_fp = trade_log_open("output/trades.csv");
    orderbook_set_trade_callback(ob, trade_log_cb, trade_fp);

    /* seed book */
    orderbook_add_limit(ob, make_order(101, 20, SELL, LIMIT));
    orderbook_add_limit(ob, make_order(102, 30, SELL, LIMIT));
    orderbook_add_limit(ob, make_order(100, 25, BUY,  LIMIT));
    orderbook_add_limit(ob, make_order(99,  40, BUY,  LIMIT));

    char choice;
    while (1) {
        printf("\nMenu:\n");
        printf("1 → View L1\n");
        printf("2 → View L2\n");
        printf("3 → Add MARKET BUY\n");
        printf("4 → Add MARKET SELL\n");
        printf("5 → Build candles & exit\n");
        printf("q → Quit (no candles)\n");
        printf("> ");
        fflush(stdout);
        scanf(" %c", &choice);

        if (choice == '1') {
            print_orderbook(ob, VIEW_L1);
        }
        else if (choice == '2') {
            print_orderbook(ob, VIEW_L2);
        }
        else if (choice == '3') {
            orderbook_add_market(ob,
                make_order(0, 15, BUY, MARKET));
        }
        else if (choice == '4') {
            orderbook_add_market(ob,
                make_order(0, 15, SELL, MARKET));
        }
        else if (choice == '5') {
            break;
        }
        else if (choice == 'q') {
            trade_log_close(trade_fp);
            orderbook_destroy(ob);
            return 0;
        }
    }

    /* cleanup */
    trade_log_close(trade_fp);
    orderbook_destroy(ob);

    /* build candles */
    build_candles("output/trades.csv", "output/candles.csv");

    printf("\nCandles written to output/candles.csv\n");
    printf("Run: gnuplot plot/plot.gp\n");

    return 0;
}
