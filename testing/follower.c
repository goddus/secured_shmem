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
    int shared_mem_fd = shm_open(SHARED_MEM, O_RDWR, S_IRWXU);


    //error handling for shm_open()
    if (shared_mem_fd < 0)
    {
        printf("there was an error creating a shared memory file descriptor\n");
        return shared_mem_fd;
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


    //declaring the local array 
    int the_array[SHARED_ARRAY_SIZE];

    
    //signal to the leader that they can start writing
    shared_mem->write_guard = 1;


    //wait for the leader to signal that it can read from the shared data
    while (shared_mem->read_guard == 0)
    {

    }
    shared_mem->read_guard = 0;


    //copying the shared memory array contents into the local array element-by-element
    for (i = 0; i < SHARED_ARRAY_SIZE; ++i)
    {
        the_array[i] = shared_mem->data[i];
    }

    //printing the follower's local array
    for (i = 0; i < SHARED_ARRAY_SIZE; ++i)
    {
        printf("element %d: %d\n", i, the_array[i]);
    }
    
    
    //signal to the leader that it can delete the shared memory region
    shared_mem->delete_guard = 1;


    //unmapping
    munmap(shm_addr, sizeof(struct shared_mem_data));

    
    //unlinking
    shm_unlink(SHARED_MEM);


    return 0;
}
