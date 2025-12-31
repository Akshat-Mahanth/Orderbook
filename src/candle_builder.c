#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>   /* REQUIRED for PRIu64 */

#define TRADES_PER_CANDLE 2 

void build_candles(
    const char* trades_csv,
    const char* candles_csv
) {
    FILE* in = fopen(trades_csv, "r");
    if (!in) {
        perror("open trades.csv");
        return;
    }

    FILE* out = fopen(candles_csv, "w");
    if (!out) {
        perror("open candles.csv");
        fclose(in);
        return;
    }

    fprintf(out, "index,open,high,low,close,volume\n");

    char header[128];
    fgets(header, sizeof(header), in); /* skip header */

    uint64_t seq;
    uint32_t price, qty;
    char side;

    uint32_t open = 0, high = 0, low = 0, close = 0;
    uint32_t volume = 0;

    uint64_t candle_idx = 0;
    int count = 0;

    while (fscanf(in, "%" PRIu64 ",%u,%u,%c\n",
                  &seq, &price, &qty, &side) == 4) {

        if (count == 0) {
            open = high = low = price;
            volume = 0;
        }

        if (price > high) high = price;
        if (price < low)  low  = price;

        close = price;
        volume += qty;
        count++;

        if (count == TRADES_PER_CANDLE) {
            fprintf(out, "%" PRIu64 ",%u,%u,%u,%u,%u\n",
                    candle_idx++,
                    open, high, low, close, volume);
            count = 0;
        }
    }
    /* emit last partial candle */
    if (count > 0) {
        fprintf(out, "%" PRIu64 ",%u,%u,%u,%u,%u\n",
                candle_idx,
                open, high, low, close, volume);
    }

    fclose(in);
    fclose(out);
}
