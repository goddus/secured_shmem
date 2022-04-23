#include "shared_memory.h" 
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>



int main()
{
    int i;

    //creating a shared memory file descriptor
    int shared_mem_fd = shm_open(SHARED_MEM, (O_RDWR | O_CREAT), S_IRWXU);


    //error handling for shm_open()
    if (shared_mem_fd < 0)
    {
        printf("there was an error creating a shared memory file descriptor\n");
        return shared_mem_fd;
    }


    //resizing the memory region
    int ftruncate_ret_val = ftruncate(shared_mem_fd, sizeof(struct shared_mem_data));



    //error handling for ftruncate()
    if (ftruncate_ret_val < 0)
    {
        printf("there was an error with ftruncate()\n");
        return -1;
    }


    //mapping the shared memory to the program's address space
    char* shm_addr = mmap(NULL, sizeof(struct shared_mem_data), PROT_READ | PROT_WRITE, MAP_SHARED, shared_mem_fd, 0);



    //error handling for mmap
    if (shm_addr == MAP_FAILED)
    {
        printf("there was an error with mmap()\n");
        return -1;
    }


    struct shared_mem_data* shared_mem = (struct shared_mem_data*) shm_addr;

    
    //initializing the guard variables to be 0
    shared_mem->write_guard = 0;
    shared_mem->read_guard = 0;
    shared_mem->delete_guard = 0;

    

    //declaring the array and filling it with random integers
    int the_array[SHARED_ARRAY_SIZE];

    for (i = 0; i < SHARED_ARRAY_SIZE; ++i)
    {
        the_array[i] = rand();
    }

    //printing the leader's local array
    for (i = 0; i < SHARED_ARRAY_SIZE; ++i)
    {
        printf("element %d: %d\n", i, the_array[i]);
    } 
   
    
    //wait for the follower to signal that its mapping to the shared data has been created
    while (shared_mem->write_guard == 0)
    {

    }
    shared_mem->write_guard = 0;


    //copying the local array contents into the shared memory's data aray element-by-element
    for (i = 0; i < SHARED_ARRAY_SIZE; ++i)
    {
        shared_mem->data[i] = the_array[i];
    }


    //signal to the follower that they can start reading
    shared_mem->read_guard = 1;



    //wait for the follower to signal that the shared memory region can be delete
    while(shared_mem->delete_guard == 0)
    {

    }
    shared_mem->delete_guard = 0;
    

    //unmapping
    munmap(shm_addr, sizeof(struct shared_mem_data));


    //unlinking
    shm_unlink(SHARED_MEM);


    return 0;
}
