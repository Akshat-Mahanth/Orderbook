#define _POSIX_C_SOURCE 200809L

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "shm.h"

#define SHM_NAME "/market_snapshot"

static int shm_fd = -1;
static ShmBuffer *shm_ptr = NULL;

int shm_init(ShmBuffer **out)
{
    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd < 0)
        return -1;

    if (ftruncate(shm_fd, sizeof(ShmBuffer)) != 0)
        return -1;

    shm_ptr = mmap(NULL,
                   sizeof(ShmBuffer),
                   PROT_READ | PROT_WRITE,
                   MAP_SHARED,
                   shm_fd,
                   0);

    if (shm_ptr == MAP_FAILED)
        return -1;

    memset(shm_ptr, 0, sizeof(ShmBuffer));

    if (out)
        *out = shm_ptr;

    return 0;
}

void shm_publish(ShmBuffer *buf)
{
    if (!shm_ptr || !buf)
        return;

    memcpy(shm_ptr, buf, sizeof(ShmBuffer));
}

void shm_destroy(void)
{
    if (shm_ptr)
        munmap(shm_ptr, sizeof(ShmBuffer));

    if (shm_fd >= 0)
        close(shm_fd);

    shm_unlink(SHM_NAME);
}

