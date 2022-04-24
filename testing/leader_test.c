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



int main(){

    int i;
    struct shared_data *shmem;
    int arr[shared_mem_size];

    //for now allocating a mem_region struct for this test shared memory region until we integrate the linked list code to hold it instead. putting that mem_region struct in its own shared memory region (lol) tagged in comments with: MEM_REGION_SHM
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
    char* mem_region_shm_addr = mmap(NULL, sizeof(struct mem_region), PROT_READ | PROT_WRITE, MAP_SHARED, mem_region_struct_fd, 0);

    //error handling for mmap for MEM_REGION_SHM
    if (mem_region_shm_addr == MAP_FAILED)
    {
        printf("there was an error with mmap() for MEM_REGION_SHM\n");
        return -1;
    } 
    mem_region_shm = (struct mem_region*) mem_region_shm_addr; 
    
    //printf("made the mem_region_shm\n");

    //initializing the guard variables to be 0 for MEM_REGION_SHM
    mem_region_shm->write_g = 0;
    mem_region_shm->read_g = 0;
    mem_region_shm->delete_g = 0;

    //wait for follower_test to signal that its mapping to MEM_REGION_SHM has been created
    while (mem_region_shm->write_g == 0)
    {

    }
    mem_region_shm->write_g = 0;
    //printf("got the notification from follower_test that it can write to mem_region_shm\n");

    //set the exec_count to 1 for MEM_REGION_SHM
    mem_region_shm->exec_count = 0;
    mem_region_shm->current_state = 1;
    printf("just set exec_count\n");

    //signal to follower_test that they can start reading from MEM_REGION_SHM
    mem_region_shm->read_g = 1;
    //printf("notified the follower_test that it can read from mem_region_shm\n");

    //create the shared memory region
    shmem = (struct shared_data*)open_shared_mem(shared_mem_name, CREATE_REGION, BOTH, sizeof(struct shared_data));

    //initialize guard variables to 0
    shmem->write_guard = 0;
    shmem->read_guard = 0;
    shmem->delete_guard = 0;


    //wait for follower to be created
    //printf("waiting for follower_test shm to be created\n");
    while(shmem->write_guard == 0){}
    //printf("done waiting for follower_test shm to be created\n");

    //fill in array
    srand(time(0));
    for(i = 0; i < shared_mem_size; i++){
        arr[i] = rand();
    }

    //printing the leader's local array
    printf("leader's local array:\n");
    for (i = 0; i < shared_mem_size; i++){
        printf("element %d: %d\n", i, arr[i]);
    }

    //copy into struct
    //printf("leader about to call write_shm(), line 76\n");
    write_shm((void*)shmem->data, &arr, sizeof(int)*shared_mem_size, 0);
    //printf("leader finished calling write_shm(), line 78\n");

    //print shared memory, leader perspective
    printf("leader's perspective of what's in shared memory:\n");
    for (i = 0; i < shared_mem_size; i++)
    {
        printf("element %d: %d\n", i, shmem->data[i]);
    }
/*
    //tell follower to read
    //shmem->read_guard = 1;

    //wait for follower to read
    while(shmem->delete_guard == 0){}
*/
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
