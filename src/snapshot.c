#define _POSIX_C_SOURCE 200809L

#include <time.h>
#include <string.h>

#include "snapshot.h"
#include "orderbook.h"

/* -------------------------------------------------- */
/* helpers                                            */
/* -------------------------------------------------- */

static uint64_t sum_queue_qty(queue *q)
{
    uint64_t total = 0;
    if (!q) return 0;

    for (queue_node *n = q->head; n; n = n->next)
        total += n->ord->qty;

    return total;
}

/* -------------------------------------------------- */
/* snapshot builder                                   */
/* -------------------------------------------------- */

void build_snapshot(orderbook *ob, MarketSnapshot *snap)
{
    /* ---- RESET PER-TICK STATE ---- */
    snap->bid_levels = 0;
    snap->ask_levels = 0;
    snap->best_bid_qty = 0;
    snap->best_ask_qty = 0;

    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    snap->timestamp = (uint64_t)(ts.tv_sec * 1000ULL + ts.tv_nsec / 1000000ULL);

    /* ---------- L1 ---------- */
    snap->best_bid = orderbook_best_bid(ob);
    snap->best_ask = orderbook_best_ask(ob);

    if (snap->best_bid) {
        queue *q = hashmap_get(ob->bids, snap->best_bid);
        snap->best_bid_qty = sum_queue_qty(q);
    }

    if (snap->best_ask) {
        queue *q = hashmap_get(ob->asks, snap->best_ask);
        snap->best_ask_qty = sum_queue_qty(q);
    }

    /* ---------- L2 BIDS ---------- */
    for (size_t i = 0;
         i < ob->bid_prices->size && snap->bid_levels < L2_DEPTH;
         i++)
    {
        uint32_t price = ob->bid_prices->data[i];
        queue *q = hashmap_get(ob->bids, price);

        if (q && q->size > 0) {
            snap->bids[snap->bid_levels].price = price;
            snap->bids[snap->bid_levels].qty   = sum_queue_qty(q);
            snap->bid_levels++;
        }
    }

    /* ---------- L2 ASKS ---------- */
    for (size_t i = 0;
         i < ob->ask_prices->size && snap->ask_levels < L2_DEPTH;
         i++)
    {
        uint32_t price = ob->ask_prices->data[i];
        queue *q = hashmap_get(ob->asks, price);

        if (q && q->size > 0) {
            snap->asks[snap->ask_levels].price = price;
            snap->asks[snap->ask_levels].qty   = sum_queue_qty(q);
            snap->ask_levels++;
        }
    }
}

/* -------------------------------------------------- */
/* trade callback                                     */
/* -------------------------------------------------- */

void snapshot_trade_cb(
    uint32_t price,
    uint32_t qty,
    char side,
    void *ctx
)
{
    (void)side;
    MarketSnapshot *snap = (MarketSnapshot *)ctx;

    if (snap->trade_count >= MAX_TRADES)
        return;

    TradeEvent *t = &snap->trades[snap->trade_count++];

    t->price = price;
    t->qty   = qty;

    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    t->timestamp = (uint64_t)(ts.tv_sec * 1000ULL + ts.tv_nsec / 1000000ULL);
}

