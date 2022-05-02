/*
 *  secure_shmem.c: main secure shm library 
 *                  (for creation, reading/writing, closing and deleting)
 */

#include "secure_shmem.h"
#include "free_list/free_list.h"



/*
 *  init: meant to be called before any other library function
 *          maps data structures relevant to the library
 */
int init(){

	int list_fd, lock_fd, ret_val;

	//open and map region list
	list_fd = shm_open(regions_list_name, O_RDWR, S_IRWXU);
	if(list_fd == -1){
		printf("error: failed to join list memory region\n");
		return MAPPING_FAILURE;
	}

	//map to address space
	regions_list = (struct mem_regions_list *) mmap(NULL, sizeof(struct mem_regions_list), PROT_READ | PROT_WRITE, MAP_SHARED, list_fd, 0);
	if(regions_list == MAP_FAILED){
		printf("error: failed to map list to address space\n");
		return MAPPING_FAILURE;
	}

	//open and map region lock
	lock_fd = shm_open(regions_list_lock_name, O_RDWR, S_IRWXU);
	if(lock_fd == -1){
		printf("error: failed to join lock memory region\n");
		return MAPPING_FAILURE;
	}

	//map to address space
	regions_list_lock = (volatile int *) mmap(NULL, sizeof(volatile int), PROT_READ | PROT_WRITE, MAP_SHARED, lock_fd, 0);
	if(regions_list_lock == MAP_FAILED){
		printf("error: failed to map lock to address space\n");
		return MAPPING_FAILURE;
	}

	return SUCCESS;

}

/*
 *  assemble_mode: return correct access option for
 *                  opening shared mem
 */
int assemble_mode (enum access_options access){

    if (access == READ){
        return O_RDONLY;
    }
    else if (access == WRITE){
        return O_WRONLY;
    }
    else if (access == BOTH){
        return O_RDWR;
    }
    else {
        return INVALID_ACCESS;
    }

}

/*
 *  add_user: adds user pid to region's user list
 */
void add_user(pid_t pid, struct mem_region *region){
    int i;
	region->users[region->next_open_idx] = pid;
	for(i = 0; i < max_users; i++){
		if(region->users[i] == 0){
			region->next_open_idx = i;
			break;
		}
	}	
}

/*
 *  delete_user: removes user pid from region's user list
 */
void delete_user(pid_t pid, struct mem_region *region){
    int i;
	for(i = 0; i < max_users; i++){
		if(region->users[i] == pid){
			region->users[i] = 0;
			region->next_open_idx = i;
			break;
		}
	}
}

/*
 * open_shared_mem: creates or opens a secured shared memory region
 */
void *open_shared_mem (const char *name, enum create_or_join action, enum access_options access, off_t size){

    //TODO: lock the list

    //shm_open()
    int access_level = assemble_mode(access);
    int shm_fd;
    void *shm_addr;

    //if user is creating region
    if (action == CREATE_REGION){

        //check that there is room for the region
        if(regions_list->control.size >= max_regions){
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
        if(search(&(regions_list->control), name) != NULL){
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
        struct mem_region *new_region = new_node(&(regions_list->control));
        strcpy(new_region->name, name);
        new_region->fd = shm_fd;
        new_region->address = shm_addr;
        new_region->size = size;
	    new_region->user_count++;
	    add_user(getpid(), new_region);
    }

    //if user is joining region
    else if (action == JOIN_REGION){

        //check that region exists
        struct mem_region *to_join = search(&(regions_list->control), name);
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

	    //update data structure
	    to_join->user_count++;
	    add_user(getpid(), to_join);
    }

    else{
        printf("invalid create option\n");
        return NULL;
    }

    //check list variables
    printf("Actual:\n");
    printf("name: %s\n", name);
    printf("fd: %d\n", shm_fd);
    printf("size: %zu\n", size);
    printf("address: %p\n", shm_addr);
    printf("------------------------------\n");
    print_list(&(regions_list->control));

    //TODO: unlock the list
    
    return shm_addr;

}

/*
 *  close_shared_mem: closes the secured memory region
 */
void close_shared_mem(char *name, void* addr){

    //TODO: lock the list
    
    //check that region exists
    struct mem_region *to_close = search(&(regions_list->control), name);
    if(to_close == NULL){
    	printf("error: memory region does not exist\n");
        //TODO: unlock the list
        return;
    }
    
    //update data structure
    to_close->user_count--;
    delete_user(getpid(), to_close);


    //munmap()
    if(munmap(addr, to_close->size) == -1){
	    printf("error unmapping while closing\n");
	    //TODO: unlock the list
	    return;
    }

    //TODO: unlock the list
}


/*
 *  delete_shared_mem: deletes the shared memory region
 */
void delete_shared_mem(const char *name){
 
    //TODO: lock the list

    
    //check that region exists
    struct mem_region *to_delete = search(&(regions_list->control), name);
    if(to_delete == NULL){
    	printf("error: memory region does not exist\n");
        //TODO: unlock the list
        return;
    }

    //verify that this is the last user of the shm region
    if(to_delete->user_count != 1){
	    printf("error: all processes have not closed the region\n");
	    //TODO: unlock the list
	    return;
    }

    //unmap the region
    if(munmap(to_delete->address, to_delete->size) == -1){
	    printf("error unmapping while deleting\n");
	    //TODO: unlock the list
	    return;
    }

    //close the file descriptor
    if(close(to_delete->fd) == -1){
	    printf("error closing while deleting\n");
	    //TODO: unlock the list
	    return;
    }

    //shm_unlink() the shm region
    if(shm_unlink(name) == -1){
	    printf("error unlinking while deleting\n");
	    //TODO: unlock the list
	    return;
    }

    //update the data structure
    if(return_to_free(&(regions_list->control), to_delete) == -1){
	    printf("error removing region from list\n");
	    return;
    } 

    //check output
    print_list(&(regions_list->control));


    //TODO: unlock the list

}
