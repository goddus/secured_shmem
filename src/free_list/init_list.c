#include "free_list.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntrl.h>

int main(){
    
    int list_fd, lock_fd, ret_val;

    //create list memory region
    list_fd = shm_open(regions_list_name, O_RDWR | O_CREAT, S_IRWXU);
    if(list_fd == -1){
        printf("error: failed to create list memory region\n");
        return -1;
    }

    //resize
    ret_val = ftruncate(list_fd, sizeof(mem_regions_list));
    if(ret_val == -1){
        printf("error: failed to resize list memory region\n");
    }

    //create list lock memory region
    lock_fd = shm_open(regions_list_lock_name, O_RDWR | O_CREAT, S_IRWXU);
    if(lock_fd == -1){
        printf("error: failed to create list lock region\n");
    }

    //resize
    ret_val = ftruncate(lock_fd, sizeof(volatile int));
    if(lock_fd == -1){
        printf("error: failed to resize list lock region\n");
    }

    return 0;
}
