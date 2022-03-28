#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>


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

int open_shared_mem (const char *name, enum create_or_join, enum access, off_t size);

