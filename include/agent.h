#ifndef AGENT_H
#define AGENT_H

#include <stdint.h>
#include "orderbook.h"
#include "order_queue.h"

typedef struct agent {
    int id;
    int inventory;
    double cash;
    void (*act)(struct agent*, order_queue*);
} agent;
/* random agent constructor */
agent* agent_random_create(int id);

/* destroy agent */
void agent_destroy(agent* a);

#endif
