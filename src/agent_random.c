#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "agent.h"

/* tunable parameters */
#define MAX_QTY        10
#define PRICE_JITTER  2      /* ticks away from mid */
#define CANCEL_PROB   20     /* percent */

/* helper: random int in [a,b] */
static int rand_range(int a, int b) {
    return a + rand() % (b - a + 1);
}

static void random_agent_act(agent* self, orderbook* ob) {
    uint32_t bid = orderbook_best_bid(ob);
    uint32_t ask = orderbook_best_ask(ob);

    if (bid == 0 || ask == 0)
        return;

    uint32_t mid = (bid + ask) / 2;

    if (self->last_order_id != 0 && rand() % 100 < CANCEL_PROB) {
        if (orderbook_cancel(ob, self->last_order_id)) {
            self->last_order_id = 0;
            return;
        }
        /* if cancel failed, order is already gone */
        self->last_order_id = 0;
    }


    /* choose side */
    order_side side = (rand() % 2) ? BUY : SELL;
    uint32_t qty = rand_range(1, MAX_QTY);

    int offset = rand_range(-PRICE_JITTER, PRICE_JITTER);
    uint32_t price = mid + offset;

    if (price == 0)
        price = 1;

    order* o = calloc(1, sizeof(order));

    o->id    = (uint32_t)rand();
    o->price = price;
    o->qty   = qty;
    o->side  = side;
    o->type  = LIMIT;

    orderbook_add_limit(ob, o);
    self->last_order_id = o->id;
}

/* constructor */
agent* agent_random_create(int id) {
    agent* a = malloc(sizeof(agent));
    a->id = id;
    a->cash = 0.0;
    a->inventory = 0;
    a->last_order_id = 0;
    a->act = random_agent_act;
    return a;
}

void agent_destroy(agent* a) {
    free(a);
}
