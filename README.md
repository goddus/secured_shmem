# Secure Shared Memory 0.5

### Brief Summary of Files

- /src/secured_shmem.c : main library code
- /src/linked_list : linked list code
- /testing/leader.c and /testing/follower.c : original CSE422 Studio 12 code
- /testing/leader_test.c and /testing/follower_test.c : modified 422 code for testing

### The Shared Memory Library

void * open_shared_mem(const char *name, enum create_or_join action, enum access_options access, off_t size)

&nbsp;&nbsp;&nbsp;&nbsp; Return Value: void pointer pointing to the start of the memory region

&nbsp;&nbsp;&nbsp;&nbsp; Params: 

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; const char *name: name of the memory region

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; enum create_or_join action: flag specifying if the memory region is being created or joined

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; enum access_options access: flag specifying if the region is being opened for read or write access (or both)

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; off_t size: size of the memory region (ignored if the region is being joined)


void close_shared_mem(void *addr, size_t shm_size)

&nbsp;&nbsp;&nbsp;&nbsp; Params: 

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; void *addr: pointer to the start of the memory region

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; size_t shm_size: size of the memory region


void delete_shared_mem(const char *name)

&nbsp;&nbsp;&nbsp;&nbsp; Params: 

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; const char *name: name of the memory region

