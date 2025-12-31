#ifndef TRADE_LOG_H
#define TRADE_LOG_H

#include <stdio.h>
#include <stdint.h>
FILE* trade_log_open(const char* filename);
void  trade_log_cb(
    uint32_t price,
    uint32_t qty,
    char side,
    void* ctx
);
void  trade_log_close(FILE* fp);

#endif
