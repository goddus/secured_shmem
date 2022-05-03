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


int open_mem_region_struct_leader()
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

    return 0;
}

int configure_mem_region_leader()
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
    open_mem_region_struct_leader();
    
    //mapping MEM_REGION_SHM
    char* mem_region_shm_addr = mmap(NULL, sizeof(struct mem_region), PROT_READ | PROT_WRITE, MAP_SHARED, mem_region_struct_fd, 0);

    //error handling for mmap for MEM_REGION_SHM
    if (mem_region_shm_addr == MAP_FAILED)
    {
        printf("there was an error with mmap() for MEM_REGION_SHM\n");
        return -1;
    } 
    mem_region_shm = (struct mem_region*) mem_region_shm_addr;

    configure_mem_region_leader(); 

    //signal to follower_test that they can start reading from MEM_REGION_SHM
    mem_region_shm->read_g = 1;

    //create the shared memory region
    shmem = (struct shared_data*)open_shared_mem(shared_mem_name, CREATE_REGION, BOTH, sizeof(struct shared_data));

    //initialize guard variables to 0
    shmem->join_guard = 0;

    //wait for follower to be created
    while(shmem->join_guard == 0){}

    //fill in and print the leader's local array with [0,10]
    for(i = 0; i < shared_mem_size; i++){
        arr[i] = i;
    }
    printf("initial value of the leader local array (before any reads or writes):\n");
    print_test_array(arr);


    //TESTING READS AND WRITES

    //write_shm, access num 1
    write_shm((void*)shmem->data, &arr, sizeof(int)*shared_mem_size, 1);
    printf("leader local array after write_shm order 1:\n");
    print_test_array(arr);

    //read_shm, access num 4
    read_shm(&arr, (void*)shmem->data, sizeof(int)*shared_mem_size, 4);
    printf("leader local array after read_shm order 4:\n");
    print_test_array(arr);

    //unmapping and unlinking the shared memory region with our data
    close_shared_mem(shmem, sizeof(struct shared_data));
    delete_shared_mem(shared_mem_name);	

    //wait for the follower_test to signal that MEM_REGION_SHM can be deleted
    while(mem_region_shm->delete_g == 0)
    {

    }
    mem_region_shm->delete_g = 0;

    //unmap and unlink for MEM_REGION_SHM
    munmap(mem_region_shm_addr, sizeof(struct mem_region));
    shm_unlink("test_mem_region");

    return 0;	

}
