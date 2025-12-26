#include <stdio.h>
#include <stdlib.h>
#include "orderbook.h"

/* ---------- helpers ---------- */

static uint64_t next_id = 1;

static order* make_order(
    uint32_t price,
    uint32_t qty,
    order_side side,
    order_type type
) {
    order* o = malloc(sizeof(order));
    o->id    = next_id++;
    o->price = price;
    o->qty   = qty;
    o->side  = side;
    o->type  = type;
    return o;
}

/* ---------- main ---------- */

int main(void) {

    printf("=== ORDERBOOK TEST START ===\n\n");

    orderbook* ob = orderbook_create(1024);

    /* ----- LIMIT orders ----- */
    printf("Adding LIMIT SELL orders\n");
    orderbook_add_limit(ob, make_order(101, 5, SELL, LIMIT));
    orderbook_add_limit(ob, make_order(102, 10, SELL, LIMIT));

    printf("Adding LIMIT BUY orders\n");
    orderbook_add_limit(ob, make_order(99, 7, BUY, LIMIT));
    orderbook_add_limit(ob, make_order(100, 8, BUY, LIMIT));

    printf("Best Bid = %u, Best Ask = %u\n\n",
           orderbook_best_bid(ob),
           orderbook_best_ask(ob));

    /* ----- MARKET order ----- */
    printf("Adding MARKET BUY (qty=6)\n");
    orderbook_add_market(ob, make_order(0, 6, BUY, MARKET));

    printf("Best Bid = %u, Best Ask = %u\n\n",
           orderbook_best_bid(ob),
           orderbook_best_ask(ob));

    /* ----- IOC order ----- */
    printf("Adding IOC SELL (price=100, qty=20)\n");
    orderbook_add_limit(ob, make_order(100, 20, SELL, IOC));

    printf("Best Bid = %u, Best Ask = %u\n\n",
           orderbook_best_bid(ob),
           orderbook_best_ask(ob));

    /* ----- FOK order (FAIL) ----- */
    printf("Adding FOK BUY (price=101, qty=100) — should FAIL\n");
    orderbook_add_limit(ob, make_order(101, 100, BUY, FOK));

    printf("Best Bid = %u, Best Ask = %u\n\n",
           orderbook_best_bid(ob),
           orderbook_best_ask(ob));

    /* ----- FOK order (SUCCESS) ----- */
    printf("Adding FOK BUY (price=102, qty=5) — should SUCCEED\n");
    orderbook_add_limit(ob, make_order(102, 5, BUY, FOK));

    printf("Best Bid = %u, Best Ask = %u\n\n",
           orderbook_best_bid(ob),
           orderbook_best_ask(ob));

    /* ----- CANCEL ----- */
    printf("Adding LIMIT BUY (price=98, qty=10)\n");
    order* to_cancel = make_order(98, 10, BUY, LIMIT);
    uint64_t cancel_id = to_cancel->id;
    orderbook_add_limit(ob, to_cancel);

    printf("Cancel order id %llu\n",
           (unsigned long long)cancel_id);
    orderbook_cancel(ob, cancel_id);

    printf("Best Bid = %u, Best Ask = %u\n\n",
           orderbook_best_bid(ob),
           orderbook_best_ask(ob));

    /* ----- MODIFY ----- */
    printf("Adding LIMIT SELL (price=105, qty=12)\n");
    order* to_modify = make_order(105, 12, SELL, LIMIT);
    uint64_t modify_id = to_modify->id;
    orderbook_add_limit(ob, to_modify);

    printf("Modify order id %llu -> price=103, qty=6\n",
           (unsigned long long)modify_id);
    orderbook_modify(ob, modify_id, 103, 6);

    printf("Best Bid = %u, Best Ask = %u\n\n",
           orderbook_best_bid(ob),
           orderbook_best_ask(ob));

    /* ----- cleanup ----- */
    orderbook_destroy(ob);

    printf("=== ORDERBOOK TEST END ===\n");
    return 0;
}
