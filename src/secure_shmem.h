#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>

enum secure_shm_return_values {
    MAPPING_FAILURE = 1,
    INVALID_ACCESS = -1
};

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

struct mem_regions_list *regions_list;
volatile int *regions_list_lock;

int init();
void *open_shared_mem (const char *name, enum create_or_join action, enum access_options access, off_t size);
void close_shared_mem(char *name, void* addr);
void delete_shared_mem(const char *name);

