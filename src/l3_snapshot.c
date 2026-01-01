#define _POSIX_C_SOURCE 200809L

#include <string.h>
#include <pthread.h> 
#include "l3_snapshot.h"
#include "orderbook.h"
#include "queue.h"
#include "hashmap.h"

void build_l3_snapshot(orderbook *ob, L3Snapshot *out)
{
    pthread_rwlock_rdlock(&ob->lock);
    memset(out, 0, sizeof(*out));

    /* -------- bids -------- */
    for (size_t i = 0;
         i < ob->bid_prices->size && out->bid_count < L3_MAX_ORDERS;
         i++)
    {
        uint32_t price = ob->bid_prices->data[i];
        queue *q = hashmap_get(ob->bids, price);
        if (!q) continue;

        for (queue_node *n = q->head;
             n && out->bid_count < L3_MAX_ORDERS;
             n = n->next)
        {
            out->bids[out->bid_count++] = (L3Order){
                .price    = n->ord->price,
                .qty      = n->ord->qty,
                .order_id = n->ord->id,
		.side     = 'B'
            };
        }
    }

    /* -------- asks -------- */
    for (size_t i = 0;
         i < ob->ask_prices->size && out->ask_count < L3_MAX_ORDERS;
         i++)
    {
        uint32_t price = ob->ask_prices->data[i];
        queue *q = hashmap_get(ob->asks, price);
        if (!q) continue;

        for (queue_node *n = q->head;
             n && out->ask_count < L3_MAX_ORDERS;
             n = n->next)
        {
            out->asks[out->ask_count++] = (L3Order){
                .price    = n->ord->price,
                .qty      = n->ord->qty,
                .order_id = n->ord->id,
		.side     = 'S'
            };
        }
    }

    pthread_rwlock_unlock(&ob->lock);
}

