#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

#include "orderbook.h"
#include "snapshot.h"
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

/* -------------------------------------------------- */
/* seed liquidity per asset                           */
/* -------------------------------------------------- */
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
       snapshot (asset 0)
       -------------------------------------------------- */
    ShmBuffer *shm_buf = NULL;
    shm_init(&shm_buf);
    
    for (int a = 0; a < NUM_ASSETS; a++) {
        orderbook_set_trade_callback(
            books[a],
            snapshot_trade_cb,
            &shm_buf->snaps[a]
        );
    }
    

    /* --------------------------------------------------
       global order queue (bounded)
       -------------------------------------------------- */
    order_queue global_q;
    order_queue_init(&global_q, 4096);

    /* --------------------------------------------------
       dispatcher
       -------------------------------------------------- */
    dispatcher disp = {
        .books      = books,      /* orderbook ** */
        .queue      = &global_q,  /* single queue */
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
        shm_buf->snaps[a].trade_count = 0;
        build_snapshot(books[a], &shm_buf->snaps[a]);
    }
    
    shm_publish(shm_buf);
    

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

