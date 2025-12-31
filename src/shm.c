#define _POSIX_C_SOURCE 200809L

#include "shm.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

static int shm_fd = -1;

int shm_init(ShmBuffer **buf)
{
    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd < 0) return -1;

    if (ftruncate(shm_fd, sizeof(ShmBuffer)) != 0) return -1;

    void *addr = mmap(NULL, sizeof(ShmBuffer),
                      PROT_READ | PROT_WRITE,
                      MAP_SHARED, shm_fd, 0);
    if (addr == MAP_FAILED) return -1;

    *buf = (ShmBuffer *)addr;
    memset(*buf, 0, sizeof(ShmBuffer));
    return 0;
}

void shm_publish(ShmBuffer *buf, const MarketSnapshot *snap)
{
    buf->seq++;
    memcpy(&buf->snapshot, snap, sizeof(MarketSnapshot));
    buf->seq++;
}

void shm_destroy(void)
{
    if (shm_fd >= 0) {
        close(shm_fd);
        shm_unlink(SHM_NAME);
    }
}

