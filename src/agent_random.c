#include <stdlib.h>
#include <time.h>
#include "config.h"

#include "agent.h"
#include "order_intent.h"
#include "order_queue.h"

/* --------------------------------------------------
   random agent parameters
   -------------------------------------------------- */

#define MAX_QTY        5
#define PRICE_CENTER  100
#define PRICE_JITTER  5

/* --------------------------------------------------
   helper
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
    (void)self;

    OrderIntent intent;

    intent.agent_id = self->id;
    intent.asset_id = rand() % NUM_ASSETS;
    intent.side     = (rand() & 1) ? BUY : SELL;
    intent.type     = LIMIT;
    intent.qty      = rand_range(1, MAX_QTY);
    intent.price    = PRICE_CENTER + rand_range(-PRICE_JITTER, PRICE_JITTER);

    if (intent.price == 0)
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

