//#define _GNU_SOURCE
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

#define UNLOCKED 0
#define LOCKED 1

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

//note: for now i'm going to make the list of users and queue an array of 2 until we add in the LinkedList stuff
struct mem_region
{
    //char* name;
    int fd;
    void* address;
    int user_cont;
//    LinkedList users;
//    LinkedList queue;
    pid_t users[2];
    pid_t queue[2];
    int lock_state;
    int created_guard;
};

int mem_region_struct_fd;
struct mem_region* mem_region_shm;
void *open_shared_mem (const char *name, enum create_or_join action, enum access_options access, off_t size);
void close_shared_mem(void* addr, size_t shm_size);
void delete_shared_mem(const char *name);
int read_shm(void *dest, void *src, size_t num_bytes, int access_num);
int write_shm(void *dest, void *src, size_t num_bytes, int access_num);
void lock();
void unlock();


