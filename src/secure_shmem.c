#include "secure_shmem.h"
#include "free_list/free_list.h"


//TODO: figure out how to properly use a mode_t data type
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
    //TODO: figure out how to return an error value from this

}


void *open_shared_mem (const char *name, enum create_or_join action, enum access_options access, off_t size){

    //TODO: lock the list

    //shm_open()
    int access_level = assemble_mode(access);
    int shm_fd;
    void *shm_addr;

    //if user is creating region
    if (action == CREATE_REGION){

        //check that there is room for the region
        if(control->size >= max_size){
            printf("error: out of space for new mem_region\n");
            //TODO: unlock the list
            return NULL;
        }

        //check that name is under max_name_length
        if(strlen(name)+1 >= max_name_length){
            printf("error: name is too long. Try with a shorter name\n");
            //TODO: unlock the list
            return NULL;
        }

        //check that the region doesn't exist
        if(search(control, name) != NULL){
            printf("error: region already exists. Try with a different name\n");
            //TODO: unlock the list
            return NULL;
        }

        //create region
        shm_fd = shm_open(name, access_level | O_CREAT, S_IRWXU);
        if (shm_fd < 0){
            printf("error creating a shared memory file descriptor\n");
            //TODO: unlock the list
            return NULL;
        }

        //resize region
        int ftruncate_ret_val = ftruncate(shm_fd, size);
        if (ftruncate_ret_val < 0){
            printf("error with ftruncate()\n");
            //TODO: unlock the list
            return NULL;
        }

        //map region to address space
        shm_addr = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, shm_fd, 0);
        if(shm_addr == (void*)-1){
            printf("error mapping to address space\n");
            //TODO: unlock the list
            return NULL;
        }


        //add a new region to the mem regions list
        struct mem_region *new_region = new_node(control);
        strcpy(new_region->name, name);
        new_region->fd = shm_fd;
        new_region->address = shm_addr;
        new_region->size = size;
    }

    //if user is joining region
    else if (action == JOIN_REGION){

        //check that region exists
        struct mem_region *to_join = search(control, name);
        if(to_join == NULL){
            printf("error: memory region does not exist\n");
            //TODO: unlock the list
            return NULL;
        }

        //check that there is room for the process
        if(to_join->user_count >= max_users){
            printf("error: too many users, could not join\n");
            //TODO: unlock the list
            return NULL;
        }

        //open region
        shm_fd = shm_open(name, access_level, S_IRWXU);
        if (shm_fd < 0){
            printf("error creating a shared memory file descriptor\n");
            //TODO: unlock the list
            return NULL;
        }

        //map region to address space
        shm_addr = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, shm_fd, 0);
        if(shm_addr == (void*)-1){
            printf("error mapping to address space\n");
            //TODO: unlock the list
            return NULL;
        }
    }

    else{
        errno = EINVAL;    
    }

    //check list variables
    printf("Actual:\n");
    printf("name: %s\n", name);
    printf("fd: %d\n", shm_fd);
    printf("size: %zu\n", size);
    printf("address: %p\n", shm_addr);
    printf("------------------------------\n");
    print_list(control);

    //TODO:add user to user list
    

    //TODO: unlcok the list
    
    return shm_addr;

}


void close_shared_mem(void* addr, size_t shm_size){

    //TODO: lock the list
    
    //check that the region exists
    

    //munmap()
    munmap(addr, shm_size);

    //close() the file descriptor

    //update the linked list data structure

}

void delete_shared_mem(const char *name){

    //TODO: verify that this is the last user of the shm region

    //shm_unlink() the shm region
    shm_unlink(name);

    //TODO: update the data structure
}
