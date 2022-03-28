

//TODO: figure out how to properly use a mode_t data type
int assemble_mode (enum access)
{
    int mem_region_mode;
    
    if (access == READ)
    {
        mem_region_mode = O_RDONLY;
    }
    else if (access == WRITE)
    {
        mem_region_mode = O_WRONLY;
    }
    else if (access == BOTH)
    {
        mem_region_mode = O_RDWR;
    }
   
    return mem_region_mode;
    //TODO: figure out how to return an error value from this

}


int open_shared_mem (const char *name, enum create_or_join, enum access, off_t size)
{

    //shm_open()
    int access_level = assemble_mode(access);
    int shm_fd;
    
    if (create_or_join == CREATE_REGION)
    {
        shm_fd = shm_open(name, access_level | O_CREAT, S_IRWXU);
        if (shm_fd < 0)
        {
            printf("there was an error creating a shared memory file descriptor\n");
            return shm_fd;
        }

        //if creating the memory region for the first time, size the region with ftruncate()
        int ftruncate_ret_val = ftruncate(shm_fd, size);

        //error handling for ftruncate()
        if (ftruncate_ret_val < 0)
        {
            printf("there was an error with ftruncate()\n");
            return -1;
        }
    }
    else if (create_or_join == JOIN_REGION)
    {
        shm_fd = shm_open(name, access_level, S_IRWXU);
        if (shm_fd < 0)
        {
            printf("there was an error creating a shared memory file descriptor\n");
            return shm_fd;
        }
    }
    else
    {
        errno = EINVAL;    
    }


    //mmap()
    char* shm_addr = mmap(NULL, size, PROT_READ|PROT_WRITE, 

    
}
