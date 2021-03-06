#include "secure_shmem.h"


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

}

//create a new shared memory region or join an existing one
void *open_shared_mem (const char *name, enum create_or_join action, enum access_options access, off_t size){

    //shm_open()
    int access_level = assemble_mode(access);
    int shm_fd, i;

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
       
        //delay loop 
        for (i = 0; i < 1000000000; ++i){}

        //open region
        shm_fd = shm_open(name, access_level, S_IRWXU);
        if (shm_fd < 0){
            printf("error joining a shared memory file descriptor\n");
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


    return shm_addr;

}


void close_shared_mem(void* addr, size_t shm_size){

    //unmap the shared memory region
    munmap(addr, shm_size);


}

void delete_shared_mem(const char *name){

    //shm_unlink() the shm region
    shm_unlink(name);

}

//reading to a shared memory region with locking
int read_shm(void *dest, void *src, size_t num_bytes, int access_num){
    int try_again = 1;
    while (try_again == 1)
    {
        lock(&(mem_region_shm->current_state));
        
        //exec_count and reading stuff
        if (mem_region_shm->exec_count == access_num)
        {
            memcpy(dest, src, num_bytes);
            try_again = 0;
            mem_region_shm->exec_count = (mem_region_shm->exec_count+1);
        }
        unlock(&(mem_region_shm->current_state));
    }
    return 0;
}

//writing to a shared memory region with locking
int write_shm(void *dest, void *src, size_t num_bytes, int access_num){
    int try_again = 1;
    while(try_again == 1)
    {
        lock(&(mem_region_shm->current_state));
        
        //exec_count and writing stuff
        if (mem_region_shm->exec_count == access_num)
        {
            memcpy(dest, src, num_bytes);
            try_again = 0;
            mem_region_shm->exec_count = (mem_region_shm->exec_count+1);
        } 
        unlock(&(mem_region_shm->current_state));
    }
    return 0;
}

//lock the futex
void lock(volatile int* data)
{
    int ret_val = __atomic_sub_fetch(data, 1, __ATOMIC_ACQ_REL);

    if (ret_val >= 0)
    {
        //printf("locked successfully\n");
        return;
    }
    else
    {
        syscall(SYS_futex, data, FUTEX_WAIT, ret_val, NULL);
        while (ret_val < 0)
        {
            ret_val = __atomic_sub_fetch(data, 1, __ATOMIC_ACQ_REL);
        }
        ret_val = __atomic_sub_fetch(data, 1, __ATOMIC_ACQ_REL);
        //printf("locked successfully\n");
        return;
    }
}

//unlock the futex
void unlock(volatile int* data)
{
    int ret_val = __atomic_add_fetch(data, 1, __ATOMIC_ACQ_REL);

    if (ret_val == 1)
    {
        //printf("unlocked successfully\n");
        return;
    }
    else
    {
        __atomic_store_n(data, 1, __ATOMIC_RELEASE);
        //printf("unlocked successfully\n");
        syscall(SYS_futex, data, FUTEX_WAKE, INT_MAX);
        return;
    }
}

//used for testing purposes; prints an array of values
void print_test_array(int* test_array)
{
    int i;
    for (i = 0; i < 10; ++i)
    {
        printf("element %d: %d\n", (i+1), test_array[i]);
    }
    for (i = 0; i < 200; ++i)
    {
        printf("-");
    }
    printf("\n");
}
