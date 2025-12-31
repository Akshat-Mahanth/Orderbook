#ifndef AGENT_H
#define AGENT_H

#include <stdint.h>
#include "orderbook.h"

/* generic agent */
typedef struct agent {
    int      id;
    double   cash;
    int      inventory;

    /* behavior */
    void (*act)(struct agent* self, orderbook* ob);

    /* private state */
    uint64_t last_order_id;
} agent;

/* random agent constructor */
agent* agent_random_create(int id);

/* destroy agent */
void agent_destroy(agent* a);

#endif
