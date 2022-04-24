#include "free_list.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

int main(){
    
    int list_fd, lock_fd, ret_val;

    //create list memory region
    list_fd = shm_open(regions_list_name, O_RDWR | O_CREAT, S_IRWXU);
    if(list_fd == -1){
        printf("error: failed to create list memory region\n");
        return -1;
    }

    //resize
    ret_val = ftruncate(list_fd, sizeof(struct mem_regions_list));
    if(ret_val == -1){
        printf("error: failed to resize list memory region\n");
	return -1;
    }

    //create list lock memory region
    lock_fd = shm_open(regions_list_lock_name, O_RDWR | O_CREAT, S_IRWXU);
    if(lock_fd == -1){
        printf("error: failed to create list lock region\n");
	return -1;
    }

    //resize
    ret_val = ftruncate(lock_fd, sizeof(volatile int));
    if(lock_fd == -1){
        printf("error: failed to resize list lock region\n");
	return -1;
    }

    //map the list
    struct mem_regions_list *regions_list = (struct mem_regions_list *) mmap(NULL, sizeof(struct mem_regions_list), PROT_READ | PROT_WRITE, MAP_SHARED, list_fd, 0);
    if(regions_list == MAP_FAILED){
	printf("error: failed to map list (init)\n");
	return -1;
    }

    //initialize the list
    if(initialize_list(&(regions_list->control), regions_list->regions)!=0){
	printf("error: failed to initialize mem_regions_list\n");
	return -1;
    }

    return 0;
}
