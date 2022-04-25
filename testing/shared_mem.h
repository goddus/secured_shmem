#define shared_mem_name "/shared_mem"
#define shared_mem_size 10

struct shared_data {
	volatile int join_guard;
	volatile int data[shared_mem_size];
};
