#include "dispatcher.h"
#include <stdlib.h>

static void *dispatcher_loop(void *arg)
{
    dispatcher *d = arg;
    OrderIntent intent;

    while (d->running) {
        if (!order_queue_pop(d->queue, &intent))
            continue;

        order *o = calloc(1, sizeof(order));
        o->id    = rand();
        o->side  = intent.side;
        o->type  = intent.type;
        o->price = intent.price;
        o->qty   = intent.qty;

        if (intent.asset_id >= d->num_assets) {
            free(o);
            continue;
        }

        if (o->type == MARKET)
            orderbook_add_market(d->books[intent.asset_id], o);
        else
            orderbook_add_limit(d->books[intent.asset_id], o);
    }
    return NULL;
}

void dispatcher_start(dispatcher *d)
{
    d->running = 1;
    pthread_create(&d->thread, NULL, dispatcher_loop, d);
}

void dispatcher_stop(dispatcher *d)
{
    d->running = 0;
    order_queue_wake_all(d->queue);
    pthread_join(d->thread, NULL);
}

