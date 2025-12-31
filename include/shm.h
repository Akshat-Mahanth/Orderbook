#ifndef SHM_H
#define SHM_H

#include <stdint.h>
#include "snapshot.h"

#define SHM_NAME "/market_snapshot"



/*
 * Simple sequence-lock style buffer:
 *  - writer increments seq before + after write
 *  - reader retries if seq changes
 */
typedef struct {
    volatile uint32_t seq;
    MarketSnapshot snapshot;
} ShmBuffer;

/* create / map shared memory */
int shm_init(ShmBuffer **buf);

/* publish a snapshot (writer only) */
void shm_publish(ShmBuffer *buf, const MarketSnapshot *snap);

/* cleanup (optional) */
void shm_destroy(void);

#endif
