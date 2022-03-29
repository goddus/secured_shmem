#include "shared_mem.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h> 	   	 /* For shm_open()*/
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */

int main(){
	
	int i, fd, ret_val;
	struct shared_data *shmem;
	int arr[shared_mem_size];

	//create shared memory
	fd = shm_open(shared_mem_name, O_RDWR | O_CREAT, S_IRWXU);
	if(fd == -1){
		printf("ERROR: failed to create shared memory region.\n");
		return 1;
	}

	//resize memory
	ret_val = ftruncate(fd, sizeof(struct shared_data));
	if(ret_val == -1){
		printf("ERROR: failed to resize memory region.\n");
		return 1;
	}

	//map to address space
	shmem = (struct shared_data*) mmap(NULL, sizeof(struct shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(shmem == MAP_FAILED){
		printf("ERROR: failed to map to address space.\n");
		return 1;
	}

	//initialize guard variables to 0
	shmem->write_guard = 0;
	shmem->read_guard = 0;
	shmem->delete_guard = 0;

	//wait for follower to be created
	while(shmem->write_guard == 0){}

	//fill in array
	srand(time(0));
	printf("LEADER: \n");
	for(i = 0; i < shared_mem_size; i++){
		arr[i] = rand();
		printf("%d\n", arr[i]); 
	} 
	
	//copy into struct
	memcpy((void *)shmem->data, &arr, sizeof(int)*shared_mem_size);

	//tell follower to read
	shmem->read_guard = 1;

	//wait for follower to read
	while(shmem->delete_guard == 0){}

	//unlink
	if(shm_unlink(shared_mem_name) != 0){
		printf("ERROR: failed to unlink.\n");
		return 1;
	}	

	return 0;	

}
