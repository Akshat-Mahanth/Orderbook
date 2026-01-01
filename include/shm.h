#ifndef SHM_H
#define SHM_H
#include <stddef.h>
#include "snapshot.h"
#include "l3_snapshot.h"
#include "config.h"

typedef struct {
    MarketSnapshot l2[NUM_ASSETS];
    L3Snapshot     l3[NUM_ASSETS];
} ShmBuffer;

int  shm_init(ShmBuffer **out);
void shm_publish(ShmBuffer *buf);
void shm_destroy(void);

#endif

