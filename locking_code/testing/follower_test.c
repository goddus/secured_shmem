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


int open_mem_region_struct_follower()
{
    //tagged everything to do with this mem_region struct with MEM_REGION_SHM in the comment
    mem_region_struct_fd = shm_open("test_mem_region", O_RDWR, S_IRWXU);
    if (mem_region_struct_fd < 0)
    {
        printf("there was an error creating the test mem_region struct shared memory region's file descriptor---leader_test.c, line 23\n");
        return mem_region_struct_fd;

    }

    return 0;
}

int configure_mem_region_follower()
{
    //notify leader_test that it can write to MEM_REGION_SHM
    mem_region_shm->write_g = 1; 
    //printf("notified leader_test that it can write to mem_region_shm\n");


    //wait for leader_test to signal that it can read from MEM_REGION_SHM
    while(mem_region_shm->read_g == 0)
    {

    }
    mem_region_shm->read_g = 0;

    return 0;
}

int main(){

    int i, fd, ret_val;
    struct shared_data *shmem;
    int arr[shared_mem_size];
    struct timespec start, end;
    time_t time_seconds;
    long time_nseconds;

    open_mem_region_struct_follower();

    //mapping MEM_REGION_SHM
    char* mem_region_shm_addr = mmap(NULL, sizeof(struct mem_region), PROT_READ | PROT_WRITE, MAP_SHARED, mem_region_struct_fd, 0);

    //error handling for mmap for MEM_REGION_SHM
    if (mem_region_shm_addr == MAP_FAILED)
    {
        printf("there was an error with mmap() for MEM_REGION_SHM\n");
        return -1;
    } 
    mem_region_shm = (struct mem_region*) mem_region_shm_addr; 

    configure_mem_region_follower();

    //join the shared memory region 
    shmem = (struct shared_data *) open_shared_mem(shared_mem_name, JOIN_REGION, BOTH, sizeof(struct shared_data));


    //notify leader to write and wait
    shmem->join_guard = 1;

    //TESTING READS AND WRITES

    //read_shm, access num 1
    read_shm(&arr, (void*)shmem->data, sizeof(int)*shared_mem_size, 0);
    printf("follower local array after read_shm order 1:\n");
    print_test_array(arr);
  
    //read_shm, access num 2
    read_shm(&arr, (void*)shmem->data, sizeof(int)*shared_mem_size, 2);
    printf("follower local array after read_shm order 2:\n");
    print_test_array(arr);


    //filling the local array with -1's before writing
    for (i = 0; i < shared_mem_size; ++i)
    {
        arr[i] = -1;
    }


    //write_shm, access num 3
    write_shm((void*)shmem->data, &arr, sizeof(int)*shared_mem_size, 3);
    printf("follower local array after write_shm order 3:\n");
    print_test_array(arr);

    //unmapping the shared memory region with our data
    close_shared_mem(shmem, sizeof(struct shared_data));

    //signal to leader_test that it can delete MEM_REGION_SHM
    mem_region_shm->delete_g = 1;

    //unmap and unlink for MEM_REGION_SHM
    munmap(mem_region_shm_addr, sizeof(struct mem_region));
    shm_unlink("test_mem_region");

    return 0;	

}
