#ifndef SHM_H
#define SHM_H

#include "snapshot.h"
#include "config.h"

typedef struct {
    MarketSnapshot snaps[NUM_ASSETS];
} ShmBuffer;

int  shm_init(ShmBuffer **out);
void shm_publish(ShmBuffer *buf);
void shm_destroy(void);

#endif

