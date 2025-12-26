#ifndef TRADE_EVENT_H
#define TRADE_EVENT_H

#include <stdint.h>

typedef void (*trade_callback)(
    uint32_t price,
    uint32_t qty,
    char     side,
    void*    user_ctx
);

#endif
