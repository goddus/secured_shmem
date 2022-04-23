#include "secure_shmem.h"
#include "linked_list/mem_region_list.h"



//modified test comment
//TODO: figure out how to properly use a mode_t data type
int assemble_mode (enum access_options access){

    int mem_region_mode;

    if (access == READ){
        mem_region_mode = O_RDONLY;
    }
    else if (access == WRITE){
        mem_region_mode = O_WRONLY;
    }
    else if (access == BOTH){
        mem_region_mode = O_RDWR;
    }

    return mem_region_mode;
    //TODO: figure out how to return an error value from this

}


void *open_shared_mem (const char *name, enum create_or_join action, enum access_options access, off_t size){

    //shm_open()
    int access_level = assemble_mode(access);
    int shm_fd;

    //if user is creating region
    if (action == CREATE_REGION){

        //create region
        shm_fd = shm_open(name, access_level | O_CREAT, S_IRWXU);
        if (shm_fd < 0){
            printf("error creating a shared memory file descriptor\n");
            return NULL;
        }

        //resize region
        int ftruncate_ret_val = ftruncate(shm_fd, size);
        if (ftruncate_ret_val < 0){
            printf("error with ftruncate()\n");
            return NULL;
        }
    }

    //if user is joining region
    else if (action == JOIN_REGION){

        //open region
        shm_fd = shm_open(name, access_level, S_IRWXU);
        if (shm_fd < 0){
            printf("error creating a shared memory file descriptor\n");
            return NULL;
        }
    }

    else{
        errno = EINVAL;    
    }

    //map region to address space
    void* shm_addr = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(shm_addr == (void*)-1){
        printf("error mapping to address space\n");
        return NULL;
    }

    //TODO: update the data structure

    return shm_addr;
    
}


void close_shared_mem(void* addr, size_t shm_size){

    //munmap()
    munmap(addr, shm_size);

    //close() the file descriptor
    
    //update the linked list data structure

}

void delete_shared_mem(const char *name){

    //TODO: verify that this is the last user of the shm region

    //shm_unlink() the shm region
    shm_unlink(name);

    //TODO: update the data structure
}

int read_shm(void *dest, void *src, size_t num_bytes, int access_num){
    printf("in read_shm(), before lock()\n");
    lock();
    printf("in read_shm(), after lock()\n");
    memcpy(dest, src, num_bytes);
    printf("in read_shm(), before unlock()\n");
    unlock();
    printf("in read_shm(), after unlock()\n");
    return 0;
}

int write_shm(void *dest, void *src, size_t num_bytes, int access_num){
    printf("in write_shm(), before lock()\n");
    lock();
    printf("in write_shm(), after lock()\n");
    memcpy(dest, src, num_bytes);
    printf("in write_shm(), before unlock()\n");
    unlock();
    printf("in write_shm(), after unlock()\n");
    return 0;
}

void lock()
{
    int expected = UNLOCKED;
    int desired = LOCKED;
    while(!__atomic_compare_exchange(&mem_region_shm->lock_state, &expected, &desired, 0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE))
    {
        expected = UNLOCKED;
    }
}

void unlock()
{
    int expected = LOCKED;
    int desired = UNLOCKED;
    if (!__atomic_compare_exchange(&mem_region_shm->lock_state, &expected, &desired, 0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE))
    {
        printf("ERROR with unlock\n");
        return;
    }
}