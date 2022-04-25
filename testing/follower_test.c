#include "shared_mem.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h> 	   	 /* For shm_open()*/
#include <sys/stat.h>        /* For mode constants */
#include "../src/secure_shmem.h"
#include <fcntl.h>           /* For O_* constants */
#include <time.h> 	/* For time() */

int main(){

	int i, fd, ret_val;
	struct shared_data *shmem;
	int arr[shared_mem_size];
	struct timespec start, end;
	time_t time_seconds;
	long time_nseconds;

	//init
	init();

	shmem = (struct shared_data *) open_shared_mem(shared_mem_name, JOIN_REGION, BOTH, sizeof(struct shared_data));

	clock_gettime(CLOCK_REALTIME, &start);

	//notify leader to write and wait
	shmem->write_guard = 1;
	while(shmem->read_guard == 0){}

	//copy into struct
	memcpy(&arr, (void *)shmem->data, sizeof(int)*shared_mem_size);

	//print data
	/*printf("FOLLOWER:\n");
	for(i = 0; i < shared_mem_size; i++){
		printf("%d\n", arr[i]);
	}*/


	//notify leader that it can destroy shared memory
	shmem->delete_guard = 1;

	clock_gettime(CLOCK_REALTIME, &end);

	if((end.tv_nsec - start.tv_nsec) < 0){
		time_seconds = (end.tv_sec - start.tv_sec) - 1;
		time_nseconds = 1000000000 + (end.tv_nsec - start.tv_nsec); 
	}
	else {
		time_seconds = (end.tv_sec - start.tv_sec);
		time_nseconds = (end.tv_nsec - start.tv_nsec);	
	}

	//printf("With a array size of %d, the data transfer through shared memory took %d seconds and %ld nanoseconds.\n", shared_mem_size, time_seconds, time_nseconds);	

	//munmap
	close_shared_mem(shared_mem_name, shmem);

	return 0;	

}
