#include "free_list.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

int main(){

	int ret_val;

	//unlink the list
	ret_val = shm_unlink(regions_list_name);
	if(ret_val == -1){
		printf("failed to unlink list\n");
		return -1;
	}

	//unlink the lock
	ret_val = shm_unlink(regions_list_lock_name);
	if(ret_val == -1){
		printf("failed to unlink lock\n");
		return -1;
	}

	return 0;

}

