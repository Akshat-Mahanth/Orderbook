#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#include "orderbook.h"
#include "print.h"
#include "trade_log.h"
#include "agent.h"

/* from candle_builder.c */
void build_candles(const char* trades_csv, const char* candles_csv);

/* ---------- helpers ---------- */

static uint32_t next_id = 1;

static order* make_order(
    uint32_t price,
    uint32_t qty,
    order_side side,
    order_type type
) {
    order* o = calloc(1, sizeof(order));

    o->id    = next_id++;
    o->price = price;
    o->qty   = qty;
    o->side  = side;
    o->type  = type;
    return o;
}

/* ---------- simulation parameters ---------- */

#define NUM_AGENTS  20
#define SIM_STEPS  50

/* ---------- main ---------- */

int main(void) {
    /* make stdout unbuffered (important on Windows/MSYS) */
    setvbuf(stdout, NULL, _IONBF, 0);

    printf("Orderbook simulation started\n");
    srand(1);

    //srand((unsigned)time(NULL));

    /* create orderbook */
    orderbook* ob = orderbook_create(1024);

    /* attach trade logger */
    FILE* trade_fp = trade_log_open("output/trades.csv");
    if (!trade_fp) {
        fprintf(stderr, "Failed to open trade log\n");
        return 1;
    }
    orderbook_set_trade_callback(ob, trade_log_cb, trade_fp);

    /* seed the book with an initial price */
    orderbook_add_limit(ob, make_order(100, 50, BUY,  LIMIT));
    orderbook_add_limit(ob, make_order(101, 50, SELL, LIMIT));

    /* create random agents */
    agent* agents[NUM_AGENTS];
    for (int i = 0; i < NUM_AGENTS; i++) {
        agents[i] = agent_random_create(i + 1);
    }

    /* ---------- main loop ---------- */

    char choice;
    while (1) {
        printf("\nMenu:\n");
        printf("1 - View L1 (Top of Book)\n");
        printf("2 - View L2 (Market Depth)\n");
        printf("3 - Run simulation step batch\n");
        printf("4 - Add MARKET BUY (manual shock)\n");
        printf("5 - Add MARKET SELL (manual shock)\n");
        printf("6 - Build candles & exit\n");
        printf("q - Quit (no candles)\n");
        printf("> ");

        scanf(" %c", &choice);

        if (choice == '1') {
            print_orderbook(ob, VIEW_L1);
        }
        else if (choice == '2') {
            print_orderbook(ob, VIEW_L2);
        }
        else if (choice == '3') {
            printf("Running %d simulation steps...\n", SIM_STEPS);
            for (int t = 0; t < SIM_STEPS; t++) {
                for (int i = 0; i < NUM_AGENTS; i++) {
                    agents[i]->act(agents[i], ob);
                }
            }
            printf("Simulation batch complete\n");
        }
        else if (choice == '4') {
            orderbook_add_market(ob,
                make_order(0, 20, BUY, MARKET));
            printf("Manual MARKET BUY injected\n");
        }
        else if (choice == '5') {
            orderbook_add_market(ob,
                make_order(0, 20, SELL, MARKET));
            printf("Manual MARKET SELL injected\n");
        }
        else if (choice == '6') {
            break;
        }
        else if (choice == 'q') {
            printf("Exiting without candle build\n");
            goto cleanup;
        }
    }

    /* ---------- cleanup & post-processing ---------- */

    printf("Building candles...\n");
    build_candles("output/trades.csv", "output/candles.csv");
    printf("Candles written to output/candles.csv\n");
    printf("You can now run: gnuplot plot/plot.gp\n");

cleanup:
    for (int i = 0; i < NUM_AGENTS; i++) {
        agent_destroy(agents[i]);
    }

    trade_log_close(trade_fp);
    orderbook_destroy(ob);

    printf("Simulation ended cleanly\n");
    return 0;
}
