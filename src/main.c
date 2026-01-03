#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <string.h>

#include "orderbook.h"
#include "snapshot.h"
#include "l3_snapshot.h"
#include "shm.h"
#include "agent.h"
#include "agent_thread.h"
#include "order_queue.h"
#include "dispatcher.h"
#include "config.h"

static volatile int running = 1;

static void handle_sigint(int sig)
{
    (void)sig;
    running = 0;
}

/* --------------------------------------------------
   seed liquidity
   -------------------------------------------------- */
static void seed_book(orderbook *ob, int asset)
{
    uint32_t base = 100 + asset * 10;

    for (uint32_t i = 0; i < 5; i++) {
        order *bid = calloc(1, sizeof(order));
        bid->id    = rand();
        bid->side  = BUY;
        bid->price = base - i;
        bid->qty   = 20;
        bid->type  = LIMIT;

        order *ask = calloc(1, sizeof(order));
        ask->id    = rand();
        ask->side  = SELL;
        ask->price = base + 1 + i;
        ask->qty   = 20;
        ask->type  = LIMIT;

        orderbook_add_limit(ob, bid);
        orderbook_add_limit(ob, ask);
    }
}

int main(void)
{
    signal(SIGINT, handle_sigint);
    srand((unsigned)time(NULL));

    /* --------------------------------------------------
       orderbooks
       -------------------------------------------------- */
    orderbook *books[NUM_ASSETS];

    for (int i = 0; i < NUM_ASSETS; i++) {
        books[i] = orderbook_create(1024);
        seed_book(books[i], i);
    }

    /* --------------------------------------------------
       shared memory
       -------------------------------------------------- */
    if (shm_init(NULL) != 0) {
        perror("shm_init");
        exit(1);
    }

    ShmBuffer shm_state;
    memset(&shm_state, 0, sizeof(shm_state));

    /* trade callbacks → L2 snapshots */
    for (int a = 0; a < NUM_ASSETS; a++) {
        orderbook_set_trade_callback(
            books[a],
            snapshot_trade_cb,
            &shm_state.l2[a]
        );
    }

    /* --------------------------------------------------
       global order queue
       -------------------------------------------------- */
    order_queue global_q;
    order_queue_init(&global_q, 4096);

    /* --------------------------------------------------
       dispatcher
       -------------------------------------------------- */
    dispatcher disp = {
        .books      = books,
        .queue      = &global_q,
        .num_assets = NUM_ASSETS
    };
    dispatcher_start(&disp);

    /* --------------------------------------------------
       agents
       -------------------------------------------------- */
    agent *agents[NUM_AGENTS];
    agent_thread threads[NUM_AGENTS];

    for (int i = 0; i < NUM_AGENTS; i++) {
        agents[i] = agent_random_create(i);
        threads[i].agent = agents[i];
        threads[i].queue = &global_q;
        agent_thread_start(&threads[i]);
    }

    struct timespec ts = {0, 100 * 1000 * 1000};

    /* ==================================================
       FRAME LOOP
       ================================================== */
    while (running) {

        for (int a = 0; a < NUM_ASSETS; a++) {

            /* ---------- L2 ---------- */
            build_snapshot(books[a], &shm_state.l2[a]);

            /* ---------- L3 ---------- */
            build_l3_snapshot(books[a], &shm_state.l3[a]);

            /* 🔴 CRITICAL DEBUG PRINT 🔴 */
            if (shm_state.l3[a].bid_count > 0 ||
                shm_state.l3[a].ask_count > 0)
            {
                printf(
                    "L3 asset %d: bids=%u asks=%u\n",
                    a,
                    shm_state.l3[a].bid_count,
                    shm_state.l3[a].ask_count
                );
            }
        }

        /* publish snapshot */
        shm_publish(&shm_state);

        /* clear per-frame trades */
        for (int a = 0; a < NUM_ASSETS; a++) {
            shm_state.l2[a].trade_count = 0;
        }

        nanosleep(&ts, NULL);
    }

    /* --------------------------------------------------
       shutdown
       -------------------------------------------------- */
    for (int i = 0; i < NUM_AGENTS; i++) {
        agent_thread_stop(&threads[i]);
        agent_destroy(agents[i]);
    }

    dispatcher_stop(&disp);
    order_queue_destroy(&global_q);
    shm_destroy();

    for (int i = 0; i < NUM_ASSETS; i++)
        orderbook_destroy(books[i]);

    return 0;
}

