/*
 * leader_test.c: tests creation, closing and deletion of memory regions 
 *                  (to be used in complement with follower_test.c)
 *                  (modified from CSE422 Studio 12)
 *                
 */

#include "shared_mem.h"
#include "../src/secure_shmem.h"
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

	int i;
	struct shared_data *shmem;
	int arr[shared_mem_size];

	//init - should remove later
	init();

	shmem = (struct shared_data*)open_shared_mem(shared_mem_name, CREATE_REGION, BOTH, sizeof(struct shared_data));

	//initialize guard variables to 0
	shmem->write_guard = 0;
	shmem->read_guard = 0;
	shmem->delete_guard = 0;

	//wait for follower to be created
	while(shmem->write_guard == 0){}

	//fill in array
	srand(time(0));
	for(i = 0; i < shared_mem_size; i++){
		arr[i] = rand();
	}

	//copy into struct
	memcpy((void *)shmem->data, &arr, sizeof(int)*shared_mem_size);

	//tell follower to read
	shmem->read_guard = 1;

	//wait for follower to read
	while(shmem->delete_guard == 0){}

	//delete
	delete_shared_mem(shared_mem_name);	

	return 0;	

}
