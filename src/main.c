#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

#include "orderbook.h"
#include "order.h"
#include "snapshot.h"
#include "shm.h"
#include "agent.h"

/* ==================================================
   runtime control
   ================================================== */
static volatile int running = 1;

static void handle_sigint(int sig)
{
    (void)sig;
    running = 0;
}

/* ==================================================
   seed initial liquidity
   ================================================== */
static void seed_book(orderbook *ob)
{
    for (uint32_t i = 0; i < 5; i++) {

        order *bid = calloc(1, sizeof(order));
        bid->id    = 1000 + i;
        bid->side  = BUY;
        bid->price = 100 - i;
        bid->qty   = 20;
        bid->type  = LIMIT;

        order *ask = calloc(1, sizeof(order));
        ask->id    = 2000 + i;
        ask->side  = SELL;
        ask->price = 101 + i;
        ask->qty   = 20;
        ask->type  = LIMIT;

        orderbook_add_limit(ob, bid);
        orderbook_add_limit(ob, ask);
        }
}
static void ensure_book_health(orderbook *ob)
{
    if (orderbook_best_ask(ob) == 0) {
        order *ask = calloc(1, sizeof(order));
        ask->id    = rand();
        ask->side  = SELL;
        ask->price = orderbook_best_bid(ob) + 1;
        ask->qty   = 20;
        ask->type  = LIMIT;
        orderbook_add_limit(ob, ask);
    }

    if (orderbook_best_bid(ob) == 0) {
        order *bid = calloc(1, sizeof(order));
        bid->id    = rand();
        bid->side  = BUY;
        bid->price = orderbook_best_ask(ob) - 1;
        bid->qty   = 20;
        bid->type  = LIMIT;
        orderbook_add_limit(ob, bid);
    }
}

/* =================================================
   main
   ================================================== */
int main(void)
{
    signal(SIGINT, handle_sigint);
    srand((unsigned)time(NULL));

    /* ---- create orderbook ---- */
    orderbook *ob = orderbook_create(1024);
    if (!ob) {
        fprintf(stderr, "failed to create orderbook\n");
        return 1;
    }

    seed_book(ob);

    /* ---- create agents ---- */
    const int NUM_AGENTS = 3;
    agent *agents[NUM_AGENTS];

    for (int i = 0; i < NUM_AGENTS; i++) {
        agents[i] = agent_random_create(i);
        if (!agents[i]) {
            fprintf(stderr, "failed to create agent %d\n", i);
            return 1;
        }
    }

    /* ---- shared memory ---- */
    ShmBuffer *shm_buf = NULL;
    if (shm_init(&shm_buf) != 0) {
        fprintf(stderr, "failed to init shared memory\n");
        return 1;
    }

    MarketSnapshot snap;
    orderbook_set_trade_callback(ob, snapshot_trade_cb, &snap);
    printf("DEBUG: on_trade ptr = %p\n", (void*)ob->on_trade);

    struct timespec ts;
    ts.tv_sec  = 0;
    ts.tv_nsec = 100 * 1000 * 1000; /* 100 ms */

    printf("simulation running (Ctrl+C to stop)\n");

    
    /* ==================================================
       MAIN SIMULATION LOOP
       ================================================== */
    while (running) {
        snap.trade_count = 0;   
        ensure_book_health(ob);
        /* ---- agents act FIRST ---- */
        for (int i = 0; i < NUM_AGENTS; i++) {
            agents[i]->act(agents[i], ob);
        }

        /* ---- snapshot AFTER state changes ---- */
        build_snapshot(ob, &snap);
        shm_publish(shm_buf, &snap);

        /* ---- debug (optional) ---- */
        printf("BID %u (%lu) | ASK %u (%lu)\n",
               snap.best_bid,
               snap.best_bid_qty,
               snap.best_ask,
               snap.best_ask_qty);

        nanosleep(&ts, NULL);
    }

    /* ==================================================
       cleanup
       ================================================== */
    shm_destroy();

    for (int i = 0; i < NUM_AGENTS; i++) {
        agent_destroy(agents[i]);
    }

    orderbook_destroy(ob);

    printf("simulation stopped\n");
    return 0;
}

