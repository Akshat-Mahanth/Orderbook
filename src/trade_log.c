#include "trade_log.h"
#include <stdint.h>
static uint64_t seq = 0;

FILE* trade_log_open(const char* filename) {
    FILE* fp = fopen(filename, "w");
    fprintf(fp, "seq,price,qty,side\n");
    return fp;
}

void trade_log_cb(
    uint32_t price,
    uint32_t qty,
    char side,
    void* ctx
) {
    FILE* fp = (FILE*)ctx;
    fprintf(fp, "%llu,%u,%u,%c\n",
            (unsigned long long)seq++,
            price, qty, side);
}

void trade_log_close(FILE* fp) {
    fclose(fp);
}
