#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>


enum create_or_join 
{
    CREATE_REGION,
    JOIN_REGION
};

enum access_options 
{
    READ,
    WRITE,
    BOTH
};

void init();
void *open_shared_mem (const char *name, enum create_or_join action, enum access_options access, off_t size);
void close_shared_mem(void* addr, size_t shm_size);
void delete_shared_mem(const char *name);

