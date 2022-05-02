# Secure Shared Memory v0.01

Secure Shared Memory is a library wrapping the POSIX shared memory interface that provides a built-in locking mechanism.

## Usage/Setup

1. Compile the library using `make` from the /testing folder

2. Run init_cleanup.c:
```C
./init_cleanup
```

3. In your C file:
```C
#include "secure_shmem.c"

init();

//any Secured Shared Memory functions
```

## Functions

#### Open or Join a Shared Memory Region
```C
void *open_shared_mem (const char *name, enum create_or_join action, enum access_options access, off_t size)
```
##### Parameters

```name``` name of the memory region to create or join (must be under 15 characters)

```action``` flag specifying if region should be created or joined

```access``` flag specifying the access option (read, write or both)

```size``` size of the memory region to be created. Ignored if region is being joined

##### Return Value

void pointer to the start of the memory region
