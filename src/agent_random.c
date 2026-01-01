#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "config.h"
#include "agent.h"
#include "order_intent.h"
#include "order_queue.h"

/* --------------------------------------------------
   random agent parameters
   -------------------------------------------------- */

#define MAX_QTY        5
#define PRICE_JITTER  3   /* tighter → more trades */

/* --------------------------------------------------
   helpers
   -------------------------------------------------- */

static int rand_range(int a, int b)
{
    return a + rand() % (b - a + 1);
}

/* --------------------------------------------------
   agent behavior
   -------------------------------------------------- */

static void random_agent_act(agent *self, order_queue *q)
{
    /* 🔴 CRITICAL: zero initialize */
    OrderIntent intent;
    memset(&intent, 0, sizeof(intent));

    intent.agent_id = self->id;
    intent.asset_id = rand() % NUM_ASSETS;
    intent.side     = (rand() & 1) ? BUY : SELL;
    intent.type     = LIMIT;
    intent.qty      = rand_range(1, MAX_QTY);

    /* asset-aware fair price */
    int base_price = 100 + intent.asset_id * 10;
    intent.price   = base_price + rand_range(-PRICE_JITTER, PRICE_JITTER);

    /* safety */
    if (intent.price < 1)
        intent.price = 1;

    order_queue_push(q, &intent);
}

/* --------------------------------------------------
   lifecycle
   -------------------------------------------------- */

agent* agent_random_create(int id)
{
    agent *a = malloc(sizeof(agent));
    if (!a)
        return NULL;

    a->id        = id;
    a->inventory = 0;
    a->cash      = 0.0;
    a->act       = random_agent_act;

    return a;
}

void agent_destroy(agent *a)
{
    free(a);
}

