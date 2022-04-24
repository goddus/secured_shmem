#include "shared_mem.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <errno.h>

int main(){
    
    int ret_val;

    //unlink
    ret_val = shm_unlink(shared_mem_name);
    if(ret_val == -1){
        printf("failed to unlink region: %s\n", strerror(errno));
        return -1;
    }

    return 0;

}
