#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "agent.h"
#include "orderbook.h"

/* =========================
   Tunable parameters
   ========================= */
#define MAX_QTY        10
#define PRICE_JITTER   2      /* ticks around mid */
#define CANCEL_PROB    20     /* % chance to cancel last order */
#define MARKET_PROB    30     /* % chance of market order */
#define AGGRESS_PROB   30     /* % chance of aggressive limit */

/* helper: random int in [a, b] */
static int rand_range(int a, int b)
{
    return a + rand() % (b - a + 1);
}

/* =========================
   Random agent behavior
   ========================= */
static void random_agent_act(agent *self, orderbook *ob)
{
    uint32_t bid = orderbook_best_bid(ob);
    uint32_t ask = orderbook_best_ask(ob);

    if (bid == 0 || ask == 0)
        return;

    uint32_t mid = (bid + ask) / 2;

    /* -------------------------
       Occasionally cancel
       ------------------------- */
    if (self->last_order_id != 0 && rand() % 100 < CANCEL_PROB) {
        orderbook_cancel(ob, self->last_order_id);
        self->last_order_id = 0;
        return;
    }

    /* choose side + quantity */
    order_side side = (rand() % 2) ? BUY : SELL;
    uint32_t qty = rand_range(1, MAX_QTY);

    int r = rand() % 100;

    /* =========================
       MARKET ORDER (forces trade)
       ========================= */
    if (r < MARKET_PROB) {
        order *o = calloc(1, sizeof(order));
        if (!o) return;

        o->id   = (uint32_t)rand();
        o->qty  = qty;
        o->side = side;
        o->type = MARKET;

        orderbook_add_market(ob, o);
        self->last_order_id = 0;
        return;
    }

    uint32_t price;

    /* =========================
       AGGRESSIVE LIMIT (cross book)
       ========================= */
    if (r < MARKET_PROB + AGGRESS_PROB) {
        price = (side == BUY) ? ask : bid;
    }
    /* =========================
       PASSIVE LIMIT (around mid)
       ========================= */
    else {
        int offset = rand_range(-PRICE_JITTER, PRICE_JITTER);
        price = mid + offset;
        if (price == 0) price = 1;
    }

    order *o = calloc(1, sizeof(order));
    if (!o) return;

    o->id    = (uint32_t)rand();
    o->price = price;
    o->qty   = qty;
    o->side  = side;
    o->type  = LIMIT;

    orderbook_add_limit(ob, o);
    self->last_order_id = o->id;
}

/* =========================
   Constructor / Destructor
   ========================= */
agent *agent_random_create(int id)
{
    agent *a = malloc(sizeof(agent));
    if (!a) return NULL;

    a->id = id;
    a->cash = 0.0;
    a->inventory = 0;
    a->last_order_id = 0;
    a->act = random_agent_act;

    return a;
}

void agent_destroy(agent *a)
{
    if (a)
        free(a);
}

