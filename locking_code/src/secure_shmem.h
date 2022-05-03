#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <linux/futex.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <limits.h>

#define UNLOCKED 1
#define LOCKED 0

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

struct mem_region
{
    int fd;
    void* address;
    int user_count;
    pid_t users[2];
    volatile int current_state;
    int exec_count;
    int write_g;
    int read_g;
    int delete_g;
};

int mem_region_struct_fd;
struct mem_region* mem_region_shm;
void *open_shared_mem (const char *name, enum create_or_join action, enum access_options access, off_t size);
void close_shared_mem(void* addr, size_t shm_size);
void delete_shared_mem(const char *name);
int read_shm(void *dest, void *src, size_t num_bytes, int access_num);
int write_shm(void *dest, void *src, size_t num_bytes, int access_num);
void lock(volatile int* data);
void unlock(volatile int* data);
void print_test_array(int* test_array);


