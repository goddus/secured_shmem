#include "shared_mem.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h> 	   	 /* For shm_open()*/
#include <sys/stat.h>        /* For mode constants */
#include "../src/secure_shmem.h"
#include <fcntl.h>           /* For O_* constants */
#include <time.h> 	/* For time() */

int main(){

    int i, fd, ret_val;
    struct shared_data *shmem;
    int arr[shared_mem_size];
    struct timespec start, end;
    time_t time_seconds;
    long time_nseconds;

    //for now allocating a mem_region struct for this test shared memory region until we integrate the linked list code to hold it instead. putting that mem_region struct in its own shared memory region (lol) tagged in comments with: MEM_REGION_SHM
    mem_region_struct_fd = shm_open("test_mem_region", O_RDWR, S_IRWXU);
    if (mem_region_struct_fd < 0)
    {
        printf("there was an error creating the test mem_region struct shared memory region's file descriptor---leader_test.c, line 23\n");
        return mem_region_struct_fd;

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

    //printf("joined the mem_region_shm\n");
    
    //notify leader_test that it can write to MEM_REGION_SHM
    mem_region_shm->write_g = 1; 
    //printf("notified leader_test that it can write to mem_region_shm\n");

    //wait for leader_test to signal that it can read from MEM_REGION_SHM
    while(mem_region_shm->read_g == 0)
    {

    }
    mem_region_shm->read_g = 0;

    //printf("exec_count from the follower_test perspective: %d\n", mem_region_shm->exec_count);


    //join the shared memory region 
    shmem = (struct shared_data *) open_shared_mem(shared_mem_name, JOIN_REGION, BOTH, sizeof(struct shared_data));

    clock_gettime(CLOCK_REALTIME, &start);

    //notify leader to write and wait
    shmem->write_guard = 1;
    //printf("just set shmem->write_guard to 1\n");
    //while(shmem->read_guard == 0){}
    
    //printf("follower about to call read_shm(), line 32\n");
    //copy into struct
    read_shm(&arr, (void*)shmem->data, sizeof(int)*shared_mem_size, 3);
    //printf("follower finished calling read_shm(), line 35\n");
/*
    //printing the follower's local array
    printf("follower's local array:\n");
    for (i = 0; i < shared_mem_size; ++i){
        printf("element %d: %d\n", i, arr[i]);
    }

    //print shared memory, follower perspective
    printf("follower's perspective of what's in shared memory:\n");
    for (i = 0; i < shared_mem_size; i++)
    {
        printf("element %d: %d\n", i, shmem->data[i]);
    }
*/
    //notify leader that it can destroy shared memory
    //shmem->delete_guard = 1;

    clock_gettime(CLOCK_REALTIME, &end);

    if((end.tv_nsec - start.tv_nsec) < 0){
        time_seconds = (end.tv_sec - start.tv_sec) - 1;
        time_nseconds = 1000000000 + (end.tv_nsec - start.tv_nsec); 
    }
    else {
        time_seconds = (end.tv_sec - start.tv_sec);
        time_nseconds = (end.tv_nsec - start.tv_nsec);	
    }

    //printf("With a array size of %d, the data transfer through shared memory took %d seconds and %ld nanoseconds.\n", shared_mem_size, time_seconds, time_nseconds);	

    //munmap
    //close_shared_mem(shmem, sizeof(struct shared_data));

    //signal to leader_test that it can delete MEM_REGION_SHM
    mem_region_shm->delete_g = 1;

    //unmap for MEM_REGION_SHM
    munmap(mem_region_shm_addr, sizeof(struct mem_region));

    //unlink the shared memory region that holds the test mem_region struct for MEM_REGION_SHM
    shm_unlink("test_mem_region");

    return 0;	

}
