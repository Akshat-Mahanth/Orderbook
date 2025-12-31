#include "depth.h"

/* NOTE:
 * We iterate price levels via heap order,
 * then aggregate quantities from the FIFO queue.
 */
size_t orderbook_get_l2(
    orderbook* ob,
    order_side side,
    depth_level* out,
    size_t max_levels
) {
    size_t count = 0;

    heap* prices = (side == BUY) ? ob->bid_prices : ob->ask_prices;
    hashmap* book = (side == BUY) ? ob->bids : ob->asks;

    for (size_t i = 0; i < prices->size && count < max_levels; i++) {
        uint32_t price = prices->data[i];
        queue* q = hashmap_get(book, price);
        if (!q) continue;

        uint32_t sum = 0;
        for (queue_node* n = q->head; n; n = n->next)
            sum += n->ord->qty;

        out[count].price = price;
        out[count].total_qty = sum;
        count++;
    }
    return count;
}
