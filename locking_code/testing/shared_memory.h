#define SHARED_MEM "/shared_mem_name1"
#define SHARED_ARRAY_SIZE 10

struct shared_mem_data 
{
    volatile int write_guard;
    volatile int read_guard;
    volatile int delete_guard;
    volatile int data[SHARED_ARRAY_SIZE];
};
