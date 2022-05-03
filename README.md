# Secure Shared Memory v0.01 

Secure Shared Memory is a library wrapping the POSIX shared memory interface that provides a built-in locking mechanism.

## Usage/Setup

1. Compile the library using `make` from the /testing folder (or a new folder in the same directory as the /src folder)

2. Make sure ASLR is disabled

3. Run init_list.c:
```C
./init_list
```

4. In your C file:
```C
#include "secure_shmem.c"

init();

//any Secured Shared Memory functions
```

<br />

## Functions

### Open or Join a Memory Region

```C
void *open_shared_mem (const char *name, enum create_or_join action, enum access_options access, off_t size)
```
##### Parameters

```name``` name of the memory region to create or join (must be under 15 characters)

```action``` flag specifying if region should be created or joined

```access``` flag specifying the access option (read, write or both)

```size``` size of the memory region to be created. Ignored if region is being joined

##### Return Value

```void *``` to the start of the memory region

<br />

### Read from a Memory Region

```C
int read_shm(void *dest, void *src, size_t num_bytes, int access_num)
```
##### Parameters

```dest``` destination address where to read into

```src``` address of the data to be read

```num_bytes``` number of bytes to be read

```access_num``` integer representing where in the order of operations to perform the read

##### Return Value

```int``` return non-zero integer upon failure

<br />

### Write to a Memory Region

```C
int write_shm(void *dest, void *src, size_t num_bytes, int access_num)
```
##### Parameters

```dest``` destination address where to written to

```src``` address of the data to be written

```num_bytes``` number of bytes to be written

```access_num``` integer representing where in the order of operations to perform the read

##### Return Value

```int``` return non-zero integer upon failure

<br />

### Close a Memory Region

```C
void close_shared_mem (char *name, void *addr)
```
##### Parameters

```name``` name of the memory region to close

```addr``` pointer to the start of the memory region

<br />

### Delete a Memory Region

```C
void delete_shared_mem (char *name)
```
##### Parameters

```name``` name of the memory region to delete
