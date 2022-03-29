

//TODO: figure out how to properly use a mode_t data type
int assemble_mode (enum access){

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


void *open_shared_mem (const char *name, enum create_or_join, enum access, off_t size){

    //shm_open()
    int access_level = assemble_mode(access);
    int shm_fd;

    //if user is creating region
    if (create_or_join == CREATE_REGION){

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
    else if (create_or_join == JOIN_REGION){

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
    if(shm_addr == -1){
        printf("error mapping to address space\n");
        return NULL;
    }

    //TODO: update the data structure

    return shm_addr;
    
}
