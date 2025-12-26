#ifndef CANDLE_H
#define CANDLE_H

#include <stdint.h>

typedef struct candle {
    uint64_t index;
    uint32_t open, high, low, close;
    uint32_t volume;
} candle;

#endif
