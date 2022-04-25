#include "shared_mem.h"
#include "../src/secure_shmem.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h> 	   	 /* For shm_open()*/
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */


int open_mem_region_struct()
{
    //tagged everything to do with this mem_region struct with MEM_REGION_SHM in the comment
    mem_region_struct_fd = shm_open("test_mem_region", (O_RDWR | O_CREAT), S_IRWXU);
    if (mem_region_struct_fd < 0)
    {
        printf("there was an error creating the test mem_region struct shared memory region's file descriptor---leader_test.c, line 23\n");
        return mem_region_struct_fd;
    } 

    //resizing the memory region for MEM_REGION_SHM
    int ftruncate_ret_val = ftruncate(mem_region_struct_fd, sizeof(struct mem_region));

    //error handling for ftruncate() for MEM_REGION_SHM
    if (ftruncate_ret_val < 0)
    {
        printf("there was an error with ftruncate() for MEM_REGION_SHM\n");
        return -1;
    }

    //mapping MEM_REGION_SHM
    mem_region_shm_addr = mmap(NULL, sizeof(struct mem_region), PROT_READ | PROT_WRITE, MAP_SHARED, mem_region_struct_fd, 0);

    //error handling for mmap for MEM_REGION_SHM
    if (mem_region_shm_addr == MAP_FAILED)
    {
        printf("there was an error with mmap() for MEM_REGION_SHM\n");
        return -1;
    } 
    mem_region_shm = (struct mem_region*) mem_region_shm_addr;



    return 0;
}

int configure_mem_region()
{
    //initializing the guard variables to be 0 for MEM_REGION_SHM
    mem_region_shm->write_g = 0;
    mem_region_shm->read_g = 0;
    mem_region_shm->delete_g = 0;

    //wait for follower_test to signal that its mapping to MEM_REGION_SHM has been created
    while (mem_region_shm->write_g == 0)
    {

    }
    mem_region_shm->write_g = 0;

    //set the exec_count to 1 for MEM_REGION_SHM
    mem_region_shm->exec_count = 0;
    mem_region_shm->current_state = UNLOCKED;


    return 0; 
}

int main(){

    int i;
    struct shared_data *shmem;
    int arr[shared_mem_size];

    //allocated space for the mem_region struct used in testing in a shared memory region of its own (which was set up with the current POSIX shared memory implementation)
    open_mem_region_struct();
    /*
    //mapping MEM_REGION_SHM
    char* mem_region_shm_addr = mmap(NULL, sizeof(struct mem_region), PROT_READ | PROT_WRITE, MAP_SHARED, mem_region_struct_fd, 0);

    //error handling for mmap for MEM_REGION_SHM
    if (mem_region_shm_addr == MAP_FAILED)
    {
    printf("there was an error with mmap() for MEM_REGION_SHM\n");
    return -1;
    } 
    mem_region_shm = (struct mem_region*) mem_region_shm_addr;
    */
    configure_mem_region(); 
    /*
    //initializing the guard variables to be 0 for MEM_REGION_SHM
    mem_region_shm->write_g = 0;
    mem_region_shm->read_g = 0;
    mem_region_shm->delete_g = 0;

    //wait for follower_test to signal that its mapping to MEM_REGION_SHM has been created
    while (mem_region_shm->write_g == 0)
    {

    }
    mem_region_shm->write_g = 0;

    //set the exec_count to 1 for MEM_REGION_SHM
    mem_region_shm->exec_count = 0;
    mem_region_shm->current_state = UNLOCKED;
    printf("just set exec_count\n");
    */

    //signal to follower_test that they can start reading from MEM_REGION_SHM
    mem_region_shm->read_g = 1;
    //printf("notified the follower_test that it can read from mem_region_shm\n");

    //create the shared memory region
    shmem = (struct shared_data*)open_shared_mem(shared_mem_name, CREATE_REGION, BOTH, sizeof(struct shared_data));

    //initialize guard variables to 0
    shmem->join_guard = 0;

    //wait for follower to be created
    while(shmem->join_guard == 0){}

    //fill in array
    for(i = 0; i < shared_mem_size; i++){
        arr[i] = i;
    }

    //printing the leader's local array
    printf("leader's local array:\n");
    for (i = 0; i < shared_mem_size; i++){
        printf("element %d: %d\n", i, arr[i]);
    }

    //copy into struct
    //printf("leader about to call write_shm(), line 76\n");
    write_shm((void*)shmem->data, &arr, sizeof(int)*shared_mem_size, 1);
    //printf("leader finished calling write_shm(), line 78\n");

    //print shared memory, leader perspective
    printf("leader's perspective of what's in shared memory:\n");
    for (i = 0; i < shared_mem_size; i++)
    {
        printf("element %d: %d\n", i, shmem->data[i]);
    }

    //munmap
    close_shared_mem(shmem, sizeof(struct shared_data));

    //unlink
    delete_shared_mem(shared_mem_name);	

    //wait for the follower_test to signal that MEM_REGION_SHM can be deleted
    while(mem_region_shm->delete_g == 0)
    {

    }
    mem_region_shm->delete_g = 0;

    //unmap for MEM_REGION_SHM
    munmap(mem_region_shm_addr, sizeof(struct mem_region));

    //unlink the shared memory region that holds the test mem_region struct for MEM_REGION_SHM
    shm_unlink("test_mem_region");

    return 0;	

}
